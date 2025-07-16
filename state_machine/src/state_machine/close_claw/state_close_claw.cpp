#include "state_close_claw.h"

static void state_close_claw_run(struct state_close_claw_data *data) {
    // TODO: implement functionality
}

void state_close_claw_init(struct state_close_claw_data *data) {
    // TODO: intialize data
}

void state_close_claw_enter(struct state_close_claw_data *data, state_e from, state_event_e event) {
    switch (from) {
        case STATE_CLOSE_CLAW:
            // do nothing
            break;
        default:
            display_handler.clearDisplay();
            display_handler.setCursor(0, 0);
            display_handler.println("CLOSE CLAW");
            display_handler.display();
            break;
    }
    state_close_claw_run(data);
}

void state_close_claw_exit(struct state_close_claw_data *data, state_e to, state_event_e event) {
    
}
