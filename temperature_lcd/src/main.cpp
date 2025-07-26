#include <Wire.h>
#include <LCDI2C_Multilingual_MCD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

LCDI2C_Generic lcd(0x27, 16, 2);
Adafruit_BMP280 bmp;

void setup() {
    Wire.begin(21, 22);
    lcd.init();
    lcd.backlight();

    lcd.setCursor(0, 0);
    if(!bmp.begin(0x76)) {
        lcd.print("no BMP280 found");
        while(true);
    }
    lcd.print("start in 3s");
    delay(3000);
}

void loop() {
    float temp = bmp.readTemperature();
    float pres = bmp.readPressure();

    lcd.setCursor(0, 0);
    lcd.println("temp:" + String(temp) + "*C");
    lcd.println("pres:" + String(pres/1000 + 0.5) + "kPa");
}
