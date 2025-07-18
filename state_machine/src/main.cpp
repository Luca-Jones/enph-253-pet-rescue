#include <Arduino.h>
#include <pin_out.h>
#include "state_machine/state_machine.h"

static struct state_machine state_machine;

void setup() {
    #ifdef DEBUG
    Serial.begin(115200);
    #endif
    state_machine_init(&state_machine);
}

void loop() {
    state_event_e event = process_input(&state_machine);
    process_event(&state_machine, event);
}