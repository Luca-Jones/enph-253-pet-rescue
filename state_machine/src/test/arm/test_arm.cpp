#include <Arduino.h>
#include <actuators/Servo.h> // redundant
#include <actuators/Arm.h>
#include <actuators/Claw.h>
#include <config/pin_out.h>
#include <config/pwm_config.h>

Servo servo_1(ARM_SERVO_1_ANGLE_MAX);
Servo servo_2(ARM_SERVO_2_ANGLE_MAX);
Arm arm(&servo_1, &servo_2);
Servo claw(CLAW_OPEN);

void setup() {
    Serial.begin(115200);
    servo_1.attach(PIN_SERVO_1, PWM_CHANNEL_SERVO_1, 500, 2500);
    servo_2.attach(PIN_SERVO_2, PWM_CHANNEL_SERVO_2, 500, 2500);
    claw.attach(PIN_SERVO_3, PWM_CHANNEL_SERVO_3, 500, 2500);
}

void loop() {
    arm.move_to_pos(ARM_HOME_X, ARM_HOME_Y);                // go to home
    claw.write(CLAW_SEMI_OPEN);                             // open claw (small opening)
    delay(500);
    arm.lerp_to_pos(ARM_PILLAR_X, ARM_PILLAR_Y, 500);       // move up above pillar (straight up)
    arm.lerp_to_pos(350, ARM_PILLAR_Y, 1000);               // move out towards pillar (straight forward)
    delay(500);
    claw.write(CLAW_CLOSED);                                // grab the pet
    delay(500);
    arm.move_to_pos(ARM_RAISED_X, ARM_RAISED_Y);            // raise the claw up higher and further in (arc path)
    delay(2000);
    claw.write(CLAW_OPEN);                                  // let go of the pet
    delay(500);
}