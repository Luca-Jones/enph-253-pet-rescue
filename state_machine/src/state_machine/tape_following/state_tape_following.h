#ifndef STATE_TAPE_FOLLOWING_H
#define STATE_TAPE_FOLLOWING_H

#include "../state_machine.h"

struct state_tape_following_data {
    const struct state_machine *state_machine;
    float error;
    float last_error;
    float pid_output;
    unsigned long last_time;
};

void state_tape_following_init(struct state_tape_following_data *data);
void state_tape_following_enter(struct state_tape_following_data *data, state_e from, state_event_e event);
void state_tape_following_exit(struct state_tape_following_data *data, state_e to, state_event_e event);

#endif