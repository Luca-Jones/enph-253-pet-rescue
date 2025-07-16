#include "state_extend_cascade.h"

static void state_extend_cascade_run(struct state_extend_cascade_data *data) {
    // TODO: implement functionality
}

void state_extend_cascade_init(struct state_extend_cascade_data *data) {
    // TODO: intialize data
}

void state_extend_cascade_enter(struct state_extend_cascade_data *data, state_e from, state_event_e event) {
    switch (from) {
        case STATE_WAIT:
            break;
        case STATE_TAPE_FOLLOWING:
            break;
        case STATE_REACH:
            break;
    }
    state_extend_cascade_run(data);
}