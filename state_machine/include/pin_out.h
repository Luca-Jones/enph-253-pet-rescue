#ifndef PIN_OUT_H
#define PIN_OUT_H

/* 
    Defines what each pin on the ESP32 is used for
    as well as each PWM channel.
*/

#define PIN_IR_SENSOR_L 14
#define PIN_IR_SENSOR_C 27
#define PIN_IR_SENSOR_R 33

#define PIN_MOTOR_LEFT_PWM 8
#define PIN_MOTOR_LEFT_DIR 19
#define PIN_MOTOR_RIGHT_PWM 7
#define PIN_MOTOR_RIGHT_DIR 5

#define PIN_START_BUTTON 10

#define PIN_ARM_SERVO_1 19
#define CHANNEL_ARM_SERVO_1 1
#define CHANNEL_ARM_SERVO_2 2
#define PIN_ARM_SERVO_2 25
#define PIN_CLAW 4
#define CHANNEL_CLAW 3

#define PIN_SDA 21
#define PIN_SCL 22


#endif