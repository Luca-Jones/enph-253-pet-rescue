#ifndef STATE_WAIT_H
#define STATE_WAIT_H

#include "state_common.h"

struct state_wait_data {
    const struct state_common_data *data;
};

void state_wait_enter(struct state_wait_data *data, state_e from, state_event_e event);

#endif