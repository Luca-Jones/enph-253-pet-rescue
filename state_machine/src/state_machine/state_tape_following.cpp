#include <state_machine/state_tape_following.h>
#include <esp32-hal.h>
#include <config/dir_config.h>

#define TAPE_FOLLOWING_BASE_SPEED       50
#define TAPE_FOLLOWING_MAX_SPEED        255
#define TAPE_FOLLOWING_RECOVERY_SPEED   40

#define KP 18
#define KD 0

#define TAPE_FOLLOWING_PERIOD 30

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
            left_motor.write(TAPE_FOLLOWING_BASE_SPEED, LEFT_MOTOR_FORWARD);
            right_motor.write(TAPE_FOLLOWING_BASE_SPEED, RIGHT_MOTOR_FORWARD);
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
        // Serial.printf("IR: %d %d %d %d %d | output = %f | dt = %f\n", ir_ll, ir_l, ir_c, ir_r, ir_rr, pid_output, delta_time_s);
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
    
    // if (dist_task_handle != NULL && eTaskGetState(dist_task_handle) != eSuspended) {
    //     if (i2c_mutex != NULL && xSemaphoreTake(i2c_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
    //         // Task is not using I2C right now (mutex is available)
    //         vTaskSuspend(dist_task_handle);  // Now safe to suspend
    //         #ifdef DEBUG
    //         Serial.println("dist task suspended");
    //         #endif
    //         xSemaphoreGive(i2c_mutex);
    //     } else {
    //         // Mutex timeout - force suspend anyway (risky but prevents deadlock)
    //         #ifdef DEBUG
    //         Serial.println("Force suspending dist task - mutex timeout");
    //         #endif
    //         vTaskSuspend(dist_task_handle);
    //     }
    // }

    if (tof_task_handle != NULL && eTaskGetState(tof_task_handle) == eSuspended) {
        vTaskResume(tof_task_handle);
        #ifdef DEBUG
        Serial.println("tof task resumed");
        #endif
        delay(100);
    }

    state_tape_following_run(state_machine);
}

void state_tape_following_exit(struct state_machine *state_machine) {
    #ifdef DEBUG
    Serial.println("backing up...");
    #endif

    left_motor.write(150, LEFT_MOTOR_BACKWARD);
    right_motor.write(150, RIGHT_MOTOR_BACKWARD);
    delay(100);
    left_motor.stop();
    right_motor.stop();

    if (tof_task_handle != NULL && eTaskGetState(tof_task_handle) != eSuspended) {
        if (i2c_mutex != NULL && xSemaphoreTake(i2c_mutex, pdMS_TO_TICKS(500)) == pdTRUE) {
            // Task is not using I2C right now (mutex is available)
            vTaskSuspend(tof_task_handle);  // Now safe to suspend
            #ifdef DEBUG
            Serial.println("tof task suspended");
            #endif
            xSemaphoreGive(i2c_mutex);
        } else {
            // Mutex timeout - force suspend anyway (risky but prevents deadlock)
            #ifdef DEBUG
            Serial.println("Force suspending tof task - mutex timeout");
            #endif
            vTaskSuspend(dist_task_handle);
        }
    }
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

    if (ll) { total_weight += 1.0; weighted_sum += -2; }
    if (l)  { total_weight += 1.0; weighted_sum += -1; }
    if (c)  { total_weight += 1.0; weighted_sum += 0;  }
    if (r)  { total_weight += 1.0; weighted_sum += 1;  }
    if (rr) { total_weight += 1.0; weighted_sum += 2;  }

    if (total_weight == 0) return last_error; // No line detected, maintain last position
    return weighted_sum / total_weight;
}
