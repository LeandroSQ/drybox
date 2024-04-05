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

    // Ensure the hotend is always at least at room temperature
    if (hotEndTemperature.value < heaterPID.setPoint) {
        heaterPID.update(hotEndTemperature.value);
    } else {
        heaterPID.update(boxTemperature.value);
    }

    hotEndOutput = heaterPID.output;

    if (enableHeaterSafeTemperature && hotEndTemperature.value > heaterPID.setPoint) {
        // Adjust the output to reach the set point by also being aware of the maximum safe temperature
        float adjustmentFactor = 1.0f - powf((hotEndTemperature.value - heaterPID.setPoint) / ((MAX_HEATER_TEMP - heaterPID.setPoint) / 1.5), 2.0f);
        if (adjustmentFactor < 0.0f) adjustmentFactor = 0.0f;
        else if (adjustmentFactor > 1.0f) adjustmentFactor = 1.0f;
        hotEndOutput *= adjustmentFactor;

        // Just to be sure
        if (hotEndOutput < 0.0f) hotEndOutput = 0.0f;
        else if (hotEndOutput > 255.0f) hotEndOutput = 255.0f;
    }


    analogWrite(HEATER_PIN, hotEndOutput);
}