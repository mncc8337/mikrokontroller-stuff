#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define LED 48

Adafruit_NeoPixel rgb_led(1, LED, NEO_GRB + NEO_KHZ800);

int sample_func(float t) {
    return 127.5 * sin(t) + 127.5;
}

void setup() {
    rgb_led.begin();
    rgb_led.setBrightness(128);
}

void loop() {
    float t = millis() / 1000.0;
    rgb_led.setPixelColor(
        0,
        sample_func(t),
        sample_func(t + 3 * PI / 2.0),
        sample_func(t)
    );
    rgb_led.show();
    delay(10);
}
