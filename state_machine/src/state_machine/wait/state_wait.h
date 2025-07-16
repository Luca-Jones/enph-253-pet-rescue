#ifndef STATE_WAIT_H
#define STATE_WAIT_H

#include "../state_machine.h"

struct state_wait_data {
    const struct state_machine *state_machine;
};

void state_wait_init(struct state_wait_data *data);
void state_wait_enter(struct state_wait_data *data, state_e from, state_event_e event);
void state_wait_exit(struct state_wait_data *data, state_e to, state_event_e event);

#endif