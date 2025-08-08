#include <Arduino.h>
#include <Adafruit_BMP280.h>
#include <SPI.h>

#include <math.h>

#define LAMP 17

#define HSPI_SCK 14
#define HSPI_MOSI 13
#define HSPI_MISO 12

#define BMP_CS 15

SPIClass *hspi = NULL;
Adafruit_BMP280 bmp(BMP_CS, HSPI_MOSI, HSPI_MISO, HSPI_SCK);

int lamp_state = HIGH;

float end_time = 16.0;
float timeline = 0;
float temperature_func(float t) {
    return 45 + 2 * pow(2.71828, 0.23 * t) - 0.3 * t * t;
}

void setup() {
    pinMode(LAMP, OUTPUT);

    Serial.begin(115200);
    delay(100);
    Serial.println("serial init");

    if(!bmp.begin()) {
        Serial.println("cannot find a bmp280, check wiring");
        while(true);
    }

    float f0 = temperature_func(0);
    Serial.println("bmp280 init");
    Serial.print("trying to get to f(0) = ");
    Serial.println(f0);

    float temp = bmp.readTemperature();
    while(temp < f0 - 0.05 || temp > f0 + 0.05) {
        Serial.print("curr ");
        Serial.println(temp);
        if(temp > f0) {
            lamp_state = LOW;
        } else {
            lamp_state = HIGH;
        }
        digitalWrite(LAMP, lamp_state);

        delay(1000);
        temp = bmp.readTemperature();
    }

    Serial.println("f0 reached, start aligning to f(t) ...");
}

void loop() {
    float temp = bmp.readTemperature();
    float target_temp = temperature_func(timeline);

    Serial.print("curr ");
    Serial.print(temp);
    Serial.print(" targ ");
    Serial.println(target_temp);

    if(temp > target_temp) {
        lamp_state = LOW;
    } else {
        lamp_state = HIGH;
    }

    digitalWrite(LAMP, lamp_state);
    delay(1000);
    if(timeline < end_time) {
        timeline += 1.0 / 60.0;
        if(timeline >= end_time) {
            Serial.println("end time reached, holding temp");
        }
    }
}
