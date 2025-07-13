#ifndef STATE_TAPE_FOLLOWING_H
#define STATE_TAPE_FOLLOWING_H

// what does this state do?

struct state_tape_following_data {
    // data for this state
    const struct state_common_data *data;
};

void state_tape_following_init(struct state_tape_following_data *data);
void state_tape_following_enter(struct state_tape_following_data *data, state_e from, state_event_e event);

#endif