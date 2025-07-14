#include "state_tape_following.h"

static void state_tape_following_run(struct state_tape_following_data *data) {
    // TODO: implement tape following functionality
}

void state_search_init(struct state_tape_following_data *data) {
    // TODO: initialize tape following data
}

void state_tape_following_enter(struct state_tape_following_data *data, state_e from, state_event_e event) {
    switch (from) {
    case STATE_WAIT:
        break;
    case STATE_TAPE_FOLLOWING:
        // do nothing
        break;
    }
    state_tape_following_run(data);
}