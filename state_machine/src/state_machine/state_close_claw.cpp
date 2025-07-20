#include <state_machine/state_close_claw.h>
#include <actuators/Claw.h>

// TODO: implement this state

static void state_close_claw_run(struct state_machine *state_machine) {
    claw.write(CLAW_CLOSED);
    delay(500);
    state_machine->internal_event = EVENT_PET_GRASPED;
}

void state_close_claw_enter(struct state_machine *state_machine) {
    state_close_claw_run(state_machine);
}

void state_close_claw_exit(struct state_machine *state_machine) {
    // reset pet check
}
