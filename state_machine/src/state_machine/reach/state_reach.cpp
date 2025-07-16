#include "state_reach.h"

static void state_reach_run(struct state_reach_data *data) {
    // TODO: implement reaching functionality
}

void state_reach_init(struct state_reach_data *data) {
    // TODO: intialize reach data
}

void state_reach_enter(struct state_reach_data *data, state_e from, state_event_e event) {
    switch (from) {
        case STATE_REACH:
            // do nothing
            break;
        default:
            display_handler.clearDisplay();
            display_handler.setCursor(0, 0);
            display_handler.println("REACH");
            display_handler.display();
            break;
    }
    state_reach_run(data);
}

void state_reach_exit(struct state_reach_data *data, state_e to, state_event_e event) {
    
}
