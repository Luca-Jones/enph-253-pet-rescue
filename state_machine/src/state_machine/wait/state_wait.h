#ifndef STATE_WAIT_H
#define STATE_WAIT_H

#include "../state_machine.h"
#include "../states.h"

void state_wait_init(struct state_machine *state_machine);
void state_wait_enter(struct state_machine *state_machine, state_e from);
void state_wait_exit(struct state_machine *state_machine);

#endif