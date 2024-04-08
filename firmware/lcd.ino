inline void setupLCD() {
    // I ran out of 5v pins, and did not want to mess with my already made PCB
    pinMode(LCD_POWER_PIN, OUTPUT);
    digitalWrite(LCD_POWER_PIN, HIGH);
    delay(250);// Wait for the LCD to power up

    lcd.setup();

    // Display startup message
    lcd.printCentered("DryBox v1.0", 0);
    lcd.printCentered("by LeandroSQ01", 1);
}

void updateLCD() {
    if constexpr (!LCD_ENABLED) return;
    if (millis() - lastLCDUpdate < LCD_UPDATE_INTERVAL) return;

    // Box temperature
    lcd.setCursor(0, 0);
    lcd.printValue((unsigned char)boxTemperature.value, nullptr, Icon::THERMOMETER, "C", Icon::CELSIUS, DigitCount::DOUBLE_DIGIT);
  	lcd.print(" ");

    // Hot end output %
    lcd.printValue((unsigned char)((hotEndOutput * 100.0f) / 255.0f), nullptr, Icon::PLUG, "% ", Icon::NONE, DigitCount::TRIPLE_DIGIT);

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

    // Box water vapor mass, ensuring .1 precision
    char tmp[4] = {0};
	dtostrf(boxWaterVaporMass, 1, 1, tmp);
    lcd.print(tmp);
    lcd.print("g");

    lastLCDUpdate = millis();
}