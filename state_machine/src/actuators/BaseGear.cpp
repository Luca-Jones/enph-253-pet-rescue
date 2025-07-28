#include <actuators/BaseGear.h>
#include <sensors/MagneticEncoder.h>
#include <config/pwm_config.h>
#include <config/pin_out.h>
#include <math.h>
#include <esp32-hal-ledc.h>
#include <esp32-hal.h>

#define BASE_GEAR_CW        LOW
#define BASE_GEAR_CCW       HIGH

#define BASE_GEAR_KP 8.0f
#define BASE_GEAR_KI 0.0f
#define BASE_GEAR_KD 6.0f

#define BASE_GEAR_MAX_PID_ITERATIONS 100
#define BASE_GEAR_MAX_OUTPUT (PWM_MAX_DUTY_BASE_GEAR)
#define BASE_GEAR_MIN_OUTPUT -(PWM_MAX_DUTY_BASE_GEAR)

#define constrain(val,low,high) (val < low ? low : ( val > high ? high : val ))

BaseGear::BaseGear(Motor *motor) {
    this->motor = motor;
}

BaseGear::~BaseGear() {}

void BaseGear::setup() {
    this->motor->setup();
    mag_setup();
    pinMode(PIN_BASE_GEAR_DIR, OUTPUT);
    ledcSetup(PWM_CHANNEL_BASE_GEAR, PWM_FRQ_HZ_BASE_GEAR, PWM_RESOLUTION_BASE_GEAR);
    ledcAttachPin(PIN_BASE_GEAR_PWM, PWM_CHANNEL_BASE_GEAR);
}

void BaseGear::write(int angle) {

    if (angle < 0) angle = 0;
    if (angle > 360) angle = 360;
    
    int error = 360, last_error = 0, current_angle = 0;
    int proportional = 0, integral = 0, derivative = 0;
    float output = 0;
    int iterations = 0;

    while (fabs(error) > 3 && iterations < BASE_GEAR_MAX_PID_ITERATIONS) {
        
        current_angle = mag_get_angle();

        error = (current_angle > 180 ? current_angle - 360 : current_angle) - (angle > 180 ? angle - 360 : angle);

        proportional = error;
        integral = constrain(integral, BASE_GEAR_MIN_OUTPUT, BASE_GEAR_MAX_OUTPUT);
        derivative = error - last_error; // idc about the time step
        last_error = error;
        output = BASE_GEAR_KP * proportional + BASE_GEAR_KI * integral + BASE_GEAR_KD * derivative;
        output = constrain(output, BASE_GEAR_MIN_OUTPUT, BASE_GEAR_MAX_OUTPUT);
        this->motor->write(fabs(output), output > 0 ? BASE_GEAR_CCW : BASE_GEAR_CW);
        delay(100);

        iterations++;
    }
    
    this->motor->stop();
}