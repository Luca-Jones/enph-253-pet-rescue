#ifndef PIN_OUT_H
#define PIN_OUT_H

/* 
    Defines what each pin on the ESP32 is used for.
*/

#define PIN_SONAR_ECHO          37
#define PIN_SONAR_TRIG          38

#define PIN_ROT_A               34
#define PIN_ROT_B               35

#define PIN_IR_SENSOR_LL        9
#define PIN_IR_SENSOR_L         10
#define PIN_IR_SENSOR_C         5
#define PIN_IR_SENSOR_R         7
#define PIN_IR_SENSOR_RR        8

#define PIN_SERVO_1             4
#define PIN_SERVO_2             2
#define PIN_SERVO_3             15

#define PIN_UART_TX             1
#define PIN_UART_RX             3

#define PIN_LIMIT_SWITCH        0

#define PIN_MOTOR_LEFT_PWM      13
#define PIN_MOTOR_LEFT_DIR      12
#define PIN_MOTOR_RIGHT_PWM     14
#define PIN_MOTOR_RIGHT_DIR     27

#define PIN_CASCADE_PWM         33
#define PIN_CASCADE_DIR         32

#define PIN_BASE_GEAR_PWM       26
#define PIN_BASE_GEAR_DIR       25

#define PIN_START_BUTTON        19

#define PIN_I2C_SCL             22
#define PIN_I2C_SDA             21
#define I2C_FRQ_HZ 400000

/* unused pins */   

#define PIN_UNUSED_1            20
#define PIN_UNUSED_2            39
#define PIN_UNUSED_3            36

#endif