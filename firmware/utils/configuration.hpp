#pragma once

/* Modules */
constexpr bool DHT_ENABLED = true;
constexpr bool THERMISTOR_ENABLED = true;
constexpr bool HEATER_ENABLED = true;
constexpr bool FAN_ENABLED = true;
constexpr bool LCD_ENABLED = true;
constexpr bool IO_ENABLED = true;
constexpr bool IO_VERBOSE = false;

/* Pins */
#define DHT_PIN 2 /* Digital */
#define THERMISTOR_PIN 0 /* Analog */
#define FAN_PIN 10 /* Digital PWM */
#define HEATER_PIN 11 /* Digital PWM */
#define LCD_POWER_PIN 3 /* Digital */
#define LCD_SDA_PIN 4 /* Analog */
#define LCD_SCL_PIN 5 /* Analog */

/* Constants */
#define DHT_TYPE DHT22

// I used my box dimensions to calculate the volume
// But it also could be calculated using the amount in liters
// Like: <liters> / 1000
// Converting from L to m³
constexpr float BOX_VOLUME_M3 = 0.062203869f;// m³

/* Accuracy */
constexpr unsigned char AVG_SAMPLES = 2;
constexpr float THERMISTOR_OFFSET = 5.0f;
constexpr float DHT_TEMPERATURE_OFFSET = 0.0f;
constexpr float DHT_HUMIDITY_OFFSET = 0.0f;
constexpr float DHT_CHANGE_THRESHOLD = 0.45f;
constexpr float THERMISTOR_CHANGE_THRESHOLD = 0.65f;
constexpr float DHT_MIN_TEMP = 0.0f;
constexpr float DHT_MAX_TEMP = 50.0f;
constexpr float DHT_MIN_HUMIDITY = 0.0f;
constexpr float DHT_MAX_HUMIDITY = 100.0f;

/* Intervals */
constexpr unsigned long DHT_UPDATE_INTERVAL = 1750UL;
constexpr unsigned long THERMISTOR_UPDATE_INTERVAL = 1500UL;
constexpr unsigned long HEATER_UPDATE_INTERVAL = DHT_UPDATE_INTERVAL + 5UL;
constexpr unsigned long FAN_UPDATE_INTERVAL = 1505UL;
constexpr unsigned long FAN_SPEED_UPDATE_INTERVAL = 33UL;
constexpr unsigned long LCD_UPDATE_INTERVAL = 1505UL;
constexpr unsigned long IO_UPDATE_INTERVAL = 3000UL;

/* Ranges */
constexpr float FAN_MAX_SPEED = 50.0f;
constexpr float FAN_MIN_SPEED = 25.0f;

/* Temperature */
constexpr float MAX_HEATER_TEMP = 70.0f;
constexpr float HEATER_SETPOINT = 45.0f;
constexpr bool ENABLE_HEATER_SAFETY_RANGE = false;

/* LCD */
constexpr unsigned char LCD_ADDRESS = 0x27;
constexpr unsigned char LCD_COLUMNS = 16;
constexpr unsigned char LCD_ROWS = 2;