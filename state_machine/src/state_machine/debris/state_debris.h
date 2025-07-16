#ifndef STATE_DEBRIS_H
#define STATE_DEBRIS_H

#include "../state_machine.h"

struct state_debris_data {
    const struct state_machine *state_machine;
    // TODO: data for this state
};

void state_debris_init(struct state_debris_data *data);
void state_debris_enter(struct state_debris_data *data, state_e from, state_event_e event);
void state_debris_exit(struct state_debris_data *data, state_e to, state_event_e event);

#endif