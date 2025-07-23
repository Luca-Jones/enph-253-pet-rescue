#include <state_machine/state_close_claw.h>
#include <actuators/Claw.h>

#define CLAW_WAIT_TIME_MS 1000

static void state_close_claw_run(struct state_machine *state_machine) {
    claw.write(CLAW_CLOSED);
    delay(CLAW_WAIT_TIME_MS); // give enough time for the limit switch to trigger
    state_machine->claw_closed = true;
}

void state_close_claw_enter(struct state_machine *state_machine) {
    state_close_claw_run(state_machine);
}
