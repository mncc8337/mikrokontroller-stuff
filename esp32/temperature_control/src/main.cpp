#include <Arduino.h>
#include <Wire.h>
#include <LCDI2C_Multilingual_MCD.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define IIC_SCL 22
#define IIC_SDA 21

#define LAMP 18
#define ONE_WIRE 19

#define INTERVAL 800
#define MIN_TEMP 30.5

LCDI2C_Generic lcd(0x27, 16, 2);

OneWire onewire(ONE_WIRE);
DallasTemperature ds18b20(&onewire);

int lamp_state = HIGH;

float timeline = 0;
float time_step = 0.05;
float temperature_func(float t) {
    if(t <= 0.5f) {
        // rising phase: 30 -> 45
        float start = 30.0f;
        float end = 45.0f;
        float k = log(end - start + 1.0f);
        return start + (pow(EULER, k * (t / 0.5f)) - 1.0f) / (pow(EULER, k) - 1.0f) * (end - start);
    } else {
        // falling phase: 45 -> 25
        float start = 45.0f;
        float end = 25.0f;
        float k = log(start - end + 1.0f);
        return end + (pow(EULER, k * ((1.0f - t) / 0.5f)) - 1.0f) / (pow(EULER, k) - 1.0f) * (start - end);
    }
}

float dt = 0;
float get_temperature() {
    dt = millis();
    ds18b20.requestTemperatures();
    dt = millis() - dt;
    return ds18b20.getTempCByIndex(0);
}

void temperature_control(float target, bool hold) {
    float temp = -1;
    // keep adjust until reaching acceptable range
    while(temp < target - 0.05 || temp > target + 0.05 || hold) {
        temp = get_temperature();
        lcd.setCursor(0, 1);
        lcd.print("curr ");
        lcd.print(temp);
        if(temp > target) {
            lamp_state = LOW;
        } else {
            lamp_state = HIGH;
        }
        digitalWrite(LAMP, lamp_state);

        delay(INTERVAL - dt);
    }
}

void setup() {
    lcd.init();
    lcd.backlight();

    ds18b20.begin();

    pinMode(LAMP, OUTPUT);
}

void loop() {
    float target = temperature_func(timeline);
    // we have no cooling element
    // TODO:
    // change MIN_TEMP to ambient temp
    if(target < MIN_TEMP) {
        target = MIN_TEMP;
    }

    lcd.setCursor(0, 0);
    lcd.print("targ ");
    lcd.print(target);

    temperature_control(target, false);
    timeline += time_step;

    if(timeline >= 1.0) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("end point reached");
        lcd.print(target);
        temperature_control(target, true);
    }
}
