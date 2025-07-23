#include <Arduino.h>
#include <state_machine/state_machine.h>

struct state_machine state_machine;
// unsigned long last_time_ms = 0;

void setup() {
    #ifdef DEBUG
    Serial.begin(115200);
    #endif
    state_machine_init(&state_machine);
}


void loop() {

    // #ifdef DEBUG
    // unsigned long current_time_ms = micros();
    // Serial.printf("delay = %d\n", current_time_ms - last_time_ms);
    // last_time_ms = current_time_ms;
    // #endif

    state_event_e event = process_input(&state_machine);
    process_event(&state_machine, event);
}