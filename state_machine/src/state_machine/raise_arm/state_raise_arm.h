#ifndef STATE_RAISE_ARM_H
#define STATE_RAISE_ARM_H

#define AM_I_HERE 2
#include "../state_machine.h"

struct state_raise_arm_data {
    const struct state_machine *state_machine;
    // TODO: data for this state
};

void state_raise_arm_init(struct state_raise_arm_data *data);
void state_raise_arm_enter(struct state_raise_arm_data *data, state_e from, state_event_e event);
void state_raise_arm_exit(struct state_raise_arm_data *data, state_e from, state_event_e event);

#endif