#include <sensors/Sonar.h>
#include <Arduino.h>
#include <config/pin_out.h>

void sonar_setup() {
    pinMode(PIN_SONAR_TRIG, OUTPUT);
    pinMode(PIN_SONAR_ECHO, INPUT);
}

int sonar_get_distance_cm() {
    digitalWrite(PIN_SONAR_TRIG, LOW);
    delayMicroseconds(SONAR_STABILIZING_DELAY_US);
    digitalWrite(PIN_SONAR_TRIG, HIGH);
    delayMicroseconds(SONAR_HIGH_TIME_US);
    digitalWrite(PIN_SONAR_TRIG, LOW);
    unsigned long duration_us = pulseIn(PIN_SONAR_ECHO, HIGH);
    return (duration_us * SPEED_OF_SOUND_CM_PER_US) / 2; // round trip distance divided by 2
}
