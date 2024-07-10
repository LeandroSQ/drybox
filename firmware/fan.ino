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
            // Enable hot end faster heat up
            fanSpeed = 0.0f;
        } else if (boxTemperature.value > heaterPID.setPoint + 0.3f) {
            // Wait for the hot end to cool down
            // This seems counter intuitive, but what actually spreads the heat is the fan
            // And the problem this can create is, when the box readings are too high
            // And the fan spins too fast, it won't cool the hotend, since there is a PID loop there as well
            // So it starts a hell loop, actually increasing the box temperature as a whole
            fanSpeed = 0.0f;
        } else if (boxTemperature.value < heaterPID.setPoint * 0.98f) {
            // Slow down the fan as we approach the set point
            // Reducing overshoot
            fanSpeed = (1.0f - powf(boxTemperature.value / heaterPID.setPoint, 2.0f)) * (FAN_MAX_SPEED - FAN_MIN_SPEED) + FAN_MIN_SPEED;
        } else {
            // SetPoint reached, keep the fan running
            fanSpeed = FAN_MIN_SPEED;
        }
    }

    // Smooth fan speed changes
    if (millis() - lastFanSpeedUpdate >= FAN_SPEED_UPDATE_INTERVAL) {
        lastFanSpeedUpdate = millis();
        currentFanSpeed += (fanSpeed - currentFanSpeed) * 0.0025f;
        pwmWrite(FAN_PIN, currentFanSpeed);
    }
}