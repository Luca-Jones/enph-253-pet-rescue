#ifndef STATE_TAPE_SWEEP_H
#define STATE_TAPE_SWEEP_H

#include "../state_machine.h"

struct state_tape_sweep_data {
    const struct state_machine *state_machine;
    // TODO: data for this state
};

void state_tape_sweep_init(struct state_tape_sweep_data *data);
void state_tape_sweep_enter(struct state_tape_sweep_data *data, state_e from, state_event_e event);

#endif