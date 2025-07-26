#include <Arduino.h>
#include <actuators/Servo.h> // redundant
#include <actuators/Arm.h>
#include <actuators/Claw.h>
#include <config/pin_out.h>
#include <config/pwm_config.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <actuators/BaseGear.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 // shared with the esp32 reset pin
Adafruit_SSD1306 display_handler(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Servo servo_1(ARM_SERVO_1_ANGLE_MAX);
Servo servo_2(ARM_SERVO_2_ANGLE_MAX);
Arm arm(&servo_1, &servo_2);
Servo claw(CLAW_OPEN);
BaseGear base_gear = BaseGear();


void setup() {
    Serial.begin(115200);
    servo_1.attach(PIN_SERVO_1, PWM_CHANNEL_SERVO_1, 500, 2500);
    servo_2.attach(PIN_SERVO_2, PWM_CHANNEL_SERVO_2, 500, 2500);
    claw.attach(PIN_SERVO_3, PWM_CHANNEL_SERVO_3, 500, 2500);
    base_gear.setup();

    display_handler.begin(SSD1306_SWITCHCAPVCC, 0x3C); // 3.3V at the default i2c addr
    display_handler.setTextSize(1);
    display_handler.setTextColor(SSD1306_WHITE);
    display_handler.clearDisplay();
    display_handler.setCursor(0, 0);
    display_handler.println("Hello World!");
    display_handler.display();

}

void loop() {
    base_gear.write(BASE_GEAR_HOME);
    // arm.move_to_pos(ARM_HOME_X, ARM_HOME_Y);                // go to home
    // claw.write(CLAW_OPEN);                                  
    // delay(2000);

    // // arm.lerp_to_pos(ARM_PILLAR_X, ARM_PILLAR_Y, 500);       // move up above pillar (straight up)
    // arm.lerp_to_pos(350, ARM_HOME_Y, 1000);               // move out towards pillar (straight forward)
    // delay(500);
    // claw.write(CLAW_CLOSED);                                // grab the pet
    // delay(1500);
    
    // // arm.lerp_to_pos(ARM_PILLAR_X, ARM_PILLAR_Y, 500);
    // // arm.lerp_to_pos(ARM_RAISED_X, ARM_RAISED_Y, 1000);       // raise the claw up higher and further in (arc path)
    // arm.move_to_phi(105, 225);

    delay(2000);
    base_gear.write(BASE_GEAR_STORE);
    // claw.write(CLAW_OPEN);                                  // let go of the pet
    delay(2000);

    /* CALIBRATION */
    // if (Serial.available() > 0) {
        
    //     String input = Serial.readStringUntil('\n');
    //     Serial.println(input);
    //     servo_1.write(input.toInt());
    //     // servo_2.write(input.toInt());
    // }
    // delay(1000);

}