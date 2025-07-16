#ifndef STATE_REACH_H
#define STATE_REACH_H

#include "../state_machine.h"

struct state_reach_data {
    const struct state_machine *state_machine;
    // TODO: data for this state
};

void state_reach_init(struct state_reach_data *data);
void state_reach_enter(struct state_reach_data *data, state_e from, state_event_e event);
void state_reach_exit(struct state_reach_data *data, state_e to, state_event_e event);

#endif