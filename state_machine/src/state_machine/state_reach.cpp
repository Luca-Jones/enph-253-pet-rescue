#include <state_machine/state_reach.h>
#include <actuators/Arm.h>

#define REACH_STEP 30

static void state_reach_run(struct state_machine *state_machine) {
    int x,y;
    arm.get_pos(&x, &y);
    
    if (arm.move_to_pos(x + REACH_STEP, y) == ARM_ILLEGAL_POS) {
        // state_machine->internal_event = EVENT_PET_MISSED;
    }
}

void state_reach_enter(struct state_machine *state_machine, state_event_e event) {

    if (state_machine->arm_in_start_pos) {
        arm.lerp_to_pos(ARM_RAISED_X, ARM_RAISED_Y, 500);
        state_machine->arm_in_start_pos = false;
        delay(1000);
        base_gear.write(BASE_GEAR_HOME);
        arm.lerp_to_pos(ARM_HOME_X, ARM_HOME_Y, 1000);
    }

    if (eTaskGetState(tof_task_handle) != eSuspended) {
        if (xSemaphoreTake(i2c_mutex, portMAX_DELAY)) {
            vTaskSuspend(tof_task_handle);
            xSemaphoreGive(i2c_mutex);
        }
    }
    
    if (eTaskGetState(dist_task_handle) == eSuspended) {    
        vTaskResume(dist_task_handle);
    }

    switch (event) {
        case EVENT_NONE:
            break;
        case EVENT_PET_DETECTED_LEFT:
            // just reach
            state_machine->claw_open_angle = CLAW_OPEN;
            claw.write(CLAW_OPEN);
            delay(500);
            arm.lerp_to_pos(ARM_REACH_X, ARM_REACH_Y, 500);
            break;
        case EVENT_PET_DETECTED_RIGHT:
            // turn right first  
            state_machine->claw_open_angle = CLAW_OPEN;
            claw.write(CLAW_OPEN);
            delay(500);
            base_gear.write(BASE_GEAR_RIGHT);
            arm.lerp_to_pos(ARM_REACH_X, ARM_REACH_Y, 500);
            break;
            case EVENT_PILLAR_DETECTED:
            // raise up first (pillars are always on the left)
            state_machine->claw_open_angle = CLAW_SEMI_OPEN;
            claw.write(CLAW_SEMI_OPEN);
            delay(500);
            arm.lerp_to_pos(ARM_PILLAR_X, ARM_PILLAR_Y, 500);
            break;
        default:
            break;
    }
    state_reach_run(state_machine);
}
