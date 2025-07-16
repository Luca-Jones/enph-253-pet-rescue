#include "state_ramp.h"

/* We could do a lot of things with the ramp, like speed up or stop to drop off a pet (another state?) */

static void state_ramp_run(struct state_ramp_data *data) {
    // TODO: implement functionality
}

void state_ramp_init(struct state_ramp_data *data) {
    // TODO: intialize data
}

void state_ramp_enter(struct state_ramp_data *data, state_e from, state_event_e event) {
    switch (from) {
        case STATE_RAMP:
            // do nothing
            break;
        default:
            display_handler.clearDisplay();
            display_handler.setCursor(0, 0);
            display_handler.println("RAMP");
            display_handler.display();
            break;
    }
    state_ramp_run(data);
}

void state_ramp_exit(struct state_ramp_data *data, state_e to, state_event_e event) {
    
}
