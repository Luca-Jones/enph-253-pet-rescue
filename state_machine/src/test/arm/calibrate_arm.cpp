#include <Arduino.h>
#include <actuators/Servo.h> // redundant
#include <actuators/Arm.h>
#include <config/pin_out.h>
#include <config/pwm_config.h>


Servo servo_1(ARM_SERVO_1_ANGLE_MAX);
Servo servo_2(ARM_SERVO_2_ANGLE_MAX);
Arm arm(&servo_1, &servo_2);


void setup() {
    Serial.begin(115200);
    servo_1.attach(PIN_SERVO_1, PWM_CHANNEL_SERVO_1, 500, 2500);
    servo_2.attach(PIN_SERVO_2, PWM_CHANNEL_SERVO_2, 500, 2500);
}

void loop() {
    
    servo_1.write(270);
    // servo_2.write(90);
    // if (Serial.available() > 0) {
        
    //     String input = Serial.readStringUntil('\n');
    //     Serial.println(input);

    //     int val = input.toInt();
        
    //     if (val == 1) {
    //         Serial.println("Input an angle for Servo 1");
    //         String next_input = Serial.readStringUntil('\n');
    //         Serial.println(next_input);
    //         val = next_input.toInt();
    //         servo_1.write(val);
    //     } else if (val == 2) {
    //         Serial.println("Input an angle for Servo 2");
    //         String next_input = Serial.readStringUntil('\n');
    //         Serial.println(next_input);
    //         val = next_input.toInt();
    //         servo_2.write(val);
    //     } else {
    //         Serial.println("Invalid input, choose Servo (1) or (2)");
    //     }
    // }
    delay(1000);

}