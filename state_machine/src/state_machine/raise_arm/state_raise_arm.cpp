#include "state_raise_arm.h"

static void state_raise_arm_run(struct state_machine *state_machine) {
    // TODO: implement functionality
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
    
}
