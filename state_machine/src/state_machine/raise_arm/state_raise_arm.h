#ifndef STATE_RAISE_ARM_H
#define STATE_RAISE_ARM_H

#include "../state_machine.h"
#include "../states.h"

void state_raise_arm_init(struct state_machine *state_machine);
void state_raise_arm_enter(struct state_machine *state_machine, state_e from);
void state_raise_arm_exit(struct state_machine *state_machine);

#endif