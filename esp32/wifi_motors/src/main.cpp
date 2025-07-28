#include <Arduino.h>
#include <WiFi.h>

#define RMOTOR 32
#define LMOTOR 33

const char SSID[] = "my-uwu-esp32";
const int PORT = 23;
WiFiServer server(PORT);

void motor_drive(int l, int r) {
    digitalWrite(LMOTOR, l);
    digitalWrite(RMOTOR, r);
}

void setup() {
    Serial.begin(115200);
    delay(100);
    Serial.printf("hi\n");

    pinMode(RMOTOR, OUTPUT);
    pinMode(LMOTOR, OUTPUT);
    motor_drive(LOW, LOW);

    WiFi.softAP(SSID);
    server.begin();
    server.setNoDelay(true);
}

void loop() {
    WiFiClient client = server.available();
    char instruction = 0;

    if(client) {
        while(client.connected()) {
            Serial.printf("client connected\n");

            while(!client.available()) {
                delay(10);
            }

            char c = client.read();
            if(c != 13) {
                instruction = c;
                Serial.printf("received %d\n", instruction);
            }

            if(instruction == 'f') {
                motor_drive(HIGH, HIGH);
            } else if(instruction == 'l') {
                motor_drive(LOW, HIGH);
            } else if(instruction == 'r') {
                motor_drive(HIGH, LOW);
            } else {
                motor_drive(LOW, LOW);
            }
        }
        Serial.printf("client disconnected\n");
    }
    client.stop();
    Serial.printf("no client, waiting\n");
}
