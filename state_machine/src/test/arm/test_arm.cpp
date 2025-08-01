#include <Arduino.h>
#include <actuators/Servo.h> // redundant
#include <actuators/Arm.h>
#include <actuators/Claw.h>
#include <config/pin_out.h>
#include <config/pwm_config.h>
#include <actuators/BaseGear.h>

Servo servo_1(ARM_SERVO_1_ANGLE_MAX);
Servo servo_2(ARM_SERVO_2_ANGLE_MAX);
Arm arm(&servo_1, &servo_2);
Servo claw(CLAW_OPEN);
Motor base_gear_motor(PIN_BASE_GEAR_PWM, PWM_CHANNEL_BASE_GEAR, PWM_FRQ_HZ_BASE_GEAR, PWM_RESOLUTION_BASE_GEAR, PIN_BASE_GEAR_DIR);
BaseGear base_gear = BaseGear(&base_gear_motor);

void setup() {
    Serial.begin(115200);
    servo_1.attach(PIN_SERVO_1, PWM_CHANNEL_SERVO_1, 500, 2500);
    servo_2.attach(PIN_SERVO_2, PWM_CHANNEL_SERVO_2, 500, 2500);
    claw.attach(PIN_SERVO_3, PWM_CHANNEL_SERVO_3, 500, 2500);
    base_gear.setup();
}

void loop() {

    arm.move_to_phi(0, 90);

    // // base_gear.write(BASE_GEAR_HOME);
    // delay(1000);
    // arm.move_to_pos(ARM_HOME_X, ARM_HOME_Y);                // go to home
    // delay(1000);
    // claw.write(CLAW_OPEN);                                  
    // delay(2000);
    // arm.lerp_to_pos(350, ARM_HOME_Y, 2000);                 // move outwards
    // delay(500);
    // claw.write(CLAW_CLOSED);                                // grab the pet
    // delay(1000);
    // arm.lerp_to_pos(ARM_RAISED_X, ARM_RAISED_Y, 500);      // raise up the pet
    // delay(1000);
    // // base_gear.write(BASE_GEAR_STORE);                       // turn to the bucket
    // delay(500);
    // claw.write(CLAW_OPEN);                                  // let go of the pet
    delay(5000);
}
