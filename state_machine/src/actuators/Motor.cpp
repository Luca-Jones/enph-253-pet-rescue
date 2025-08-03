#include <actuators/Motor.h>
#include <Arduino.h>

Motor::Motor(uint8_t pwm_pin, uint8_t pwm_channel, int pwm_frequency, uint8_t pwm_resolution, uint8_t dir_pin) {
    this->pwm_pin = pwm_pin;
    this->pwm_channel = pwm_channel;
    this->pwm_frequency = pwm_frequency;
    this->pwm_resolution = pwm_resolution;
    this->pwm_width = 1 << pwm_resolution;
    this->dir_pin = dir_pin;
    this->last_dir = -1; // the first time a direction is specified, the wait it applied just in case
}


Motor::~Motor() {
    ledcDetachPin(this->pwm_pin);
}

void Motor::setup() {
    pinMode(this->dir_pin, OUTPUT);
    ledcSetup(pwm_channel, pwm_frequency, pwm_resolution);
    ledcAttachPin(pwm_pin, pwm_channel);
}

void Motor::write(int speed, int direction) {
    if (direction != HIGH && direction != LOW) return;
    if (speed < 0) speed = 0;
    if (speed > pwm_width) speed = pwm_width;
    if (direction != last_dir || last_dir == -1) {
        last_dir = direction;
        ledcWrite(pwm_channel, 0);
        delayMicroseconds(100);
        digitalWrite(dir_pin, direction);
        delayMicroseconds(100);
    }
    ledcWrite(pwm_channel, speed);
}

void Motor::stop() {
    ledcWrite(pwm_channel, 0);
    delayMicroseconds(10);
}
