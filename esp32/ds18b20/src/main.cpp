#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE 23

OneWire onewire(ONE_WIRE);
DallasTemperature sensors(&onewire);

void setup() {
    Serial.begin(115200);
    sensors.begin();
}

void loop() {
    sensors.requestTemperatures();
    Serial.print("Temperature: ");
    Serial.println(sensors.getTempCByIndex(0));
    delay(1000);
}
