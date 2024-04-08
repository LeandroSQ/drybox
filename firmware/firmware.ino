#include "utils/configuration.hpp"
#include "utils/accumulator.hpp"
#include "utils/lcd-printer.hpp"
#include "utils/pid.hpp"
#include <Thermistor.h>
#include <Wire.h>
#include <DHT.h>
#include <PWM.h>

/* DHT module */
DHT dht(DHT_PIN, DHT_TYPE);
Accumulator boxTemperature;
Accumulator boxHumidity;
float boxAbsoluteHumidity = 0.0f;
float boxWaterVaporMass = 0.0f;
unsigned long lastDHTUpdate = 0UL;

/* Thermistor module */
Thermistor thermistor(THERMISTOR_PIN);
Accumulator hotEndTemperature;
unsigned long lastThermistorUpdate = 0UL;

/* Fan module */
float overrideFanSpeed = -1.0f;
float fanSpeed = 0.0f;
float currentFanSpeed = 0.0f;
PID fanPID(0.3f, 0.005f, 0.01f, HEATER_SETPOINT, FAN_MIN_SPEED, FAN_MAX_SPEED, PID::Direction::REVERSE);
unsigned long lastFanUpdate = 0UL;
unsigned long lastFanSpeedUpdate = 0UL;

/* Hotend module */
float hotEndOutput = 0;
float overrideHeaterOutput = -1.0f;
PID heaterPID(4.5f, 1.15f, 0.15f, HEATER_SETPOINT, 0.0, 255.0, PID::Direction::DIRECT);
unsigned long lastHeaterUpdate = 0UL;

/* LCD module */
LCDPrinter lcd;
unsigned short lcdInfoIndex = 0;
unsigned long lastLCDUpdate = 0UL;

/* GUI */
unsigned long lastIOUpdate = 0UL;

void setup() {
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
    updateThermistor();
    updateHeater();
    updateDHT();
    updateFan();
    updateLCD();
    updateIO();
}
