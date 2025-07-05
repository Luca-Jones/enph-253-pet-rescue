#include <Arduino.h>
// #include <ESP32Servo.h>
#include <Servo.h>

// ESP32 recommends only the following pins 2,4,12-19,21-23,25-27,32-33
#define CLAW_PIN 14
#define POT_PIN 25

Servo claw;
int pot_val = 0;

void setup() {
    Serial.begin(115200);
	claw.attach(CLAW_PIN, 0);

    if (!claw.attached()) 
       Serial.println("servo not attached to a pin");

}

void loop() {

    // pot_val = analogRead(POT_PIN);
    // Serial.println(pot_val);
    // Serial.println((int) (pot_val / (float) (1 << 12) * SERVO_MAX_ANGLE));
    // claw.write((int) (pot_val / (float) (1 << 12) * SERVO_MAX_ANGLE));

    // claw.write(0);
    claw.writeMicroseconds(1000);
    delay(1000);
    // claw.write(180);
    claw.writeMicroseconds(2000);
    delay(1000);
}