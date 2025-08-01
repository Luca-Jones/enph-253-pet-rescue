#include <Arduino.h>
#include <config/pin_out.h>

#define SWITCH_CLOSED   LOW
#define SWITCH_OPEN     HIGH

volatile bool switch_triggered = false;

void setup() {
    Serial.begin(115200);
    pinMode(PIN_LIMIT_SWITCH, INPUT_PULLUP);
}

void loop() {
    if (digitalRead(PIN_LIMIT_SWITCH) == SWITCH_CLOSED && !switch_triggered) {
        Serial.println("PET GRASPED!");
        switch_triggered = true;
    } else if (digitalRead(PIN_LIMIT_SWITCH) == SWITCH_OPEN && switch_triggered) {
        Serial.println("No Pet...");
        switch_triggered = false;
    }
    delay(100);
}