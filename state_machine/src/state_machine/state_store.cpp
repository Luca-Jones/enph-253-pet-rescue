#include <state_machine/state_store.h>
#include <actuators/Arm.h>

static void state_store_run(struct state_machine *state_machine) {
    
    arm.lerp_to_pos(ARM_RAISED_X, ARM_RAISED_Y, 500);

    base_gear.write(BASE_GEAR_STORE);

    delay(500);
    claw.write(CLAW_OPEN);
    delay(1000);

    base_gear.write(BASE_GEAR_HOME);

    arm.move_to_pos(ARM_HOME_X, ARM_HOME_Y);
    delay(1000);
    claw.write(CLAW_OPEN);
    delay(500);

    state_machine->pets_stored++;
    state_machine->claw_closed = false;
    state_machine->internal_event = EVENT_PET_STORED;
}

void state_store_enter(struct state_machine *state_machine) {
    state_store_run(state_machine);
}
