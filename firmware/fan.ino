void updateFan() {
    if constexpr (!FAN_ENABLED) return;

    // Manual fan speed override
    if (overrideFanSpeed >= 0.0f) {
        fanSpeed = overrideFanSpeed;
    } else if (millis() - lastFanUpdate >= FAN_UPDATE_INTERVAL) {
        lastFanUpdate = millis();

        if (hotEndTemperature.value >= MAX_HEATER_TEMP) {
            // Safety override
            fanSpeed = FAN_MAX_SPEED;
        } else if (hotEndTemperature.value < heaterPID.setPoint / 2.0f) {
            // Enable faster heat up
            fanSpeed = 0.0f;
        } else if (boxTemperature.value < heaterPID.setPoint * 0.96f) {
            // Slow down the fan as we approach the set point
            fanSpeed = (1.0f - powf(boxTemperature.value / heaterPID.setPoint, 2.0f)) * (FAN_MAX_SPEED - FAN_MIN_SPEED) + FAN_MIN_SPEED;
        } else {
            // SetPoint reached, keep the fan running
            // fanSpeed = FAN_MIN_SPEED;
            fanPID.update(hotEndTemperature.value);
            fanSpeed = fanPID.output;
        }
    }

    // Smooth fan speed changes
    if (millis() - lastFanSpeedUpdate >= FAN_SPEED_UPDATE_INTERVAL) {
        lastFanSpeedUpdate = millis();
        currentFanSpeed += (fanSpeed - currentFanSpeed) * 0.0025f;
        pwmWrite(FAN_PIN, currentFanSpeed);
    }
}