#include "state_reach.h"

#define REACH_STEP 10

static void state_reach_run(struct state_machine *state_machine) {
    int x,y;
    arm.get_pos(&x, &y);
    arm.move_to_pos(x + REACH_STEP, y);
}

void state_reach_enter(struct state_machine *state_machine, state_e from) {
    switch (from) {
        case STATE_REACH:
            // do nothing
            break;
        default:
            break;
    }
    state_reach_run(state_machine);
}

void state_reach_exit(struct state_machine *state_machine) {
    // TODO: remove?
}
