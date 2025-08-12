#include <Arduino.h>
#include <Wire.h>
#include <LCDI2C_Multilingual_MCD.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define IIC_SCL 22
#define IIC_SDA 21

#define BUTTON 5
#define HEATER 18
#define ONE_WIRE 19

#define INTERVAL 800 // ms

// settings
#define WATER_MASS 83.3 // g
#define START_TEMP 45.0 // degC
#define END_TEMP 35.0 // degC, should be higher than ambient
#define TEMP_STEP 0.1 // degC
#define GROW_RATE 0.005 // g/100gH2O/s

LCDI2C_Generic lcd(0x27, 16, 2);

OneWire onewire(ONE_WIRE);
DallasTemperature ds18b20(&onewire);

int heater_state = HIGH;

// solubility func for alum
const float sa = 1.43541667e-4f;
const float sb = -2.0875e-3f;
const float sc = 0.102833333f;
const float sd = 3.64f;
float solubility(float T) {
    return ((sa * T + sb) * T + sc) * T + sd;
}

float current_temp_target;

float dt = 0;
float get_temperature() {
    dt = millis();
    ds18b20.requestTemperatures();
    dt = millis() - dt;
    return ds18b20.getTempCByIndex(0);
}

void temperature_target(float targ_temp) {
    float curr_temp = -1;
    while(
        digitalRead(BUTTON) == HIGH
        && (
            curr_temp < targ_temp - 0.5
            || curr_temp > targ_temp + 0.5
            || targ_temp == END_TEMP
        )
    ) {
        curr_temp = get_temperature();
        lcd.setCursor(0, 1);
        lcd.print("curr ");
        lcd.print(curr_temp);
        if(curr_temp > targ_temp) {
            heater_state = LOW;
        } else {
            heater_state = HIGH;
        }
        digitalWrite(HEATER, heater_state);

        delay(INTERVAL - dt);
    }
}

void wait_for_button() {
    while(digitalRead(BUTTON) == HIGH) {
        delay(10);
    }
    while(digitalRead(BUTTON) == LOW) {
        delay(10);
    }
}

void setup() {
    pinMode(BUTTON, INPUT_PULLUP);
    pinMode(HEATER, OUTPUT);

    ds18b20.begin();

    lcd.init();
    lcd.backlight();

    // calculating estimate time
    float curr_temp = START_TEMP;
    float total_secs = 0;
    while(curr_temp > END_TEMP) {
        float next_temp = curr_temp - TEMP_STEP;
        float sol_decrease = solubility(curr_temp) - solubility(next_temp);
        total_secs += sol_decrease / GROW_RATE;
        curr_temp = next_temp;
    }
    int total_mins = total_secs / 60; total_secs -= total_mins * 60;
    int total_hours = total_mins / 60; total_mins -= total_hours * 60;
    lcd.setCursor(0, 0);
    lcd.print("est time");
    lcd.setCursor(0, 1);
    lcd.print(total_hours); lcd.print("h ");
    lcd.print(total_mins); lcd.print("m ");
    lcd.print(int(total_secs + 0.5)); lcd.print("s");
    wait_for_button();
    lcd.clear();

    // calculating result
    float result = solubility(START_TEMP) - solubility(END_TEMP);
    lcd.setCursor(0, 0);
    lcd.print("result crystal");
    lcd.setCursor(0, 1);
    lcd.print(result * WATER_MASS / 100.0); lcd.print("g");
    wait_for_button();
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("try getting to start temp");
    delay(3000);
    lcd.clear();

    // get to start temp
    lcd.setCursor(0, 0);
    lcd.print("getting to ");
    lcd.print(START_TEMP);
    temperature_target(START_TEMP);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("start monitoring");
    delay(3000);
    lcd.clear();

    current_temp_target = START_TEMP;
}

void loop() {
    float next_temp_target = current_temp_target - TEMP_STEP;

    // calculate how much solubility will decrease
    // NOTE:
    // this only works when the solubility func is increasing
    // for decreasing func we need another algorithm
    float sol_decrease = solubility(current_temp_target) - solubility(next_temp_target);

    // create a temperature func based on time
    // T(t) = at + b
    // where T is temperature in C and t is time in second
    float required_time = sol_decrease / GROW_RATE;
    float a = (next_temp_target - current_temp_target) / required_time;
    float b = current_temp_target;

    // try to follow the temperature line
    float timeline = 0;
    float targ_temp, curr_temp;
    while(timeline < required_time) {
        targ_temp = a * timeline + b;
        curr_temp = get_temperature();

        lcd.setCursor(0, 0);
        lcd.print("targ ");
        lcd.print(targ_temp);
        lcd.setCursor(0, 1);
        lcd.print("curr ");
        lcd.print(curr_temp);
        if(curr_temp > targ_temp) {
            heater_state = LOW;
        } else {
            heater_state = HIGH;
        }
        digitalWrite(HEATER, heater_state);

        delay(INTERVAL - dt);
        timeline += INTERVAL / 1000.0;
    }

    current_temp_target = next_temp_target;
    if(current_temp_target <= END_TEMP) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("holding temp");
        temperature_target(END_TEMP);
    }
}
