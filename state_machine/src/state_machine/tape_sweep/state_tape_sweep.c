#include "state_tape_sweep.h"

static void state_tape_sweep_run(struct state_tape_sweep_data *data) {
    // TODO: implement functionality
}

void state_tape_sweep_init(struct state_tape_sweep_data *data) {
    // TODO: intialize data
}

void state_tape_sweep_enter(struct state_tape_sweep_data *data, state_e from, state_event_e event) {
    switch (from) {
        case STATE_WAIT:
            break;
        case STATE_TAPE_FOLLOWING:
            break;
        case STATE_REACH:
            break;
    }
    state_tape_sweep_run(data);
}