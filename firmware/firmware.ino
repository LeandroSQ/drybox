#include <DHT.h>
#include <Thermistor.h>
#include "configuration.hpp"
#include "accumulator.hpp"
#include "PID.hpp"
#include <PWM.h>

/* Pins */
#define DHT_PIN 2 /* Digital */
#define THERMISTOR_PIN 0 /* Analog */
#define FAN_PIN 10 /* Digital PWM */
#define HEATER_PIN 11 /* Digital PWM */

/* Constants */
#define DHT_TYPE DHT11

/* Temperature controls */
DHT dht(DHT_PIN, DHT_TYPE);
Thermistor thermistor(THERMISTOR_PIN);
Accumulator hotEndTemperature;
Accumulator boxTemperature;
Accumulator boxHumidity;
float boxAbsoluteHumidity = 0.0f;
float boxWaterVaporMass = 0.0f;
unsigned long lastDHTUpdate = 0;
unsigned long lastThermistorUpdate = 0;

/* Hot end controls */
PID heaterPID(0.5f, 1.5f, 0.5f, HEATER_SETPOINT, 0.0, 255.0, DIRECT);
unsigned long lastHeaterUpdate = 0UL;

/* Fan controls */
float fanSpeed = 0.0f;
float currentFanSpeed = 0.0f;
// PID fanPID(0.32f, 2.0f, 0.75f, HEATER_SETPOINT, FAN_MIN_SPEED, FAN_MAX_SPEED, REVERSE);
unsigned long lastFanUpdate = 0UL;
unsigned long lastFanSpeedUpdate = 0UL;

/* GUI controls */
unsigned long lastGUIUpdate = 0UL;

void setup() {
    Serial.begin(9600);
	log("Drybox starting up...");

    if (DHT_ENABLED) {
	    log("Setting up DHT sensor...");
	    dht.begin();
    }

    if (THERMISTOR_ENABLED) {
	    log("Setting up thermistor sensor...");
	    //pinMode(A1, INPUT);
    }

    if (HEATER_ENABLED) {
	    log("Setting up heater...");
	    pinMode(HEATER_PIN, OUTPUT);
    }

    if (FAN_ENABLED) {
        log("Setting up fan...");
        InitTimersSafe();
        if (!SetPinFrequencySafe(FAN_PIN, 31372.55f)) {
            log("Failed to set fan frequency!");
        }
        pinMode(FAN_PIN, OUTPUT);
        pwmWrite(FAN_PIN, 0);
    }

    log("Drybox ready!");

    // Setup the timers
    lastDHTUpdate = millis() + DHT_UPDATE_INTERVAL;
    lastThermistorUpdate = millis() + THERMISTOR_UPDATE_INTERVAL;
    lastHeaterUpdate = millis() + HEATER_UPDATE_INTERVAL;
    lastFanUpdate = millis() + FAN_UPDATE_INTERVAL;
    lastGUIUpdate = millis() + GUI_UPDATE_INTERVAL;

    delay(1000);
}

void loop() {
    unsigned long now = millis();
    updateThermistor();
    updateDHT();
    updateHeater();
    updateFan();
    updateSerialInput();
    updateGUI();
}

void updateThermistor() {
    if (!THERMISTOR_ENABLED) return;
    if (millis() - lastThermistorUpdate < THERMISTOR_UPDATE_INTERVAL) {
        return;
    }
    lastThermistorUpdate = millis();

    float temperature = thermistor.getTemp();
    if (isnan(temperature) || temperature <= 0.0f) {
        log("Failed to read thermistor temperature!");
        return;
    }

    hotEndTemperature.set(temperature - 5.0f);
}

void updateDHT() {
    if (!DHT_ENABLED) return;
    if (millis() - lastDHTUpdate < DHT_UPDATE_INTERVAL) return;
    lastDHTUpdate = millis();

    float temperature = dht.readTemperature();
    if (isnan(temperature) || temperature <= 0.0f) {
        log("Failed to read DHT temperature!");
        return;
    }

    float humidity = dht.readHumidity();
    if (isnan(humidity) || humidity <= 0.0f) {
        log("Failed to read DHT humidity!");
        return;
    }

    if (abs(temperature) / abs(boxTemperature.value) > 2.0f || abs(humidity) / abs(boxHumidity.value) > 2.0f) {
        log("DHT values are too far off, ignoring...");
        return;
    }

    boxTemperature.set(temperature);
    boxHumidity.set(humidity);
    calculateAbsoluteHumidity(temperature, humidity, boxAbsoluteHumidity, boxWaterVaporMass);
}

void updateHeater() {
    if (!HEATER_ENABLED) return;
    if (millis() - lastHeaterUpdate < HEATER_UPDATE_INTERVAL) return;
    lastHeaterUpdate = millis();

    if (hotEndTemperature.value >= MAX_HEATER_TEMP) {
        analogWrite(HEATER_PIN, 0);
        return;
    }

    heaterPID.update(boxTemperature.value);
    analogWrite(HEATER_PIN, heaterPID.output);
}

