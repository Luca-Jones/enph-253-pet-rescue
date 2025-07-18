#include "state_store.h"

static void state_store_run(struct state_machine *state_machine) {
    // moves to put the pet into the bucket; run once

    int x,y;
    arm.get_pos(&x,&y);
    arm.lerp_to_pos(ARM_HOME_X, y, 500);
    arm.lerp_to_pos(ARM_RAISED_X, ARM_RAISED_Y, 500);

    // TODO: turn the arm

    delay(1000);
    claw.open();
    delay(500);

    // turn the arm back to home

    arm.move_to_pos(ARM_HOME_X, ARM_HOME_Y);
    delay(500);

    state_machine->pets++;
    state_machine->internal_event = STATE_EVENT_PET_STORED;
}

void state_store_enter(struct state_machine *state_machine, state_e from) {
    switch (from) {
        case STATE_STORE:
            // do nothing
            break;
        default:
            break;
    }
    state_store_run(state_machine);
}

void state_store_exit(struct state_machine *state_machine) {
    // TODO: remove?
}
