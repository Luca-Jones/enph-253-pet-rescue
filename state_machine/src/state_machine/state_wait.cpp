#include <state_machine/state_wait.h>
#include "esp32-hal.h"

static void state_wait_run(struct state_machine *state_machine) {
    delay(500);
}

void state_wait_enter(struct state_machine *state_machine) {
    state_wait_run(state_machine);
}
