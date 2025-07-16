#include "state_tape_following.h"

static void state_tape_following_run(struct state_tape_following_data *data) {
    // TODO: implement tape following functionality
}

void state_tape_following_init(struct state_tape_following_data *data) {
    data->error = 0;
    data->last_error = 0;
    data->pid_output = 0;
    data->last_time = 0;
}

void state_tape_following_enter(struct state_tape_following_data *data, state_e from, state_event_e event) {
    // TODO: switch on event to make debugging easier?
    switch (from) {
    case STATE_TAPE_FOLLOWING:
        // do nothing
        break;
    case STATE_WAIT:
    case STATE_STORE:
    case STATE_TAPE_SWEEP:
    default:
        display_handler.clearDisplay();
        display_handler.setCursor(0, 0);
        display_handler.println("TAPE FOLLOWING");
        display_handler.display();
        break;
    }
    state_tape_following_run(data);
}

void state_tape_following_exit(struct state_tape_following_data *data, state_e to, state_event_e event) {
    switch (to) {
        case STATE_REACH:
        case STATE_RAISE_ARM:
        case STATE_EXTEND_CASCADE:
            // TODO: turn off motors
            break;
        case STATE_RAMP:
        case STATE_DEBRIS:
            // TODO: decide if we should just keep line following or turn off IR
            break;
    }
}
