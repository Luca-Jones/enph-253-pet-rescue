#include <state_machine/state_reach.h>
#include <actuators/Arm.h>

#define REACH_STEP 10

static void state_reach_run(struct state_machine *state_machine) {
    int x,y;
    arm.get_pos(&x, &y);
    arm.move_to_pos(x + REACH_STEP, y);
}

void state_reach_enter(struct state_machine *state_machine, state_event_e event) {
    switch (event) {
        case EVENT_NONE:
        case EVENT_PET_DETECTED_LEFT:
            // reach
            state_machine->claw_open_angle = CLAW_OPEN;
            break;
        case EVENT_PET_DETECTED_RIGHT:
            // turn right    
            state_machine->claw_open_angle = CLAW_OPEN;
            if (state_machine->arm_in_start_pos) {
                arm.lerp_to_pos(ARM_RAISED_X, ARM_RAISED_Y, 500);
                state_machine->arm_in_start_pos = false;
            }
            base_gear.write(BASE_GEAR_RIGHT);
            arm.lerp_to_pos(ARM_HOME_X, ARM_HOME_Y, 500);
            break;
        case EVENT_PILLAR_DETECTED:
            // raise
            state_machine->claw_open_angle = CLAW_SEMI_OPEN;
            arm.lerp_to_pos(ARM_PILLAR_X, ARM_PILLAR_Y, 500);
            break;
        default:
            break;
    }
    state_reach_run(state_machine);
}
