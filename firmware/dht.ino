void updateDHT() {
    if constexpr (!DHT_ENABLED) return;
    if (millis() - lastDHTUpdate < DHT_UPDATE_INTERVAL) return;
    lastDHTUpdate = millis();

    float temperature = dht.readTemperature() - DHT_TEMPERATURE_OFFSET;
    float humidity = dht.readHumidity() - DHT_HUMIDITY_OFFSET;
    if (isnan(temperature) || temperature <= DHT_MIN_TEMP || temperature > DHT_MAX_TEMP || isnan(humidity) || humidity <= DHT_MIN_HUMIDITY || humidity > DHT_MAX_HUMIDITY) {
        log("Failed to read DHT values!");
        return;
    }

    // These cheap sensors are not 100% reliable lol
    // Check if there is a huge difference between the current and previous values
    if ((boxTemperature.value > 0 && abs((temperature - boxTemperature.value) / boxTemperature.value) > DHT_CHANGE_THRESHOLD) || (boxHumidity.value > 0 && abs((humidity - boxHumidity.value) / boxHumidity.value) > DHT_CHANGE_THRESHOLD)) {
        log("DHT values are too far off, ignoring...");
        log("Current: " + String(temperature) + "°C, " + String(humidity) + "%");
        log("Previous: " + String(boxTemperature.value) + "°C, " + String(boxHumidity.value) + "%");
        log("Threshold: " + String(DHT_CHANGE_THRESHOLD));
        log("Difference: " + String(abs((temperature - boxTemperature.value) / boxTemperature.value)) + ", " + String(abs((humidity - boxHumidity.value) / boxHumidity.value)));
        return;
    }

    boxTemperature.set(temperature);
    boxHumidity.set(humidity);
    calculateAbsoluteHumidity(temperature, humidity, boxAbsoluteHumidity, boxWaterVaporMass, boxMaxWaterVaporMass);
}

inline void calculateAbsoluteHumidity(const float airTemperature, const float relativeHumidity, float &absoluteHumidity, float &waterVaporMass, float &maxWaterVaporMass) {
    // Teten's formula
    const float saturationVaporPressure = 0.611 * exp((17.502 * airTemperature) / (240.97 + airTemperature)); // KPa
    const float vaporPressure = (relativeHumidity / 100.0f) * saturationVaporPressure; // KPa

    // Ideal gas law
    const float gasConstant = 8.3144621f; // J/(mol·K)
    const float waterMolarMass = 18.01528f; // g/mol
    const float kelvinTemperature = airTemperature + 273.15f; // K
    const float vaporDensity = ((vaporPressure * waterMolarMass) / (gasConstant * kelvinTemperature)) * 1000.0f; // g/m³

    // Calculate the maximum amount of water vapor that can be in the air
    const float saturationVaporDensity = ((saturationVaporPressure * waterMolarMass) / (gasConstant * kelvinTemperature)) * 1000.0f; // g/m³

    absoluteHumidity = vaporDensity;
    waterVaporMass = BOX_VOLUME_M3 * vaporDensity;
    maxWaterVaporMass = BOX_VOLUME_M3 * saturationVaporDensity;
}