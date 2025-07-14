#include "state_wait.h"

static void state_wait_run(struct state_wait_data *data) {
    // TODO: implement waiting functionality
}

void state_wait_init(struct state_wait_data *data) {
    // TODO: intialize wait data
}

void state_wait_enter(struct state_wait_data *data, state_e from, state_event_e event) {
    switch (from) {
        case STATE_WAIT:
            // do nothing
            break;
        case STATE_TAPE_FOLLOWING:
            break;
    }
    state_wait_run(data);
}