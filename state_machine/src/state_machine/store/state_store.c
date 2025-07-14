#include "state_store.h"

static void state_store_run(struct state_store_data *data) {
    // TODO: implement functionality
}

void state_store_init(struct state_store_data *data) {
    // TODO: intialize data
}

void state_store_enter(struct state_store_data *data, state_e from, state_event_e event) {
    switch (from) {
        case STATE_WAIT:
            break;
        case STATE_TAPE_FOLLOWING:
            break;
        case STATE_REACH:
            break;
    }
    state_store_run(data);
}