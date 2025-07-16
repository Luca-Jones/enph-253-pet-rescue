#include <Arduino.h>
#include <Arm.h>
#include <Servo.h>

#define POT_PIN_1 33
#define POT_PIN_2 26
#define BUTTON_PIN 27

#define CLAW_PIN 4
#define CLAW_ANGLE_MAX 180

Servo servo_1(ARM_SERVO_1_ANGLE_MAX);
Servo servo_2(ARM_SERVO_2_ANGLE_MAX);
Servo claw(CLAW_ANGLE_MAX);
volatile bool is_claw_closed = false;
Arm arm(&servo_1, &servo_2);
volatile bool button_pressed = false;
volatile int counter = 0;


// IRAM safe code
void IRAM_ATTR switchPressedISR() {
    button_pressed = gpio_get_level((gpio_num_t) BUTTON_PIN);
    if(!button_pressed) {
        counter++;
        is_claw_closed = !is_claw_closed;
    }
}

void setup() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), switchPressedISR, CHANGE);
    Serial.begin(115200);
    Serial.setTimeout(1000000);
    servo_1.attach(ARM_SERVO_1_PIN, ARM_SERVO_1_CHANNEL, 500, 2500);
    servo_2.attach(ARM_SERVO_2_PIN, ARM_SERVO_2_CHANNEL, 500, 2500);
    claw.attach(CLAW_PIN, 3, 500, 2500);

}

void log_pos() {
    int theta_1; int theta_2; float phi_1; float phi_2; int x; int y;
    arm.log_pos(&theta_1, &theta_2, &phi_1, &phi_2, &x, &y);
    Serial.printf("x = %d, y = %d\nphi_1 = %f, phi_2 = %f\ntheta_1 = %d, theta_2 = %d\n\n\n", x, y, phi_1, phi_2, theta_1, theta_2);
}

void calibrate() {
    String input = Serial.readStringUntil('\n');
    Serial.println(input);
    servo_1.write(input.toInt());
}

void loop() {

    // calibrate();
    
    claw.write(140);
    arm.lerp_to_pos(250,50,250);
    delay(1000);
    arm.lerp_to_pos(250,200,250);
    delay(500);
    arm.lerp_to_pos(350,200,250);                   
    delay(500);
    claw.write(95);
    delay(500);
    arm.lerp_to_pos(220,210,250);
    delay(500);
    arm.lerp_to_pos(220,300,250);
    delay(500);
    claw.write(180);
    delay(5000);

    // claw.write(180);
    // arm.lerp_to_pos(250,50,250);
    // delay(1500);
    // arm.lerp_to_pos(250,100,250);
    // delay(500);
    // arm.lerp_to_pos(350,100,250);
    // delay(500);
    // claw.write(100);
    // delay(500);
    // arm.lerp_to_pos(350,300,250);
    // delay(500);
    // arm.lerp_to_pos(180,300,250);
    // delay(500);
    // claw.write(180);
    // delay(5000);
    
}

// if (button_pressed) {
//     Serial.println(counter);
//     if (is_claw_closed) {
//         claw.write(110);
//         Serial.println("Claw CLOSED");
//     } else {
//         claw.write(180);
//         Serial.println("Claw OPEN");
//     }
// }
// button_pressed = false;