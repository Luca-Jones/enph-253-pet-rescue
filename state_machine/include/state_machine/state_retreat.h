#ifndef STATE_RETREATE_H
#define STATE_RETREATE_H

#include "state_machine.h"

void state_retreat_init(struct state_machine *state_machine);
void state_retreat_enter(struct state_machine *state_machine, state_e from);
void state_retreat_exit(struct state_machine *state_machine);

#endif