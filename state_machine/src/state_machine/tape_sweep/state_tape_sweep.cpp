#include "state_tape_sweep.h"

static void state_tape_sweep_run(struct state_tape_sweep_data *data) {
    // TODO: implement functionality
}

void state_tape_sweep_init(struct state_tape_sweep_data *data) {
    // TODO: intialize data
}

void state_tape_sweep_enter(struct state_tape_sweep_data *data, state_e from, state_event_e event) {
    switch (from) {
        case STATE_TAPE_SWEEP:
            break;
        default:
            display_handler.clearDisplay();
            display_handler.setCursor(0, 0);
            display_handler.println("TAPE SWEEP");
            display_handler.display();
            break;
    }
    state_tape_sweep_run(data);
}

void state_tape_sweep_exit(struct state_tape_sweep_data *data, state_e to, state_event_e event) {

}