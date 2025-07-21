#include <state_machine/state_return_pets.h>
#include <sensors/RotaryEncoder.h>
#include <esp32-hal-ledc.h>
#include <config/pwm_config.h>
#include <config/pin_out.h>

#define CASCADE_RAISED_ANGLE 360 * 20 // TODO: determine the angle to raise
#define REVERSE_DRIVING_TIME_MS 2000

void state_return_pets_run(struct state_machine *state_machine) {
    
    // raise the cascade
    digitalWrite(PIN_CASCADE_DIR, HIGH); // TODO: associate HIGH to the correct direction
    ledcWrite(PWM_CHANNEL_CASCADE, PWM_MAX_DUTY_CASCADE);

    while (rot_get_angle() < CASCADE_RAISED_ANGLE);

    // drive backwards for some time, then stop
    digitalWrite(PIN_MOTOR_LEFT_DIR, LOW);  // TODO: associate with the correct direction
    digitalWrite(PIN_MOTOR_RIGHT_DIR, LOW);
    ledcWrite(PWM_CHANNEL_MOTOR_LEFT, PWM_MAX_DUTY_MOTOR_LEFT);
    ledcWrite(PWM_CHANNEL_MOTOR_RIGHT, PWM_MAX_DUTY_MOTOR_RIGHT);
    delay(REVERSE_DRIVING_TIME_MS);
    ledcWrite(PWM_CHANNEL_MOTOR_LEFT, 0);
    ledcWrite(PWM_CHANNEL_MOTOR_RIGHT, 0);

    // lower the cascade
    ledcWrite(PWM_CHANNEL_CASCADE, 0);
    delayMicroseconds(100);
    digitalWrite(PIN_CASCADE_DIR, LOW);
    ledcWrite(PWM_CHANNEL_CASCADE, PWM_MAX_DUTY_CASCADE);
    
    while (rot_get_angle() > 10); // something close to 0

    ledcWrite(PWM_CHANNEL_CASCADE, 0); // stop the cascade

    // post an internal event
    state_machine->internal_event = EVENT_PETS_RETURNED;

}

void state_return_pets_enter(struct state_machine *state_machine) {
    state_return_pets_run(state_machine);
}
