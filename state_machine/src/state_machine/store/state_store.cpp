#include "state_store.h"

// single-iteration event

static void state_store_run(struct state_store_data *data) {
    // TODO: implement functionality
}

void state_store_init(struct state_store_data *data) {
    // TODO: intialize data
}

void state_store_enter(struct state_store_data *data, state_e from, state_event_e event) {
    switch (from) {
        case STATE_STORE:
            // do nothing
            break;
        default:
            print_event(event);
            print_state(STATE_STORE);
            break;
    }
    state_store_run(data);
}

void state_store_exit(struct state_store_data *data, state_e to, state_event_e event) {

}
