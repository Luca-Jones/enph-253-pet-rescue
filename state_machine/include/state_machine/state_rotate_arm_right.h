#ifndef STATE_ROTATE_ARM_RIGHT_H
#define STATE_ROTATE_ARM_RIGHT_H

// TODO: replace rotate and raise with enter procedures??

#include "state_machine.h"

void state_rotate_arm_right_init(struct state_machine *state_machine);
void state_rotate_arm_right_enter(struct state_machine *state_machine, state_e from);
void state_rotate_arm_right_exit(struct state_machine *state_machine);

#endif