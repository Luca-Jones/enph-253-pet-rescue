#include "state_close_claw.h"

static void state_close_claw_run(struct state_machine *state_machine) {
    claw.close();
    delay(500);
    state_machine->internal_event = STATE_EVENT_PET_GRASPED;
}

void state_close_claw_enter(struct state_machine *state_machine, state_e from) {
    switch (from) {
        case STATE_CLOSE_CLAW:
            // do nothing
            break;
        default:
            break;
    }
    state_close_claw_run(state_machine);
}

void state_close_claw_exit(struct state_machine *state_machine) {
    // reset pet check
}
