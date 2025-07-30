#include <state_machine/state_return_pets.h>
#include <esp32-hal-ledc.h>
#include <config/pwm_config.h>
#include <config/pin_out.h>
#include <config/dir_config.h>

void state_return_pets_run(struct state_machine *state_machine) {

    // raise the cascade
    cascade_motor.write(PWM_MAX_DUTY_CASCADE, CASCADE_MOTOR_UP);
    delay(CASCADE_LIFT_TIME_MS);
    cascade_motor.stop();

    // drive backwards for some time, then stop
    left_motor.write(PWM_MAX_DUTY_MOTOR_LEFT, LEFT_MOTOR_BACKWARD);
    right_motor.write(PWM_MAX_DUTY_MOTOR_RIGHT, RIGHT_MOTOR_BACKWARD);
    delay(REVERSE_DRIVING_TIME_MS);
    left_motor.stop();
    right_motor.stop();

    // lower the cascade
    cascade_motor.write(PWM_MAX_DUTY_CASCADE, CASCADE_MOTOR_DOWN);
    delay(CASCADE_LIFT_TIME_MS);
    cascade_motor.stop();
    
    // move a little bit forwards
    left_motor.write(PWM_MAX_DUTY_MOTOR_LEFT, LEFT_MOTOR_FORWARD);
    right_motor.write(PWM_MAX_DUTY_MOTOR_RIGHT, RIGHT_MOTOR_FORWARD);
    delay(REVERSE_DRIVING_TIME_MS / 2);
    left_motor.stop();
    right_motor.stop();
    
    // post an internal event
    state_machine->internal_event = EVENT_PETS_RETURNED;

}

void state_return_pets_enter(struct state_machine *state_machine) {
    state_return_pets_run(state_machine);
}
