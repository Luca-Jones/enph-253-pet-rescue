#ifndef STATE_COMMON_H
#define STATE_COMMON_H

// common data for all of the states

typedef enum {
    STATE_WAIT,
    STATE_TAPE_FOLLOWING,
} state_e;

typedef enum {
    STATE_EVENT_NONE,
    STATE_EVENT_TAPE_SPOTTED,
} state_event_e;

struct state_common_data {
    struct state_machine_data *state_machine_data;
    // any data that all states need...
};

void state_machine_post_internal_event(struct state_machine_data *data, state_event_e event);

#endif