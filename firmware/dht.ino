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
    if (abs(temperature / boxTemperature.value) > DHT_CHANGE_THRESHOLD || abs(humidity / boxHumidity.value) > DHT_CHANGE_THRESHOLD) {
        log("DHT values are too far off, ignoring...");
        return;
    }

    boxTemperature.set(temperature);
    boxHumidity.set(humidity);
    calculateAbsoluteHumidity(temperature, humidity, boxAbsoluteHumidity, boxWaterVaporMass);
}

inline void calculateAbsoluteHumidity(const float airTemperature, const float relativeHumidity, float &absoluteHumidity, float &waterVaporMass) {
    // Teten's formula
    const float saturationVaporPressure = 0.611 * exp((17.502 * airTemperature) / (240.97 + airTemperature)); // KPa
    const float vaporPressure = (relativeHumidity / 100.0f) * saturationVaporPressure; // KPa

    // Clapeyron's equation
    const float gasConstant = 8.3144621f; // J/(mol·K)
    const float waterMolarMass = 18.01528f; // g/mol
    const float kelvinTemperature = airTemperature + 273.15f; // K
    const float vaporDensity = ((vaporPressure * waterMolarMass) / (gasConstant * kelvinTemperature)) * 1000.0f; // g/m³

    absoluteHumidity = vaporDensity;
    waterVaporMass = BOX_VOLUME_M3 * vaporDensity;
}