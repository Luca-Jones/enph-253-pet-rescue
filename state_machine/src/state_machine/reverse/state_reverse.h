#ifndef STATE_REVERSE_H
#define STATE_REVERSE_H

#include "../state_machine.h"

struct state_reverse_data {
    const struct state_machine *state_machine;
    // TODO: data for this state
};

void state_reverse_init(struct state_reverse_data *data);
void state_reverse_enter(struct state_reverse_data *data, state_e from, state_event_e event);
void state_reverse_exit(struct state_reverse_data *data, state_e to, state_event_e event);

#endif