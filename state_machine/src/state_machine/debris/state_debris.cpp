#include "state_debris.h"

static void state_debris_run(struct state_debris_data *data) {
    // TODO: implement functionality
}

void state_debris_init(struct state_debris_data *data) {
    // TODO: intialize data
}

void state_debris_enter(struct state_debris_data *data, state_e from, state_event_e event) {
    switch (from) {
        case STATE_DEBRIS:
            // do nothing 
            break;
        default:
            display_handler.clearDisplay();
            display_handler.setCursor(0, 0);
            display_handler.println("DEBRIS");
            display_handler.display();
            break;
    }
    state_debris_run(data);
}

void state_debris_exit(struct state_debris_data *data, state_e to, state_event_e event) {
    
}
