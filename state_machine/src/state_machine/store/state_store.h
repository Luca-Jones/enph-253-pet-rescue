#ifndef STATE_STORE_H
#define STATE_STORE_H

#include "../state_machine.h"

struct state_store_data {
    const struct state_machine *state_machine;
    // TODO: data for this state
};

void state_store_init(struct state_store_data *data);
void state_store_enter(struct state_store_data *data, state_e from, state_event_e event);
void state_store_exit(struct state_store_data *data, state_e to, state_event_e event);

#endif