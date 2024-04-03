void updateHeater() {
    if constexpr (!HEATER_ENABLED) return;
    if (millis() - lastHotEndUpdate < HEATER_UPDATE_INTERVAL) return;
    lastHeaterUpdate = millis();

    // Safety override
    if (hotEndTemperature.value >= MAX_HEATER_TEMP) {
        analogWrite(HEATER_PIN, 0);
    }

    heaterPID.update(hotEndTemperature.value);
    analogWrite(HEATER_PIN, heaterPID.output);
}