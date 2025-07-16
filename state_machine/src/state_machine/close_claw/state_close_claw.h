#ifndef STATE_CLOSE_CLAW_H
#define STATE_CLOSE_CLAW_H

#include "../state_machine.h"

struct state_close_claw_data {
    const struct state_machine *state_machine;
    // TODO: data for this state
};

void state_close_claw_init(struct state_close_claw_data *data);
void state_close_claw_enter(struct state_close_claw_data *data, state_e from, state_event_e event);

#endif