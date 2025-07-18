#include "state_raise_arm.h"

#define RAISE_STEP 10

static void state_raise_arm_run(struct state_machine *state_machine) {
    int x,y;
    arm.get_pos(&x, &y);
    arm.move_to_pos(x, y + RAISE_STEP);
}

void state_raise_arm_enter(struct state_machine *state_machine, state_e from) {
    switch (from) {
        case STATE_RAISE_ARM:
            // do nothing
            break;
        default:
            break;
    }
    state_raise_arm_run(state_machine);
}

void state_raise_arm_exit(struct state_machine *state_machine) {
    // TODO: remove?
}
