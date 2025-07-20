#ifndef STATE_RETURN_PETS_H
#define STATE_RETURN_PETS_H

#include "state_machine.h"

void state_return_pets_init(struct state_machine *state_machine);
void state_return_pets_enter(struct state_machine *state_machine, state_e from);
void state_return_pets_exit(struct state_machine *state_machine);

#endif