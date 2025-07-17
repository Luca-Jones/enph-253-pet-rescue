#ifndef STATE_REACH_H
#define STATE_REACH_H

#include "../state_machine.h"
#include "../states.h"

void state_reach_init(struct state_machine *state_machine);
void state_reach_enter(struct state_machine *state_machine, state_e from);
void state_reach_exit(struct state_machine *state_machine);

#endif