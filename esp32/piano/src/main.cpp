#include <Arduino.h>

#define BUTC_PIN 0
#define BUTD_PIN 27
#define BUTE_PIN 4
#define BUTF_PIN 5
#define BUTG_PIN 12
#define BUTA_PIN 13
#define BUTB_PIN 14
#define BUTc_PIN 15

#define BUT_REC_PIN 25
#define BUT_PLAY_PIN 26

#define BUZZER_PIN 23
#define LED_PIN 22

#define PWD_CHANNEL 0
#define PWD_RESOLUTION 8

#define BUTC 0
#define BUTD 1
#define BUTE 2
#define BUTF 3
#define BUTG 4
#define BUTA 5
#define BUTB 6
#define BUTc 7
#define BUT_REC 8
#define BUT_PLAY 9
#define BUT_EMPTY 10

struct {
    int button;
    int duration;
} typedef instruction_t;

enum Mode {
    IDLE,
    RECORDING,
    PLAYING,
};

const int buttons[BUT_EMPTY] = {
    BUTC_PIN,
    BUTD_PIN,
    BUTE_PIN,
    BUTF_PIN,
    BUTG_PIN,
    BUTA_PIN,
    BUTB_PIN,
    BUTc_PIN,
    BUT_REC_PIN,
    BUT_PLAY_PIN,
};

bool button_pressed[BUT_EMPTY];
bool prev_funcion_button[2];

int current_mode = IDLE;

instruction_t instructions[512];
size_t instruction_len = 0;

instruction_t current_instruction = {BUT_EMPTY, 0};

int get_freq(int id) {
    switch(id) {
        case BUTC:
            return 523;
        case BUTD:
            return 587;
        case BUTE:
            return 659;
        case BUTF:
            return 698;
        case BUTG:
            return 784;
        case BUTA:
            return 880;
        case BUTB:
            return 988;
        case BUTc:
            return 1047;
    }

    return 0;
}

void setup() {
    ledcSetup(PWD_CHANNEL, 2000, PWD_RESOLUTION);
    ledcAttachPin(BUZZER_PIN, PWD_CHANNEL);
    ledcWrite(PWD_CHANNEL, 0);
    
    for(int i = 0; i < BUT_EMPTY; i++)
        pinMode(buttons[i], INPUT_PULLUP);
    memset(button_pressed, false, BUT_EMPTY);
    memset(prev_funcion_button, false, 2);

    pinMode(LED_PIN, OUTPUT);

    current_mode = IDLE;
}

void loop() {
    // poll button states
    int beeping = -1;
    for(int i = 0; i < BUT_EMPTY; i++) {
        int prev_val = button_pressed[i];
        button_pressed[i] = digitalRead(buttons[i]) == LOW;
        if(button_pressed[i] && i < 8) {
            ledcSetup(PWD_CHANNEL, get_freq(i), PWD_RESOLUTION);
            ledcWrite(PWD_CHANNEL, 200);
            beeping = i;
            break;
        }
        if(i >= 8)
            prev_funcion_button[i - 8] = prev_val;
    }
    if(beeping < 0) {
        ledcWrite(PWD_CHANNEL, 0);
        beeping = BUT_EMPTY;
    }

    digitalWrite(LED_PIN, LOW);

    switch(current_mode) {
        case IDLE:
            if(!button_pressed[BUT_PLAY] && prev_funcion_button[1]) {
                if(instruction_len > 0)
                    current_mode = PLAYING;
            } else if(!button_pressed[BUT_REC] && prev_funcion_button[0]) {
                current_mode = RECORDING;
                instruction_len = 0;
                current_instruction.button = -2;
            }
            break;
        case RECORDING:
            if((!button_pressed[BUT_REC] && prev_funcion_button[0]) || instruction_len == 512) {
                current_mode = IDLE;
                break;
            }

            digitalWrite(LED_PIN, HIGH);

            if(beeping == current_instruction.button)
                current_instruction.duration += 10;
            else {
                if(current_instruction.button != -2)
                    instructions[instruction_len++] = current_instruction;
                current_instruction.button = beeping;
                current_instruction.duration = 10;
            }
            break;
        case PLAYING:
            digitalWrite(LED_PIN, HIGH);

            for(int i = 0; i < instruction_len; i++) {
                if(instructions[i].button == BUT_EMPTY) {
                    ledcWrite(PWD_CHANNEL, 0);
                    delay(instructions[i].duration);
                } else {
                    ledcSetup(PWD_CHANNEL, get_freq(instructions[i].button), PWD_RESOLUTION);
                    ledcWrite(PWD_CHANNEL, 200);
                    delay(instructions[i].duration);
                }
            }
            current_mode = IDLE;
            break;
    }

    delay(10);
}
