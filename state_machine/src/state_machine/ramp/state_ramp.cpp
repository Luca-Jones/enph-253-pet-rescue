#include "state_ramp.h"

static void state_ramp_run(struct state_ramp_data *data) {
    // TODO: implement functionality
}

void state_ramp_init(struct state_ramp_data *data) {
    // TODO: intialize data
}

void state_ramp_enter(struct state_ramp_data *data, state_e from, state_event_e event) {
    switch (from) {
        case STATE_WAIT:
            break;
        case STATE_TAPE_FOLLOWING:
            break;
        case STATE_REACH:
            break;
    }
    state_ramp_run(data);
}