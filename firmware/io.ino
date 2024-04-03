void updateIO() {
    if constexpr (!IO_ENABLED) return;
    if (millis() - lastIOUpdate < IO_UPDATE_INTERVAL) return;
    lastIOUpdate = millis();

    Serial.flush();
    if constexpr (IO_VERBOSE) {
        printVerboseStats();
    } else {
        printStats();
    }
    Serial.println();
}

inline void printVerboseStats() {
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
}

inline void printStats() {
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
}

inline void log(String message) {
	if constexpr (IO_VERBOSE) {
		Serial.println(message);
    }
}
