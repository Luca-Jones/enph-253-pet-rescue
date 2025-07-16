#include "state_reverse.h"

static void state_reverse_run(struct state_reverse_data *data) {
    // TODO: implement functionality
}

void state_reverse_init(struct state_reverse_data *data) {
    // TODO: intialize data
}

void state_reverse_enter(struct state_reverse_data *data, state_e from, state_event_e event) {
    switch (from) {
        case STATE_WAIT:
            break;
        case STATE_TAPE_FOLLOWING:
            break;
        case STATE_REACH:
            break;
    }
    state_reverse_run(data);
}