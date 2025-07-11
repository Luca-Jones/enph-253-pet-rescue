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
    servo_1.attach(ARM_SERVO_1_PIN, ARM_SERVO_1_CHANNEL, 500, 2500);
    servo_2.attach(ARM_SERVO_2_PIN, ARM_SERVO_2_CHANNEL, 500, 2500);
    claw.attach(CLAW_PIN, 3, 500, 2500);
}

void log_pos() {
    int x_2, y_2;
    arm.get_pos(&x_2, &y_2);
    Serial.printf("x = %d, y = %d\n", x_2, y_2);
    Serial.printf("theta_1 = %d, theta_2 = %d\n\n", servo_1.read(), servo_2.read());
}

void calibrate() {
    /* Servo calibration with pots */
    int pot_1 = analogRead(POT_PIN_1);
    int pot_2 = analogRead(POT_PIN_2);
    Serial.printf("servo 1: %d\n", pot_1);
    Serial.printf("servo 2: %d\n", pot_2);
    int angle_1 = (int) (pot_1 / (float) (1 << 12) * 270);
    int angle_2 = (int) (pot_2 / (float) (1 << 12) * 270);

    servo_1.write(angle_1);
    servo_2.write(angle_2);
}

void loop() {

    // calibrate();
    // delay(1000);
    
    // arm.move_to_angle(270-ARM_SERVO_1_ANGLE_OFFSET,270);
    // log_pos();
    // delay(500);

    /* Tests */
    arm.lerp_to_pos(200,100,500);
    delay(500);
    arm.lerp_to_pos(200,200,500);
    delay(500);
    arm.lerp_to_pos(300,200,500);
    delay(500);
    arm.lerp_to_pos(300,100,500);
    delay(500);

    if (button_pressed) {
        Serial.println(counter);
        if (is_claw_closed) {
            claw.write(110);
            Serial.println("Claw CLOSED");
        } else {
            claw.write(180);
            Serial.println("Claw OPEN");
        }
    }
    button_pressed = false;
}