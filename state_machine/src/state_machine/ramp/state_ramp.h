#ifndef STATE_RAMP_H
#define STATE_RAMP_H

#include "../state_machine.h"

struct state_ramp_data {
    const struct state_machine *state_machine;
    // TODO: data for this state
};

void state_ramp_init(struct state_ramp_data *data);
void state_ramp_enter(struct state_ramp_data *data, state_e from, state_event_e event);
void state_ramp_exit(struct state_ramp_data *data, state_e to, state_event_e event);

#endif