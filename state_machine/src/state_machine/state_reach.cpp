#include <state_machine/state_reach.h>
#include <actuators/Arm.h>

#define REACH_STEP 10

// TOOD: implement this state

static void state_reach_run(struct state_machine *state_machine) {
    int x,y;
    arm.get_pos(&x, &y);
    arm.move_to_pos(x + REACH_STEP, y);
}

void state_reach_enter(struct state_machine *state_machine, state_event_e event) {
    switch (event) {
        case EVENT_NONE:
            // do nothing
            break;
        case EVENT_PET_DETECTED_LEFT:
            // reach
            break;
        case EVENT_PET_DETECTED_RIGHT:
            // turn right
            break;
        case EVENT_PILLAR_DETECTED:
            // raise
            break;
        default:
            break;
            //         int x,y;
            // arm.get_pos(&x, &y);
            // arm.lerp_to_pos(x, ARM_PILLAR_Y, 500);
    }
    state_reach_run(state_machine);
}
