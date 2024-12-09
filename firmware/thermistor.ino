void updateThermistor() {
    if constexpr (!THERMISTOR_ENABLED) return;
    if (millis() - lastThermistorUpdate < THERMISTOR_UPDATE_INTERVAL) return;
    lastThermistorUpdate = millis();

    float temperature = thermistor.getTemp() - THERMISTOR_OFFSET;
    if (isnan(temperature) || temperature <= THERMISTOR_MIN_TEMP || temperature > THERMISTOR_MAX_TEMP) {
        log("Failed to read thermistor temperature!");
        invalidThermistorReads++;
    } else if (hotEndTemperature.value > 0 && abs((temperature - hotEndTemperature.value) / hotEndTemperature.value) > THERMISTOR_CHANGE_THRESHOLD) {
        // Didn't really had issues with my thermistor, but just to be sure
        log("Thermistor temperature is too far off, ignoring...");
        invalidThermistorReads++;
    } else {
        hotEndTemperature.set(temperature);
        invalidThermistorReads = 0;
    }
}