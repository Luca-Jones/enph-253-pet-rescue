#ifndef PWM_CONFIG_H
#define PWM_CONFIG_H

#define PWM_CHANNEL_SERVO_1         14          // timer 3      50 Hz
#define PWM_CHANNEL_SERVO_2         15          // timer 3      50 Hz
#define PWM_CHANNEL_SERVO_3         6           // timer 3      50 Hz
#define PWM_CHANNEL_MOTOR_LEFT      12          // timer 2      1 kHz
#define PWM_CHANNEL_MOTOR_RIGHT     13          // timer 2      1 kHz
#define PWM_CHANNEL_CASCADE         8           // timer 0      1 kHz
#define PWM_CHANNEL_BASE_GEAR       11          // timer 1      50 Hz

#define PWM_FRQ_HZ_MOTOR_LEFT       1000
#define PWM_FRQ_HZ_MOTOR_RIGHT      1000
#define PWM_FRQ_HZ_CASCADE          1000
#define PWM_FRQ_HZ_BASE_GEAR        50

#define PWM_RESOLUTION_MOTOR_LEFT   8
#define PWM_RESOLUTION_MOTOR_RIGHT  8
#define PWM_RESOLUTION_CASCADE      8
#define PWM_RESOLUTION_BASE_GEAR    8

#define PWM_MAX_DUTY_MOTOR_LEFT     1 << PWM_RESOLUTION_MOTOR_LEFT
#define PWM_MAX_DUTY_MOTOR_RIGHT    1 << PWM_RESOLUTION_MOTOR_RIGHT
#define PWM_MAX_DUTY_CASCADE        1 << PWM_RESOLUTION_CASCADE
#define PWM_MIN_DUTY_BASE_GEAR      60
#define PWM_MAX_DUTY_BASE_GEAR      65
#endif