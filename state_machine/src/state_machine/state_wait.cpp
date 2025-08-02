#include <state_machine/state_wait.h>
#include "esp32-hal.h"

static void state_wait_run(struct state_machine *state_machine) {
    delay(5000);
    state_machine->internal_event = EVENT_PET_GRASPED;
}

void state_wait_enter(struct state_machine *state_machine) {
    state_wait_run(state_machine);
}
