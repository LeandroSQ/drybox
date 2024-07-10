void updateHeater() {
    if constexpr (!HEATER_ENABLED) return;
    if (millis() - lastHeaterUpdate < HEATER_UPDATE_INTERVAL) return;
    lastHeaterUpdate = millis();

    // Manual heater output override
    if (overrideHeaterOutput >= 0.0f) {
        hotEndOutput = heaterPID.output = overrideHeaterOutput;
        analogWrite(HEATER_PIN, overrideHeaterOutput);
        return;
    }

    // Safety override
    if (hotEndTemperature.value >= MAX_HEATER_TEMP || boxTemperature.value >= MAX_HEATER_TEMP) {
        hotEndOutput = heaterPID.output = 0.0f;
        heaterPID.reset();
        analogWrite(HEATER_PIN, 0);
        return;
    }

    // Detects whenever the lid is open, and reset the PID so we don't get a huge overshoot
    if (boxTemperature.value < lastHighTemp && lastHighTemp - boxTemperature.value >= 1.5f) {
        isWarmingUp = true;
    } else if (boxTemperature.value >= heaterPID.setPoint && isWarmingUp) {
        isWarmingUp = false;
        lastHighTemp = boxTemperature.value;
        log("Target temperature reached!");
        heaterPID.reset();
    }

    heaterPID.update(boxTemperature.value, hotEndTemperature.value);

    hotEndOutput = heaterPID.output;

    if (ENABLE_HEATER_SAFETY_RANGE && hotEndTemperature.value > HOT_END_MAX_TEMP) {
        // Adjust the output to reach the set point by also being aware of the maximum safe temperature
        float adjustmentFactor = 1.0f - powf((hotEndTemperature.value - HOT_END_MAX_TEMP) / ((MAX_HEATER_TEMP - HOT_END_MAX_TEMP) / 1.5), 2.0f);
        if (adjustmentFactor < 0.0f) adjustmentFactor = 0.0f;
        else if (adjustmentFactor > 1.0f) adjustmentFactor = 1.0f;
        hotEndOutput *= adjustmentFactor;

        // Just to be sure
        if (hotEndOutput < 0.0f) hotEndOutput = 0.0f;
        else if (hotEndOutput > 255.0f) hotEndOutput = 255.0f;
    }

    analogWrite(HEATER_PIN, (unsigned char) hotEndOutput);
}