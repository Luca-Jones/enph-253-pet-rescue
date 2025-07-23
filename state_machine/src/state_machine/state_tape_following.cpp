#include <state_machine/state_tape_following.h>
#include <esp32-hal.h>

#define TAPE_FOLLOWING_BASE_SPEED       100
#define TAPE_FOLLOWING_MAX_SPEED        200
#define TAPE_FOLLOWING_RECOVERY_SPEED   60

#define INTEGRAL_DECAY  0.95
#define MAX_INTEGRAL    100

#define KP 25
#define KI 0 // why bother with integral?
#define KD 5

#define WIFI_PID_TUNING

/* Helper functions */
void control_motors(float pid_output);
float calculate_error(float last_error, bool ll, bool l, bool c, bool r, bool rr);
void recovery_spin();

void state_tape_following_run(struct state_machine *state_machine) {

    unsigned long now = millis();
    if (state_machine->last_pid_time == 0) {
        state_machine->last_pid_time = now;
    }

    bool ir_ll = digitalRead(PIN_IR_SENSOR_LL);
    bool ir_l  = digitalRead(PIN_IR_SENSOR_L);
    bool ir_c  = digitalRead(PIN_IR_SENSOR_C);
    bool ir_r  = digitalRead(PIN_IR_SENSOR_R);
    bool ir_rr = digitalRead(PIN_IR_SENSOR_RR);

    float pid_output, error, proportional, integral, derivative;

    if (!ir_ll && !ir_l && !ir_c && !ir_r && !ir_rr) {
        if (state_machine->last_ir_ll) {
            control_motors(TAPE_FOLLOWING_RECOVERY_SPEED);
        } else if (state_machine->last_ir_rr) {
            control_motors(-TAPE_FOLLOWING_RECOVERY_SPEED);
        } else {
            // go straight to run over debris
            control_motors(0);
        }
    } else {
        error = calculate_error(state_machine->last_error, ir_ll, ir_l, ir_c, ir_r, ir_rr);
        float delta_time_s = (now - state_machine->last_pid_time) / 1000.0f;
        
        proportional = KP * error;
        integral = integral * INTEGRAL_DECAY + error * delta_time_s;
        integral = constrain(integral, -MAX_INTEGRAL, MAX_INTEGRAL);
        integral = KI * integral;
        derivative = KD * (state_machine->last_error - error) / delta_time_s;

        pid_output = proportional + integral + derivative;
        control_motors(pid_output);

        state_machine->last_error = error;

    }
    
    state_machine->last_pid_time = now;
    state_machine->last_ir_ll = ir_ll;
    state_machine->last_ir_l  = ir_l;
    state_machine->last_ir_c  = ir_c;
    state_machine->last_ir_r  = ir_r;
    state_machine->last_ir_rr = ir_rr;
}

void state_tape_following_enter(struct state_machine *state_machine, state_event_e event) {
    state_tape_following_run(state_machine);
}

void state_tape_following_exit(struct state_machine *state_machine) {
    // turn off motors
    ledcWrite(PWM_CHANNEL_MOTOR_LEFT, 0);
    ledcWrite(PWM_CHANNEL_MOTOR_RIGHT, 0);
}

void control_motors(float pid_output) {
    int left_speed = TAPE_FOLLOWING_BASE_SPEED - pid_output;
    int right_speed = TAPE_FOLLOWING_BASE_SPEED + pid_output;

    left_speed = constrain(left_speed, -TAPE_FOLLOWING_MAX_SPEED, TAPE_FOLLOWING_MAX_SPEED);
    right_speed = constrain(right_speed, -TAPE_FOLLOWING_MAX_SPEED, TAPE_FOLLOWING_MAX_SPEED);

    // Stop motors first
    ledcWrite(PWM_CHANNEL_MOTOR_LEFT, 0);
    ledcWrite(PWM_CHANNEL_MOTOR_RIGHT, 0);
    delayMicroseconds(100);

    // Set directions
    digitalWrite(PIN_MOTOR_LEFT_DIR, left_speed >= 0 ? HIGH : LOW);
    digitalWrite(PIN_MOTOR_RIGHT_DIR, right_speed >= 0 ? HIGH : LOW);
    delayMicroseconds(100);

    // Apply PWM values
    ledcWrite(PWM_CHANNEL_MOTOR_LEFT, abs(left_speed));
    ledcWrite(PWM_CHANNEL_MOTOR_RIGHT, abs(right_speed));
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

void recovery_spin() {
    // Spin right to find line
    digitalWrite(PIN_MOTOR_LEFT_DIR, HIGH);
    digitalWrite(PIN_MOTOR_RIGHT_DIR, LOW);
    ledcWrite(PWM_CHANNEL_MOTOR_LEFT, TAPE_FOLLOWING_RECOVERY_SPEED);
    ledcWrite(PWM_CHANNEL_MOTOR_RIGHT, TAPE_FOLLOWING_RECOVERY_SPEED);
}
