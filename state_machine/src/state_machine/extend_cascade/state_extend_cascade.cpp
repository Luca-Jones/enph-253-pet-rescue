#include "state_extend_cascade.h"

static void state_extend_cascade_run(struct state_extend_cascade_data *data) {
    // TODO: implement functionality
}

void state_extend_cascade_init(struct state_extend_cascade_data *data) {
    // TODO: intialize data
}

void state_extend_cascade_enter(struct state_extend_cascade_data *data, state_e from, state_event_e event) {
    switch (from) {
        case STATE_EXTEND_CASCADE:
            // do nothing
            break;
        default:
            display_handler.clearDisplay();
            display_handler.setCursor(0, 0);
            display_handler.println("EXTEND CASCADE");
            display_handler.display();
            break;
    }
    state_extend_cascade_run(data);
}

void state_extend_cascade_exit(struct state_extend_cascade_data *data, state_e to, state_event_e event) {
    
}
