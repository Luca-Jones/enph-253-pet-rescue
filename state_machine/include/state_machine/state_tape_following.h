#ifndef STATE_TAPE_FOLLOWING_H
#define STATE_TAPE_FOLLOWING_H

#include "state_machine.h"

void state_tape_following_enter(struct state_machine *state_machine, state_event_e event);
void state_tape_following_exit(struct state_machine *state_machine);

#endif