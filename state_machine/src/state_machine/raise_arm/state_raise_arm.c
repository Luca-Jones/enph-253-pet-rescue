#include "state_raise_arm.h"

static void state_raise_arm_run(struct state_raise_arm_data *data) {
    // TODO: implement functionality
}

void state_raise_arm_init(struct state_raise_arm_data *data) {
    // TODO: intialize data
}

void state_raise_arm_enter(struct state_raise_arm_data *data, state_e from, state_event_e event) {
    switch (from) {
        case STATE_WAIT:
            break;
        case STATE_TAPE_FOLLOWING:
            break;
        case STATE_REACH:
            break;
        case STATE_RAISE_ARM:
            // do nothing
            break;
    }
    state_raise_arm_run(data);
}