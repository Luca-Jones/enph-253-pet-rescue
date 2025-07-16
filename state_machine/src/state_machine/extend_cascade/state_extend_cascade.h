#ifndef STATE_EXTEND_CASCADE_H
#define STATE_EXTEND_CASCADE_H

#include "../state_machine.h"

struct state_extend_cascade_data {
    const struct state_machine *state_machine;
    // TODO: data for this state
};

void state_extend_cascade_init(struct state_extend_cascade_data *data);
void state_extend_cascade_enter(struct state_extend_cascade_data *data, state_e from, state_event_e event);
void state_extend_cascade_exit(struct state_extend_cascade_data *data, state_e to, state_event_e event);

#endif