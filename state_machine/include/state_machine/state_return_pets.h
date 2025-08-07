#ifndef STATE_RETURN_PETS_H
#define STATE_RETURN_PETS_H

#include "state_machine.h"

#define CASCADE_LIFT_TIME_MS        5500
#define CASCADE_LOWER_TIME_MS       5200

void state_return_pets_enter(struct state_machine *state_machine);

#endif