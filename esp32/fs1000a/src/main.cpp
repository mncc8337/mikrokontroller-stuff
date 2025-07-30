#include <Arduino.h>
#include <RH_ASK.h>
#include <SPI.h>

#define LED 2
#define TX 32
RH_ASK driver(2000, -1, TX);

unsigned packet_count = 0;

void setup() {
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);

    Serial.begin(115200);
    if(!driver.init()) {
        Serial.println("tx init failed");
    }
}

void loop() {
    digitalWrite(LED, LOW);

    if(driver.send((uint8_t*)&packet_count, sizeof(packet_count))) {
        driver.waitPacketSent();
        Serial.print("Sent packet #: ");
        Serial.println(packet_count);
        packet_count++;
    } else {
        Serial.println("Send failed");
    }

    digitalWrite(LED, HIGH);
    delay(1000);
}
