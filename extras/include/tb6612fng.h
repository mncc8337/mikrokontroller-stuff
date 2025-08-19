#ifndef TB6612FNG_H
#define TB6612FNG_H

#include <Arduino.h>

#define CHANNEL_A 0
#define CHANNEL_B 1

class MotorOutput {
private:
    int PWM;
    int PWM_CHN;
    int PWM_RES;
    int IN1, IN2;
public:
    MotorOutput(int pwm = -1, int pwm_chn = -1, int in1 = -1, int in2 = -1) {
        PWM = pwm;
        PWM_CHN = pwm_chn;
        IN1 = in1;
        IN2 = in2;

        if(IN1 >= 0 and IN2 >= 0) {
            pinMode(IN1, OUTPUT);
            pinMode(IN2, OUTPUT);
        }
    }

    void setup_pwm(int pwm_freq, int pwm_res) {
        if(PWM < 0) {
            return;
        }

        PWM_RES = pwm_res;
        ledcSetup(PWM_CHN, pwm_freq, pwm_res);
        ledcAttachPin(PWM, PWM_CHN);
    }

    void speed(int spd) {
        ledcWrite(PWM_CHN, spd);
    }

    void cw() {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
    }

    void ccw() {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
    }

    void brake() {
        speed((1 << PWM_RES) - 1);
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, HIGH);
    }

    void coast() {
        speed(0);
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
    }
};

class MotorDriver {
private:
    int STBY;
    MotorOutput* CHN[2];
public:
    MotorDriver(int stby, MotorOutput& chn_a, MotorOutput& chn_b) {
        STBY = stby;
        CHN[0] = &chn_a;
        CHN[1] = &chn_b;

        pinMode(STBY, OUTPUT);
        deactivate();
    }

    void reset() {
        deactivate();
        coast();
    }

    void activate() {
        digitalWrite(STBY, HIGH);
    }

    void deactivate() {
        digitalWrite(STBY, LOW);
    }

    void speed(int channel, int spd) {
        CHN[channel]->speed(spd);
    }

    void speed(int spd) {
        CHN[0]->speed(spd);
        CHN[1]->speed(spd);
    }

    void cw(int channel) {
        CHN[channel]->cw();
    }

    void cw() {
        CHN[0]->cw();
        CHN[1]->cw();
    }

    void ccw(int channel) {
        CHN[channel]->ccw();
    }

    void ccw() {
        CHN[0]->ccw();
        CHN[1]->ccw();
    }

    void brake(int channel) {
        CHN[channel]->brake();
    }

    void brake() {
        CHN[0]->brake();
        CHN[1]->brake();
    }

    void coast(int channel) {
        CHN[channel]->coast();
    }

    void coast() {
        CHN[0]->coast();
        CHN[1]->coast();
    }
};

#endif
