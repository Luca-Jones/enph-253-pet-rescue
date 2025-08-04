#include <state_machine/state_tape_following.h>
#include <esp32-hal.h>
#include <config/dir_config.h>

#define TAPE_FOLLOWING_BASE_SPEED       80
#define TAPE_FOLLOWING_MAX_SPEED        255
#define TAPE_FOLLOWING_RECOVERY_SPEED   255

#define KP 35
#define KD 5

#define TAPE_FOLLOWING_PERIOD 10

/* Helper functions */
void control_motors(float pid_output);
float calculate_error(float last_error, bool ll, bool l, bool c, bool r, bool rr);

void state_tape_following_run(struct state_machine *state_machine) {

    unsigned long now = millis();
    if (state_machine->last_pid_time == 0) {
        state_machine->last_pid_time = now;
    } 
    else if (now - state_machine->last_pid_time < TAPE_FOLLOWING_PERIOD) {
        return;
    }

    bool ir_ll = digitalRead(PIN_IR_SENSOR_LL);
    bool ir_l  = digitalRead(PIN_IR_SENSOR_L);
    bool ir_c  = digitalRead(PIN_IR_SENSOR_C);
    bool ir_r  = digitalRead(PIN_IR_SENSOR_R);
    bool ir_rr = digitalRead(PIN_IR_SENSOR_RR);

    float pid_output, error, proportional, derivative;

    if (!ir_ll && !ir_l && !ir_c && !ir_r && !ir_rr) {
        if (state_machine->last_ir_ll) {
            control_motors(-TAPE_FOLLOWING_RECOVERY_SPEED);
        } else if (state_machine->last_ir_rr) {
            control_motors(+TAPE_FOLLOWING_RECOVERY_SPEED);
        } else {
            // go straight to run over debris
            control_motors(0);
        }
    } else {
        error = calculate_error(state_machine->last_error, ir_ll, ir_l, ir_c, ir_r, ir_rr);
        state_machine->last_error = error;
        float delta_time_s = (now - state_machine->last_pid_time) / 1000.0f;
        
        proportional = KP * error;
        derivative = KD * (error - state_machine->last_error) / delta_time_s;

        pid_output = proportional + derivative; // output is not constrained, but the speed of each motor is
        control_motors(pid_output);

        #ifdef DEBUG
        Serial.printf("IR: %d %d %d %d %d | output = %f | dt = %f\n", ir_ll, ir_l, ir_c, ir_r, ir_rr, pid_output, delta_time_s);
        #endif

        state_machine->last_ir_ll = ir_ll;
        state_machine->last_ir_l  = ir_l;
        state_machine->last_ir_c  = ir_c;
        state_machine->last_ir_r  = ir_r;
        state_machine->last_ir_rr = ir_rr;
    }
    
    state_machine->last_pid_time = now;
}

void state_tape_following_enter(struct state_machine *state_machine, state_event_e event) {
    state_tape_following_run(state_machine);
}

void state_tape_following_exit(struct state_machine *state_machine) {
    // turn off motors
    state_machine->last_pid_time = 0;
    left_motor.stop();
    right_motor.stop();
}

// positive output -> turning left (since the right motor gets more power)
void control_motors(float pid_output) {
    int left_speed = TAPE_FOLLOWING_BASE_SPEED - pid_output;
    int right_speed = TAPE_FOLLOWING_BASE_SPEED + pid_output;

    left_speed = constrain(left_speed, -TAPE_FOLLOWING_MAX_SPEED, TAPE_FOLLOWING_MAX_SPEED);
    right_speed = constrain(right_speed, -TAPE_FOLLOWING_MAX_SPEED, TAPE_FOLLOWING_MAX_SPEED);

    left_motor.write(abs(left_speed), left_speed >= 0 ? LEFT_MOTOR_FORWARD : LEFT_MOTOR_BACKWARD);
    right_motor.write(abs(right_speed), right_speed >= 0 ? RIGHT_MOTOR_FORWARD : RIGHT_MOTOR_BACKWARD);

}

float calculate_error(float last_error, bool ll, bool l, bool c, bool r, bool rr) {
    float total_weight = 0;
    float weighted_sum = 0;

    if (ll) { total_weight += 0.5; weighted_sum += -4; }
    if (l)  { total_weight += 1.0; weighted_sum += -2; }
    if (c)  { total_weight += 1.5; weighted_sum += 0;  }
    if (r)  { total_weight += 1.0; weighted_sum += 2;  }
    if (rr) { total_weight += 0.5; weighted_sum += 4;  }

    if (total_weight == 0) return last_error; // No line detected, maintain last position
    return weighted_sum / total_weight;
}
