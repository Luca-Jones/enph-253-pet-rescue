#ifndef PIN_OUT_H
#define PIN_OUT_H

/* 
    Defines what each pin on the ESP32 is used for.
*/

#define PIN_SERVO_1             4
#define PIN_SERVO_2             2
#define PIN_SERVO_3             15

#define PIN_MOTOR_LEFT_PWM      13
#define PIN_MOTOR_LEFT_DIR      12
#define PIN_MOTOR_RIGHT_PWM     14
#define PIN_MOTOR_RIGHT_DIR     27
#define PIN_CASCADE_PWM         26
#define PIN_CASCADE_DIR         25
#define PIN_BASE_GEAR_PWM       33
#define PIN_BASE_GEAR_DIR       32

#define PIN_IR_SENSOR_LL        37
#define PIN_IR_SENSOR_L         38
#define PIN_IR_SENSOR_C         34
#define PIN_IR_SENSOR_R         35
#define PIN_IR_SENSOR_RR        9

#define PIN_SONAR_ECHO          10
#define PIN_SONAR_TRIG          5

#define PIN_LIMIT_SWITCH        19

#define PIN_I2C_SCL             22
#define PIN_I2C_SDA             21
#define I2C_FRQ_HZ 400000

/* unused pins */   

#define PIN_UART_TX             1
#define PIN_UART_RX             3

#define PIN_UNUSED_1            7
#define PIN_UNUSED_2            8
#define PIN_UNUSED_3            20
#define PIN_UNUSED_4            39
#define PIN_UNUSED_5            36


#endif