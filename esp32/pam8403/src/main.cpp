#include <Arduino.h>

#include <driver/dac.h>
#include <math.h>

void play_noise(int freq, int sample_rate, int amplitude, int offset) {
    int samples = sample_rate / freq;
    for(int i = 0; i < samples; i++) {
        float t = offset + amplitude * sin(2 * 3.141592654 * i / samples);
        dac_output_voltage(DAC_CHANNEL_1, t);
        delayMicroseconds(1000000 / sample_rate);
    }
}

void setup() {
    dac_output_enable(DAC_CHANNEL_1);
}

void loop() {
    for(int freq = 440; freq <= 2000; freq++) {
        play_noise(freq, 8000, 127, 128);
    }
    for(int freq = 1999; freq >= 440; freq--) {
        play_noise(freq, 8000, 127, 128);
    }
}

