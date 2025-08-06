#include <actuators/BaseGear.h>
#include <sensors/MagneticEncoder.h>
#include <config/pwm_config.h>
#include <config/pin_out.h>
#include <math.h>
#include <esp32-hal-ledc.h>
#include <esp32-hal.h>
#include <config/dir_config.h>

#define BASE_GEAR_KP 1.5f
#define BASE_GEAR_KI 0.5f
#define BASE_GEAR_KD 2.0f   

#define BASE_GEAR_MAX_PID_ITERATIONS 100

#define constrain(val,low,high) (val < low ? low : ( val > high ? high : val ))

BaseGear::BaseGear(Motor *motor) {
    this->motor = motor;
}

BaseGear::~BaseGear() {}

void BaseGear::setup() {
    this->motor->setup();
    mag_setup();
}

void BaseGear::write(int target_angle) {

    if (target_angle < 0) target_angle = 0;
    if (target_angle > 360) target_angle = 360;
    
    int error = 360, last_error = 0, current_angle = 0;
    int proportional = 0, integral = 0, derivative = 0;
    float output = 0;
    int iterations = 0;

    while (iterations < BASE_GEAR_MAX_PID_ITERATIONS) { 
        
        current_angle = mag_get_angle();

        error = (current_angle > 135 ? current_angle - 360 : current_angle) - (target_angle > 135 ? target_angle - 360 : target_angle);

        proportional = error;
        integral = integral + error;     // idc about the time step
        integral = constrain(integral, -PWM_MAX_DUTY_BASE_GEAR, PWM_MAX_DUTY_BASE_GEAR);
        derivative = error - last_error; // idc about the time step
        last_error = error;
        output = BASE_GEAR_KP * proportional + BASE_GEAR_KI * integral + BASE_GEAR_KD * derivative;
        output = constrain(output, -PWM_MAX_DUTY_BASE_GEAR, PWM_MAX_DUTY_BASE_GEAR);
        if (fabs(output) < 30) output = 0;
        else if (fabs(output) <= PWM_MIN_DUTY_BASE_GEAR && output >= 30) output = PWM_MIN_DUTY_BASE_GEAR;
        this->motor->write(fabs(output), output > 0 ? BASE_GEAR_CCW : BASE_GEAR_CW);
        delay(50);

        iterations++;
    }
    
    this->motor->stop();
}