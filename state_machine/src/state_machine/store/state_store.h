#ifndef STATE_STORE_H
#define STATE_STORE_H

#include "../state_machine.h"

void state_store_init(struct state_machine *state_machine);
void state_store_enter(struct state_machine *state_machine, state_e from);
void state_store_exit(struct state_machine *state_machine);

#endif