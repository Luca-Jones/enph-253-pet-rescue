#include "state_reverse.h"\

/* Goes for a set time interval? */

static void state_reverse_run(struct state_reverse_data *data) {
    // TODO: implement functionality
}

void state_reverse_init(struct state_reverse_data *data) {
    // TODO: intialize data
}

void state_reverse_enter(struct state_reverse_data *data, state_e from, state_event_e event) {
    switch (from) {
        case STATE_REVERSE:
            // do nothing
            break;
        default:
            print_event(event);
            print_state(STATE_REVERSE);
            break;
    }
    state_reverse_run(data);
}

void state_reverse_exit(struct state_reverse_data *data, state_e to, state_event_e event) {

}
