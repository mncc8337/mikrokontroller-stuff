#include <Arduino.h>

#define LDR1 32
#define LDR2 33
#define LED1 25
#define LED2 26

void setup() {
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);

    Serial.begin(115200);
    delay(100);
    Serial.println("hi");
}

void loop() {
    Serial.print(analogRead(LDR1));
    Serial.print(" ");
    Serial.print(analogRead(LDR2));
    Serial.print("\n");

    if(analogRead(LDR1) < 500) {
        digitalWrite(LED1, HIGH);
    } else {
        digitalWrite(LED1, LOW);
    }

    if(analogRead(LDR2) < 500) {
        digitalWrite(LED2, HIGH);
    } else {
        digitalWrite(LED2, LOW);
    }
}
