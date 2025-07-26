#include <Arduino.h>
#include <Wire.h>
#include <LCDI2C_Multilingual_MCD.h>

#define VIN 32
#define SDA 26
#define SCL 27

const float MIN_VOLTAGE_DIVIDER_RATIO = 0.2475;
const float MAX_VOLTAGE_DIVIDER_RATIO = 0.255050505051;

LCDI2C_Generic lcd(0x27, 16, 2);

byte plus_minus[8] = {
    B00000,
    B00100,
    B01110,
    B00100,
    B00000,
    B01110,
    B00000,
};
#define PLUS_MINUS byte(0)

float read_voltage() {
    return analogReadMilliVolts(VIN) / 1000.0;
}

void setup() {
    analogReadResolution(16);
    analogSetPinAttenuation(32, ADC_11db);

    Wire.begin(SDA, SCL);
    lcd.init();
    lcd.createChar(PLUS_MINUS, plus_minus);
    lcd.backlight();
}

void loop() {
    float val = read_voltage();
    float max_voltage = val / MIN_VOLTAGE_DIVIDER_RATIO;
    float min_voltage = val / MAX_VOLTAGE_DIVIDER_RATIO;
    float mean = (max_voltage + min_voltage) / 2.0;
    float diff = (max_voltage - min_voltage) / 2.0;
    lcd.setCursor(0, 0);
    if(mean > 0.57) {
        lcd.clear();
        lcd.print(mean, 3);
        lcd.write(PLUS_MINUS);
        lcd.print(diff, 3);
        lcd.print("V");
        delay(300);
    } else {
        lcd.print("reading is inaccurate");
        delay(1000);
    }
}
