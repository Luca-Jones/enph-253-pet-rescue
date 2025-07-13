#include "state_tape_following.h"
#include "state_machine.h"
#include "state_common.h"

static void state_tape_following_run(struct state_tape_following_data *data) {
    // do tape following stuff?
}

void state_tape_following_enter(struct state_tape_following_data *data, state_e from, state_event_e event) {
    switch (from) {
    case STATE_WAIT:
        state_tape_following_run(data);
        break;
    }
}

void state_search_init(struct state_tape_following_data *data) {
    // init stuff
}