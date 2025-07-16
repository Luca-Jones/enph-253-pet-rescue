#include <Arduino.h>
// #include <ESP32Servo.h>
#include <Servo.h>

// ESP32 recommends only the following pins 2,4,12-19,21-23,25-27,32-33
#define CLAW_PIN 4
#define POT_PIN 25
#define CLAW_SERVO_MAX_ANGLE 180

Servo claw(CLAW_SERVO_MAX_ANGLE);
int pot_val = 0;

void setup() {
    Serial.begin(115200);
	claw.attach(CLAW_PIN, 0, 500, 2500);

    if (!claw.attached()) 
       Serial.println("servo not attached to a pin");

}

void loop() {

    // pot_val = analogRead(POT_PIN);
    // Serial.println(pot_val);
    // Serial.println((int) (pot_val / (float) (1 << 12) * SERVO_MAX_ANGLE));
    // int angle = (int) (pot_val / (float) (1 << 12) * 180);
    claw.write(180);
    delay(1000);
    claw.write(100);
    delay(1000);

    // claw.write(0);
    // delay(1000);
    // claw.write(180);
    // delay(1000);
}
