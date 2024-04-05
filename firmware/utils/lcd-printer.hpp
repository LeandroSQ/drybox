#include <LiquidCrystal_I2C.h>
#include "configuration.hpp"

enum Icon : unsigned char {
  	NONE = 255,
    CELSIUS = 0,
    DROPLET = 1,
    CUBIC = 2,
    THERMOMETER = 3,
    PLUG = 4,
    FAN0 = 5,
    FAN1 = 6
};

enum DigitCount : unsigned char {
    DOUBLE_DIGIT = 0,
    TRIPLE_DIGIT
};

struct LCDPrinter {

    LiquidCrystal_I2C lcd;

    LCDPrinter(): lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS) {  }

    setup() {
        lcd.init();
        lcd.backlight();
        lcd.clear();

        setupIcons();
    }

    inline clear() {
        lcd.clear();
    }

    template <typename T>
    inline print(T value) {
        lcd.print(value);
    }

    inline printIcon(const unsigned char icon) {
        lcd.write(icon);
    }

    /**
     * Prints a value on the LCD screen.
     *
     * @param value The value to be printed.
     * @param prefix An optional prefix string to be displayed before the value.
     * @param prefixIcon An optional icon to be displayed before the prefix.
     * @param suffix An optional suffix string to be displayed after the value.
     * @param suffixIcon An optional icon to be displayed after the suffix.
     * @param digits The number of digits to display for the value, used for padding.
     */
    printValue(const unsigned char value, const char* prefix = nullptr, const Icon prefixIcon = Icon::NONE, const char* suffix = nullptr, const Icon suffixIcon = Icon::NONE, DigitCount digits = DigitCount::DOUBLE_DIGIT) {
        // Padding
        if (value < 10) lcd.print(" ");
        if (digits == DigitCount::TRIPLE_DIGIT && value < 100) lcd.print(" ");

      	if (prefix != nullptr) lcd.print(prefix);
      	if (prefixIcon != Icon::NONE) lcd.write(prefixIcon);

        lcd.print(value);

        if (suffix != nullptr) lcd.print(suffix);
        if (suffixIcon != Icon::NONE) lcd.write(suffixIcon);
    }

    inline printCentered(const char* text, const unsigned char row = 0) {
        const size_t length = strlen(text);
        lcd.setCursor((LCD_COLUMNS - length) / 2, row);
        lcd.print(text);
    }

    inline setCursor(const unsigned char column, const unsigned char row) {
        lcd.setCursor(column, row);
    }

    inline void setupIcons() {
        unsigned char droplet[] = {
            B00000,
            B00100,
            B01010,
            B10001,
            B10101,
            B10001,
            B01110,
            B00000
        };

        unsigned char celsius[] = {
            B01100,
            B10010,
            B10010,
            B01100,
            B00000,
            B00000,
            B00000,
            B00000
        };

        unsigned char cubic[] = {
            B11000,
            B00100,
            B11000,
            B00100,
            B11000,
            B00000,
            B00000,
            B00000
        };

        unsigned char thermometer[] = {
            B00100,
            B01010,
            B01010,
            B01010,
            B11011,
            B10001,
            B11011,
            B01110
        };

        unsigned char plug[] = {
            B00000,
            B00000,
            B01010,
            B01010,
            B11111,
            B10001,
            B10001,
            B01110
        };

        unsigned char fan0[] = {
            B00000,
            B10011,
            B11010,
            B00100,
            B01011,
            B11001,
            B00000,
            B00000
        };

        unsigned char fan1[] = {
            B00000,
            B01100,
            B00101,
            B11111,
            B10100,
            B00110,
            B00000,
            B00000
        };

        lcd.createChar(Icon::CELSIUS, celsius);
        lcd.createChar(Icon::DROPLET, droplet);
        lcd.createChar(Icon::CUBIC, cubic);
        lcd.createChar(Icon::THERMOMETER, thermometer);
        lcd.createChar(Icon::PLUG, plug);
        lcd.createChar(Icon::FAN0, fan0);
        lcd.createChar(Icon::FAN1, fan1);
    }

};