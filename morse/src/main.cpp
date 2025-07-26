#include <Arduino.h>

#define LED_PIN 1

char buffer[512];
int buffer_len = 0;

int base_beep_duration = 300;

char* morse_table[26] = {
    "et", "teee", "tete", "tee", "e", "eete", "tte", "eeee", "ee", "ettt",
    "tet", "eette", "tt", "te", "ttt", "eett", "tett", "ete", "eee", "t",
    "eet", "eeet", "ett", "teet", "tett", "ttee"
};

void led_blink(int dur) {
    digitalWrite(LED_PIN, LOW);
    delay(dur);
    digitalWrite(LED_PIN, HIGH);
}

void enable_serial() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("input mode. type any character.");
}

void print_morse() {
    Serial.end();

    for(int i = 0; i < buffer_len; i++) {
        if(buffer[i] == ' ') {
            delay(7 * base_beep_duration);
            continue;
        }
        char* ins = morse_table[buffer[i] - 97];
        char last_char = '\0';
        for(int j = 0; j < 4; j++) {
            if(ins[j] == '\0')
                break;

            if(last_char == ins[j])
                delay(base_beep_duration);
            else
                delay(3 * base_beep_duration);

            if(ins[j] == 'e')
                led_blink(base_beep_duration);
            else
                led_blink(3 * base_beep_duration);
            last_char = ins[j];
        }
    }

    buffer_len = 0;
}

void setup() {
    pinMode(LED_PIN, OUTPUT);
    delay(100);
}

void loop() {
    digitalWrite(LED_PIN, HIGH);
    enable_serial();

    while(buffer_len < 512) {
        delay(100);
        if(Serial.available()) {
            char c = Serial.read();
            if(c == '\n') {
                print_morse();
                break;
            }
            if((c >= 'a' && c <= 'z') || c == ' ')
                buffer[buffer_len++] = c;
        }
    }
    if(buffer_len == 512)
        print_morse();
}
