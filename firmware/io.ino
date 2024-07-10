void updateIO() {
    if constexpr (!IO_ENABLED) return;
    if (millis() - lastIOUpdate < IO_UPDATE_INTERVAL) return;
    lastIOUpdate = millis();

    handleSerialInput();

    Serial.flush();
    if constexpr (IO_VERBOSE) {
        printVerboseStats();
    } else {
        printStats();
    }
    Serial.println();
}

inline void handleSerialInput() {
    if (Serial.available() <= 0) return;
    String input = Serial.readString();

    // SAFE <BOOL>
    // if (input.startsWith("SAFE")) {
    //     bool safe = input.substring(input.indexOf(" ") + 1).toInt();
    //     enableHeaterSafeTemperature = safe;
    //     Serial.print("Safe temperature enabled: ");
    //     Serial.println(enableHeaterSafeTemperature);
    // }

    // FAN <VALUE>
    if (input.startsWith("FAN")) {
        float speed = input.substring(input.indexOf(" ") + 1).toFloat();
        overrideFanSpeed = speed;
        Serial.print("Fan speed updated: ");
        Serial.println(overrideFanSpeed);
    }

    // PID <TARGET> <KP> <KI> <KD>
    if (input.startsWith("PID")) {
        float kp, ki, kd, ff;

        int index = input.indexOf(" ");
        if (index == -1) {
            log("Invalid PID command!");
            return;
        }

        input = input.substring(index + 1);
        kp = input.substring(0, input.indexOf(" ")).toFloat();
        input = input.substring(input.indexOf(" ") + 1);
        ki = input.substring(0, input.indexOf(" ")).toFloat();
        input = input.substring(input.indexOf(" ") + 1);
        kd = input.toFloat();
        input = input.substring(input.indexOf(" ") + 1);
        ff = input.toFloat();

        heaterPID.kp = kp;
        heaterPID.ki = ki;
        heaterPID.kd = kd;
        heaterPID.feedForwardGain = ff;
        heaterPID.reset();
        Serial.print("Heater PID updated: ");

        Serial.print(kp);
        Serial.print(" ");
        Serial.print(ki);
        Serial.print(" ");
        Serial.print(kd);
        Serial.print(" ");
        Serial.println(ff);
    }

    // HEATER <VALUE>
    if (input.startsWith("HEATER")) {
        float output = input.substring(input.indexOf(" ") + 1).toFloat();
        overrideHeaterOutput = output;
        heaterPID.reset();
        Serial.print("Heater output updated: ");
        Serial.println(overrideHeaterOutput);
    }

    // SETPOINT <VALUE>
    if (input.startsWith("SETPOINT")) {
        float setPoint = input.substring(input.indexOf(" ") + 1).toFloat();
        heaterPID.setPoint = setPoint;
        Serial.print("SetPoint updated: ");
        Serial.println(heaterPID.setPoint);
    }
}

inline void printVerboseStats() {
    if (FAN_ENABLED) {
        Serial.print("Fan speed: ");
        Serial.print(fanSpeed);
        Serial.print("% - ");
        Serial.print(currentFanSpeed);
        Serial.print("PWM ");
    }

    if (THERMISTOR_ENABLED) {
        Serial.print("Hotend temperature: ");
        Serial.print(hotEndTemperature.value);
        Serial.print("°C ");
    }

    if (HEATER_ENABLED) {
        Serial.print("Heater setpoint: ");
        Serial.print(heaterPID.setPoint);
        Serial.print("°C ");
        Serial.print("Heater output: ");
        Serial.print((hotEndOutput * 100.0) / 255.0);
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
        Serial.print("g (Max: ");
        Serial.print(boxMaxWaterVaporMass);
        Serial.print("g) ");
    }

    Serial.print("Free RAM: ");
    Serial.print(getFreeRAM());
    Serial.print(" bytes");
}

inline void printStats() {
    if (FAN_ENABLED) {
        Serial.print(fanSpeed);
        Serial.print(",");
        Serial.print(currentFanSpeed / FAN_MAX_SPEED * 100.0);
        Serial.print(",");
    }

    if (THERMISTOR_ENABLED) {
        Serial.print(hotEndTemperature.value);
        Serial.print(",");
    }

    if (HEATER_ENABLED) {
        Serial.print(heaterPID.setPoint);
        Serial.print(",");
        Serial.print((hotEndOutput * 100.0) / 255.0);
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
