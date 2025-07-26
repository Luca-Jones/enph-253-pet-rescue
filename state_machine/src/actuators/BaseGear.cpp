#include <actuators/BaseGear.h>
#include <sensors/MagneticEncoder.h>
#include <config/pwm_config.h>
#include <config/pin_out.h>
#include <math.h>
#include <esp32-hal-ledc.h>
#include <esp32-hal.h>

#include <Arduino.h>

#define BASE_GEAR_CW        LOW
#define BASE_GEAR_CCW       HIGH

#define BASE_GEAR_KP 8.0f
#define BASE_GEAR_KI 0.0f
#define BASE_GEAR_KD 6.0f

#define BASE_GEAR_MAX_OUTPUT (PWM_MAX_DUTY_BASE_GEAR)
#define BASE_GEAR_MIN_OUTPUT -(PWM_MAX_DUTY_BASE_GEAR)
#define BASE_GEAR_PID_ITERATIONS 20

BaseGear::BaseGear() {}
BaseGear::~BaseGear() {}

void BaseGear::setup() {
    pinMode(PIN_BASE_GEAR_DIR, OUTPUT);
    ledcSetup(PWM_CHANNEL_BASE_GEAR, PWM_FRQ_HZ_BASE_GEAR, PWM_RESOLUTION_BASE_GEAR);
    ledcAttachPin(PIN_BASE_GEAR_PWM, PWM_CHANNEL_BASE_GEAR);
}

void BaseGear::write(int angle) {

    if (angle < 0) angle = 0;
    if (angle > 360) angle = 360;
    
    int error = 360, last_error = 0, proportional = 0, integral = 0, derivative = 0, current_angle = 0;
    float output = 0;

    Serial.print("(error, output) = ");
    while (fabs(error) > 3) {
        
        current_angle = mag_get_angle();

        error = (current_angle > 180 ? current_angle - 360 : current_angle) - (angle > 180 ? angle - 360 : angle);

        Serial.printf("(%d, ", error);
        proportional = error;
        integral = fmin(fmax(integral, BASE_GEAR_MIN_OUTPUT), BASE_GEAR_MAX_OUTPUT);
        derivative = error - last_error; // idc about the time step
        last_error = error;
        output = BASE_GEAR_KP * proportional + BASE_GEAR_KI * integral + BASE_GEAR_KD * derivative;
        output = fmin(fmax(output, BASE_GEAR_MIN_OUTPUT), BASE_GEAR_MAX_OUTPUT);
        Serial.printf("%f), ", output);
        ledcWrite(PWM_CHANNEL_BASE_GEAR, 0);
        delayMicroseconds(10);
        digitalWrite(PIN_BASE_GEAR_DIR, output > 0 ? BASE_GEAR_CCW : BASE_GEAR_CW);
        ledcWrite(PWM_CHANNEL_BASE_GEAR, fabs(output));
        delay(100);
    }
    Serial.println("");
    ledcWrite(PWM_CHANNEL_BASE_GEAR, 0);
    delayMicroseconds(10);

}