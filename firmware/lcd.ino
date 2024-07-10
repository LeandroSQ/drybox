inline void setupLCD() {
    // I ran out of 5v pins, and did not want to mess with my already made PCB
    pinMode(LCD_POWER_PIN, OUTPUT);
    digitalWrite(LCD_POWER_PIN, HIGH);

    delay(150);// Wait for the LCD to power up

    // I2C is known to hang sometimes, so set a timeout
    Wire.setWireTimeout(3000, true);

    lcd.setup();

    // Display startup message
    lcd.printCentered("DryBox 1.0", 0);
    lcd.printCentered("by LeandroSQ", 1);
}

void updateLCD() {
    if constexpr (!LCD_ENABLED) return;

    if (Wire.getWireTimeoutFlag()) {
        // Not using verbose log here, since this is a critical error
        Serial.println("I2C timeout detected, resetting...");
        i2cTimeoutCount++;
        Wire.clearWireTimeoutFlag();
        return;
    }

    if (millis() - lastLCDUpdate < LCD_UPDATE_INTERVAL) return;

    // Box temperature
    lcd.setCursor(0, 0);
    lcd.printValue((unsigned char)boxTemperature.value, nullptr, Icon::THERMOMETER, "C", Icon::CELSIUS, DigitCount::DOUBLE_DIGIT);
  	lcd.print(" ");

    if ((millis() / (LCD_UPDATE_INTERVAL * 2)) % 3 == 0) {
        // Hot end temperature
        lcd.printValue((unsigned char)hotEndTemperature.value, nullptr, Icon::HEAT, "C", Icon::CELSIUS, DigitCount::DOUBLE_DIGIT);
    } else {
        // Hot end output %
        float hotEndOutputPercentage = (hotEndOutput * 100.0f) / 255.0f;
        if (hotEndOutputPercentage > 0 && hotEndOutputPercentage < 1) {
            char tmp[4] = {0};
            dtostrf(hotEndOutputPercentage, 1, 1, tmp);

            lcd.printIcon(Icon::PLUG);
            lcd.print(tmp);
            lcd.print("%");
            if (strlen(tmp) < 3) lcd.print(" ");
        } else if (hotEndOutputPercentage == 100) {
            lcd.printValue((unsigned char) hotEndOutputPercentage, nullptr, Icon::PLUG, "% ", Icon::NONE, DigitCount::TRIPLE_DIGIT);
        } else {
            lcd.printValue((unsigned char) hotEndOutputPercentage, nullptr, Icon::PLUG, "% ", Icon::NONE, DigitCount::DOUBLE_DIGIT);
        }
    }

    // Fan speed - PWM value
    const Icon fanIcon = (millis() / LCD_UPDATE_INTERVAL) % 2 == 0 ? Icon::FAN0 : Icon::FAN1;
    lcd.printValue((unsigned char)currentFanSpeed, nullptr, fanIcon, "% ", Icon::NONE, DigitCount::TRIPLE_DIGIT);

    // Box humidity
    lcd.setCursor(0, 1);
  	lcd.printIcon(Icon::DROPLET);
  	lcd.print((unsigned char)boxHumidity.value);
	lcd.print("% ");

    // Box absolute humidity
    lcd.printValue((unsigned char)boxAbsoluteHumidity, nullptr, Icon::NONE, "g/m", Icon::CUBIC, DigitCount::DOUBLE_DIGIT);
    lcd.print(" ");

    // Debug info
    /* lcd.print(getFreeRAM());
    lcd.print(" ");
    lcd.print(i2cTimeoutCount); */

    // Box water vapor mass, ensuring .1 precision
    char tmp[4] = {0};
	dtostrf(boxWaterVaporMass, 1, 1, tmp);
    lcd.print(tmp);
    lcd.print("g");

    lastLCDUpdate = millis();
}