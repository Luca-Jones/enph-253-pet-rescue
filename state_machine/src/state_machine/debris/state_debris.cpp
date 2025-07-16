#include "state_debris.h"

static void state_debris_run(struct state_debris_data *data) {
    // TODO: implement functionality
}

void state_debris_init(struct state_debris_data *data) {
    // TODO: intialize data
}

void state_debris_enter(struct state_debris_data *data, state_e from, state_event_e event) {
    switch (from) {
        case STATE_WAIT:
            break;
        case STATE_TAPE_FOLLOWING:
            break;
        case STATE_REACH:
            break;
    }
    state_debris_run(data);
}