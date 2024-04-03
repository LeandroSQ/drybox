void updateThermistor() {
    if constexpr (!THERMISTOR_ENABLED) return;
    if (millis() - lastThermistorUpdate < THERMISTOR_UPDATE_INTERVAL) return;
    lastThermistorUpdate = millis();

    float temperature = thermistor.getTemp() - THERMISTOR_OFFSET;
    if (isnan(temperature) || temperature <= 0.0f) {
        log("Failed to read thermistor temperature!");
    } else if (abs(temperature) / abs(hotEndTemperature.value) > 0.7f) {
        // Didn't really had issues with my thermistor, but just to be sure
        log("Thermistor temperature is too far off, ignoring...");
    } else {
        hotEndTemperature.set(temperature);
    }
}