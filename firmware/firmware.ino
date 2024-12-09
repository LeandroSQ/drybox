#include "utils/configuration.hpp"
#include "utils/accumulator.hpp"
#include "utils/lcd-printer.hpp"
#include "utils/hardware.hpp"
#include "utils/pid.hpp"
#include <Thermistor.h>
#include <Wire.h>
#include <DHT.h>
#include <PWM.h>
#include <avr/wdt.h>

/* DHT module */
DHT dht(DHT_PIN, DHT_TYPE);
Accumulator boxTemperature;
Accumulator boxHumidity;
float boxAbsoluteHumidity = 0.0f;
float boxWaterVaporMass = 0.0f;
float boxMaxWaterVaporMass = 0.0f;
unsigned long lastDHTUpdate = 0UL;
unsigned char invalidDHTReads = 0;

/* Thermistor module */
Thermistor thermistor(THERMISTOR_PIN);
Accumulator hotEndTemperature;
unsigned long lastThermistorUpdate = 0UL;
unsigned char invalidThermistorReads = 0;

/* Fan module */
float overrideFanSpeed = -1.0f;
float fanSpeed = 0.0f;
float currentFanSpeed = 0.0f;
unsigned long lastFanUpdate = 0UL;
unsigned long lastFanSpeedUpdate = 0UL;

/* Hotend module */
float hotEndOutput = 0.0f;
float overrideHeaterOutput = -1.0f;
uint8_t lastHighTemp = 0;
bool isWarmingUp = true;
PID heaterPID(5.5f, 0.04f, 0.015f, 0.850f, HEATER_SETPOINT, 0.0, 255.0, PID::Direction::DIRECT);
unsigned long lastHeaterUpdate = 0UL;

/* LCD module */
LCDPrinter lcd;
uint8_t i2cTimeoutCount = 0;
unsigned long lastLCDUpdate = 0UL;

/* GUI */
unsigned long lastIOUpdate = 0UL;

void setup() {
    // Start the watchdog timer with a 4s timeout
    wdt_enable(WDTO_4S);

    Serial.begin(9600);
	log("Drybox starting up...");

    if constexpr (DHT_ENABLED) {
	    log("Setting up DHT sensor...");
	    dht.begin();
    }

    if constexpr (HEATER_ENABLED) {
	    log("Setting up heater...");
	    pinMode(HEATER_PIN, OUTPUT);
        analogWrite(HEATER_PIN, 0);
    }

    if constexpr (FAN_ENABLED) {
        log("Setting up fan...");
        InitTimersSafe();
        if (!SetPinFrequencySafe(FAN_PIN, 31372.55f)) {
            log("Failed to set fan frequency!");
        }
        pinMode(FAN_PIN, OUTPUT);
        pwmWrite(FAN_PIN, 0);
    }

    if constexpr (LCD_ENABLED) {
        log("Setting up LCD...");
        setupLCD();
    }

    log("Drybox ready!");

    // Setup the timers
    lastThermistorUpdate = millis() + THERMISTOR_UPDATE_INTERVAL;
    lastDHTUpdate = millis() + DHT_UPDATE_INTERVAL;

    delay(1000);
}

void loop() {
    wdt_reset();

    updateThermistor();
    updateDHT();

    // Ensure we don't have too many invalid reads
    if (invalidDHTReads >= MAX_INVALID_READS || invalidThermistorReads >= MAX_INVALID_READS) return safetyOverride();

    updateHeater();
    updateFan();
    updateLCD();
    updateIO();
}

inline void safetyShutdown() {
    // For safety, turn off the heater and fan
    if constexpr (HEATER_ENABLED) {
        analogWrite(HEATER_PIN, 0);
    }

    if constexpr (FAN_ENABLED) {
        analogWrite(FAN_PIN, 0);
    }
}

void safetyOverride() {
    // Disable the watchdog timer
    wdt_disable();

    log("Too many invalid reads, safety override!");
    safetyShutdown();

    // Serial output
    Serial.println("Failure!");

    // LCD output
    if constexpr (LCD_ENABLED) {
        lcd.clear();
        lcd.printCentered("Safety override!");
        bool dhtFailure = invalidDHTReads >= MAX_INVALID_READS;
        bool thermistorFailure = invalidThermistorReads >= MAX_INVALID_READS;
        lcd.setCursor(0, 1);
        if (dhtFailure) lcd.print("DHT");
        if (dhtFailure && thermistorFailure) lcd.print(" & ");
        if (thermistorFailure) lcd.print("Therm");
    }

    // Delay 20 seconds and blink the LCD backlight
    for (int i = 0; i < 20; i++) {
        delay(500);
        lcd.setBacklight(false);
        delay(500);
        lcd.setBacklight(true);
    }

    // Reset the board
    asm volatile ("  jmp 0");
}

// Watchdog timer interrupt
ISR(WDT_vect) {
    // For safety, turn off the heater and fan
    safetyShutdown();

    // Reset the board
    asm volatile ("  jmp 0");
}