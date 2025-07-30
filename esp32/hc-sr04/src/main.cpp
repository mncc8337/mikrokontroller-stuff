#include <Arduino.h>
#include <NewPing.h>

#define TRIG 23
#define ECHO 22

NewPing sonar(TRIG, ECHO, 400);

void setup() {
    Serial.begin(115200);
}

void loop() {
    int dist = sonar.ping_cm();
    Serial.println(dist);
    delay(100);
}
