#include <Arduino.h>
#include <tb6612fng.h>

#define STBY 5
#define PWMA 22
#define PWMB 23
#define AIN1 18
#define AIN2 19
#define BIN1 16
#define BIN2 17

MotorOutput a(PWMA, 0, AIN1, AIN2);
MotorOutput b(PWMB, 1, BIN1, BIN2);
MotorDriver motor(STBY, a, b);

void setup() {
    a.setup_pwm(8000, 10);
    b.setup_pwm(8000, 10);

    motor.reset();
    motor.activate();
}

void loop() {
    // brake test
    motor.cw();
    motor.speed(1023);
    delay(1000);
    motor.brake();
    delay(1000);

    // coast test
    motor.cw();
    motor.speed(1023);
    delay(1000);
    motor.coast();
    delay(1000);

    // reverse direction test
    motor.cw(CHANNEL_A);
    motor.ccw(CHANNEL_B);
    for(int i = 0; i < 1000; i++) {
        motor.speed(1023);
        delay(10);
        if(i % 2) {
            motor.cw(CHANNEL_A);
            motor.ccw(CHANNEL_B);
        } else {
            motor.cw(CHANNEL_B);
            motor.ccw(CHANNEL_A);
        }
    }
    motor.brake();
    delay(1000);

    // on/off test
    motor.cw();
    motor.speed(1023);
    for(int i = 0; i < 10; i++) {
        if(i % 2) {
            motor.deactivate();
        } else {
            motor.activate();
        }
        delay(1000);
    }
    motor.brake();
    delay(1000);
    motor.activate();
}