void updateFan() {
    if (!FAN_ENABLED) return;

    // Smooth out fan speed changes
    if (millis() - lastFanSpeedUpdate > FAN_SPEED_UPDATE_INTERVAL) {
        currentFanSpeed += (fanSpeed - currentFanSpeed) * 0.1;
        lastFanSpeedUpdate = millis();
        pwmWrite(FAN_PIN, currentFanSpeed);
    }

    // Set the fan target output
    if (millis() - lastFanUpdate < FAN_UPDATE_INTERVAL) return;
    lastFanUpdate = millis();

    // Override
    if (overrideFanSpeed > 0.0f) {
        pwmWrite(FAN_PIN, overrideFanSpeed);
        fanSpeed = overrideFanSpeed;
        return;
    }

    if (hotEndTemperature.value >= MAX_HEATER_TEMP) {
        // Safety override
        fanSpeed = FAN_MAX_SPEED;
        currentFanSpeed = FAN_MAX_SPEED;
    } else if (hotEndTemperature.value < HEATER_SETPOINT / 2.0f) {
        // Enable faster heat up
        fanSpeed = 0;
    } else if (boxTemperature.value < HEATER_SETPOINT * 0.8) {
        // Close to the target, slow down the fan
        fanSpeed = (1.0f - boxTemperature.value / HEATER_SETPOINT) * (FAN_MAX_SPEED - FAN_MIN_SPEED) + FAN_MIN_SPEED;
    } else {
        // Stable state
        fanSpeed = FAN_MIN_SPEED;
        // fanPID.update(hotEndTemperature.value, 0.4, boxTemperature.value, 0.6);
        // fanSpeed = fanPID.output;
    }
}

void updateGUI() {
    if (millis() - lastGUIUpdate < GUI_UPDATE_INTERVAL) return;
    lastGUIUpdate = millis();

    #ifdef PLOT
        Serial.flush();

        if (FAN_ENABLED) {
            Serial.print(fanSpeed);
            Serial.print(",");
            Serial.print(fanSpeed / FAN_MAX_SPEED * 100.0);
            Serial.print(",");
        }

        if (THERMISTOR_ENABLED) {
            Serial.print(hotEndTemperature.value);
            Serial.print(",");
        }

        if (HEATER_ENABLED) {
            Serial.print(HEATER_SETPOINT);
            Serial.print(",");
            Serial.print((heaterPID.output * 100.0) / 255.0);
            Serial.print(",");
        }

        if (DHT_ENABLED) {
            Serial.print(boxTemperature.value);
            Serial.print(",");
            Serial.print(boxHumidity.value);
            Serial.print(",");
            Serial.print(boxAbsoluteHumidity);
            Serial.print(",");
            Serial.print(boxWaterVaporMass);
        }

        Serial.println();
    #else
        if (FAN_ENABLED) {
            Serial.print("Fan speed: ");
            Serial.print(fanSpeed / FAN_MAX_SPEED * 100.0);
            Serial.print("% ");
        }

        if (THERMISTOR_ENABLED) {
            Serial.print("Hotend temperature: ");
            Serial.print(hotEndTemperature.value);
            Serial.print("°C ");
        }

        if (HEATER_ENABLED) {
            Serial.print("Heater setpoint: ");
            Serial.print(HEATER_SETPOINT);
            Serial.print("°C ");
            Serial.print("Heater output: ");
            Serial.print((heaterPID.output * 100.0) / 255.0);
            Serial.print("% ");
        }

        if (DHT_ENABLED) {
            Serial.print("Box temperature: ");
            Serial.print(boxTemperature.value);
            Serial.print("°C, Box humidity: ");
            Serial.print(boxHumidity.value);
            Serial.print("%, Box absolute humidity: ");
            Serial.print(boxAbsoluteHumidity);
            Serial.print("g/m³ ");
            Serial.print("Box water vapor mass: ");
            Serial.print(boxWaterVaporMass);
            Serial.print("g ");
        }

        Serial.println();
    #endif
}

void updateSerialInput() {
    if (Serial.available() > 0) {
        String input = Serial.readString();
        // FAN <KP> <KI> <KD>
        if (input.startsWith("FAN")) {
            float kp, ki, kd;

            int index = input.indexOf(" ");
            if (index == -1) {
                log("Invalid FAN command!");
                return;
            }

            input = input.substring(index + 1);
            kp = input.substring(0, input.indexOf(" ")).toFloat();
            input = input.substring(input.indexOf(" ") + 1);
            ki = input.substring(0, input.indexOf(" ")).toFloat();
            input = input.substring(input.indexOf(" ") + 1);
            kd = input.toFloat();

            fanPID.kp = kp;
            fanPID.ki = ki;
            fanPID.kd = kd;
            fanPID.reset();

            Serial.print("Fan PID updated: ");
            Serial.print(fanPID.kp);
            Serial.print(" ");
            Serial.print(fanPID.ki);
            Serial.print(" ");
            Serial.println(fanPID.kd);
        }

        // SPEED <VALUE>
        if (input.startsWith("SPEED")) {
            float speed = input.substring(input.indexOf(" ") + 1).toFloat();
            overrideFanSpeed = speed;
            Serial.print("Fan speed updated: ");
            Serial.println(overrideFanSpeed);
        }

        // HEATER <KP> <KI> <KD>
        if (input.startsWith("HEATER")) {
            float kp, ki, kd;

            int index = input.indexOf(" ");
            if (index == -1) {
                log("Invalid HEATER command!");
                return;
            }

            input = input.substring(index + 1);
            kp = input.substring(0, input.indexOf(" ")).toFloat();
            input = input.substring(input.indexOf(" ") + 1);
            ki = input.substring(0, input.indexOf(" ")).toFloat();
            input = input.substring(input.indexOf(" ") + 1);
            kd = input.toFloat();

            heaterPID.kp = kp;
            heaterPID.ki = ki;
            heaterPID.kd = kd;
            heaterPID.reset();

            Serial.print("Heater PID updated: ");
            Serial.print(heaterPID.kp);
            Serial.print(" ");
            Serial.print(heaterPID.ki);
            Serial.print(" ");
            Serial.println(heaterPID.kd);
        }

        // SETPOINT <VALUE>
        if (input.startsWith("SETPOINT")) {
            float setPoint = input.substring(input.indexOf(" ") + 1).toFloat();
            heaterPID.setPoint = setPoint;
            Serial.print("SetPoint updated: ");
            Serial.println(heaterPID.setPoint);
        }
    }
}