#include "state_wait.h"
#include "esp32-hal.h"

static void state_wait_run(struct state_wait_data *data) {
    delay(500);
}

void state_wait_init(struct state_wait_data *data) {
    // no data to initialize
}

void state_wait_enter(struct state_wait_data *data, state_e from, state_event_e event) {
    switch (from) {
        case STATE_WAIT:
            // do nothing
            break;
        default:
            display_handler.clearDisplay();
            display_handler.setCursor(0, 0);
            display_handler.println("WAIT");
            display_handler.display();
            break;
    }
    state_wait_run(data);
}

void state_wait_exit(struct state_wait_data *data, state_e to, state_event_e event) {
    // do nothing
}