#include <Arduino.h>

#define BUFFER_LEN(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY[0]))

#define DIGIT1 20
#define DIGIT2 21
#define SER 2
#define OE 3
#define RCLK 6
#define SRCLK 7

#define DISPLAY_INTERVAL 5

#define MINUS 11
#define NOTHING 10
#define LETTER_A 12

const char SEGMENT_LAYOUT[] = {
    0b0111111, // 0
    0b0000110, // 1
    0b1011011, // 2
    0b1001111, // 3
    0b1100110, // 4
    0b1101101, // 5
    0b1111101, // 6
    0b0000111, // 7
    0b1111111, // 8
    0b1101111, // 9
    0b0000000, // nothing
    0b1000000, // -
    0b1110111, // a
    0b1111100, // b
    0b1011000, // c
    0b1011110, // d
    0b1111001, // e
    0b1110001, // f
    0b1111101, // g
    0b1110110, // h
    0b0000110, // i
    0b0001110, // j
    0b1110110, // k
    0b0111000, // l
    0b1010100, // m
    0b1010100, // n
    0b1011100, // o
    0b1110011, // p
    0b1100111, // q
    0b1010000, // r
    0b1101101, // s
    0b1010000, // t
    0b0011100, // u
    0b0011100, // v
    0b0011100, // w
    0b1110110, // x
    0b1101110, // y
    0b1011011, // z
};

int digit1 = NOTHING;
int digit2 = NOTHING;
bool dot1 = false;
bool dot2 = false;

void push_and_shift(int bit) {
    digitalWrite(SER, bit);
    digitalWrite(SRCLK, HIGH);
    digitalWrite(SRCLK, LOW);
}

void latch() {
    digitalWrite(RCLK, HIGH);
    digitalWrite(RCLK, LOW);
}

void set_num(float num) {
    dot2 = false;

    // round up
    num = int(num * 10 + 0.5) / 10.0;

    if(num >= 10 and num < 100) {
        digit1 = int(num) / 10;
        digit2 = int(num) % 10;
        dot1 = false;
    } else if(num <= -1 and num > -10) {
        digit1 = MINUS;
        digit2 = -int(num);
        dot1 = false;
    } else if(num < 0 and num > -1) {
        digit1 = MINUS;
        digit2 = int(num * 10);
        dot1 = true;
    } else if(num >= 0 and num < 10) {
        digit1 = int(num);
        digit2 = int(num * 10) % 10;
        dot1 = true;
        if(digit2 == 0) {
            digit2 = digit1;
            digit1 = NOTHING;
            dot1 = false;
        }
    } else {
        digit1 = NOTHING;
        digit2 = NOTHING;
        dot1 = 0;
    }
}

void print() {
    // digit 1
    for(int i = 0; i < 7; i++) {
        int state = SEGMENT_LAYOUT[digit1] >> i & 1;
        push_and_shift(state);
    }
    push_and_shift(dot1);
    latch();

    digitalWrite(DIGIT1, HIGH);
    delay(DISPLAY_INTERVAL);
    digitalWrite(DIGIT1, LOW);

    // digit 2
    for(int i = 0; i < 7; i++) {
        int state = SEGMENT_LAYOUT[digit2] >> i & 1;
        push_and_shift(state);
    }
    push_and_shift(dot2);
    latch();

    digitalWrite(DIGIT2, HIGH);
    delay(DISPLAY_INTERVAL);
    digitalWrite(DIGIT2, LOW);
}

void setup() {
    pinMode(DIGIT1, OUTPUT);
    pinMode(DIGIT2, OUTPUT);
    pinMode(SER, OUTPUT);
    pinMode(OE, OUTPUT);
    pinMode(RCLK, OUTPUT);
    pinMode(SRCLK, OUTPUT);

    digitalWrite(DIGIT1, LOW);
    digitalWrite(DIGIT2, LOW);
    digitalWrite(SER, LOW);
    digitalWrite(OE, LOW);
    digitalWrite(RCLK, LOW);
    digitalWrite(SRCLK, HIGH);

    set_num(3.2);
}

void loop() {
    // print time elapsed
    float tot_time = -10;
    while(tot_time <= 15) {
        set_num(tot_time);
        long t = millis();
        print();
        tot_time += (millis() - t) / 1000.0;
    }

    delay(1000);

    char str[] = "hello, please brick your pi pi";
    for(int i = 0; i < BUFFER_LEN(str) - 1; i += 2) {
        char ch1 = str[i];
        char ch2 = str[i + 1];

        digit1 = ch1 - 'a' + LETTER_A;
        digit2 = ch2 - 'a' + LETTER_A;
        dot1 = false;
        dot2 = false;

        if(ch1 == ' ') {
            digit1 = NOTHING;
        } else if(ch1 == '.' or ch1 == ',') {
            digit1 = NOTHING;
            dot1 = true;
        }

        if(ch2 == ' ') {
            digit2 = NOTHING;
        } else if(ch2 == '.' or ch2 == ',') {
            digit2 = NOTHING;
            dot2 = true;
        }

        // print the characters for 1s
        long t = millis();
        while(millis() - t < 1000) {
            print();
        }
    }
}
