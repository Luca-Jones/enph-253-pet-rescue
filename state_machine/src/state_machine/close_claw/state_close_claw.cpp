#include "state_close_claw.h"

static void state_close_claw_run(struct state_close_claw_data *data) {
    // TODO: implement functionality
}

void state_close_claw_init(struct state_close_claw_data *data) {
    // TODO: intialize data
}

void state_close_claw_enter(struct state_close_claw_data *data, state_e from, state_event_e event) {
    switch (from) {
        case STATE_WAIT:
            break;
        case STATE_TAPE_FOLLOWING:
            break;
        case STATE_REACH:
            break;
    }
    state_close_claw_run(data);
}