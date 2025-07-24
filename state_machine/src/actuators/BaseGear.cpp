#include <actuators/BaseGear.h>
#include <sensors/MagneticEncoder.h>
#include <config/pwm_config.h>
#include <config/pin_out.h>
#include <math.h>
#include <esp32-hal-ledc.h>
#include <esp32-hal.h>

#define BASE_GEAR_KP 1
#define BASE_GEAR_KI 1
#define BASE_GEAR_KD 1

#define BASE_GEAR_MAX_OUTPUT PWM_MAX_DUTY_BASE_GEAR
#define BASE_GEAR_MIN_OUTPUT -PWM_MAX_DUTY_BASE_GEAR
#define BASE_GEAR_INTEGRAL_DECAY 0.95
#define BASE_GEAR_PID_ITERATIONS 10

BaseGear::BaseGear() {}
BaseGear::~BaseGear() {}

void BaseGear::write(int angle) {

    if (angle < 0) angle = 0;
    if (angle > 360) angle = 360;
    
    int error = 0, last_error = 0, proportional = 0, integral = 0, derivative = 0, output = 0;

    for (int i = 0; i < BASE_GEAR_PID_ITERATIONS; i ++) {
        error = angle - mag_get_angle();
        proportional = error;
        integral = integral * BASE_GEAR_INTEGRAL_DECAY + error;
        integral = fmin(fmax(integral, BASE_GEAR_MIN_OUTPUT), BASE_GEAR_MAX_OUTPUT);
        derivative = error - last_error; // idc about the time step
        last_error = error;
        output = BASE_GEAR_KP * proportional + BASE_GEAR_KI * integral + BASE_GEAR_KD * derivative;
        output = fmin(fmax(output, BASE_GEAR_MIN_OUTPUT), BASE_GEAR_MAX_OUTPUT);
        ledcWrite(PWM_CHANNEL_BASE_GEAR, 0);
        delayMicroseconds(100);
        digitalWrite(PIN_BASE_GEAR_DIR, output > 0 ? HIGH : LOW);
        ledcWrite(PWM_CHANNEL_BASE_GEAR, fabs(output));
        delay(BASE_GEAR_TURNING_TIME_MS / BASE_GEAR_PID_ITERATIONS);
    }

}