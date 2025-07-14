#ifndef STATE_WAIT_H
#define STATE_WAIT_H

#include "../state_machine.h"

struct state_raise_arm_data {
    const struct state_machine *state_machine;
    // TODO: data for this state
};

void state_raise_arm_init(struct state_raise_arm_data *data);
void state_raise_arm_enter(struct state_raise_arm_data *data, state_e from, state_event_e event);

#endif