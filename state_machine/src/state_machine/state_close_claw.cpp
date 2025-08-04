#include <state_machine/state_close_claw.h>
#include <actuators/Claw.h>
#include <driver/timer.h>

#define CLAW_WAIT_TIME_MS 1000

static void state_close_claw_run(struct state_machine *state_machine) {
    claw.write(CLAW_CLOSED);
    delay(CLAW_WAIT_TIME_MS); 
    // if (state_machine->pets_stored == 0) {
    //     timer_start(TIMER_GROUP_0, TIMER_0);
    //     // move the arm back home
    //     base_gear.write(BASE_GEAR_HOME);
    //     arm.move_to_pos(ARM_HOME_X, ARM_HOME_Y);
    //     state_machine->internal_event = EVENT_FIRST_PET_GRASPED;
    // } else {
        state_machine->internal_event = EVENT_PET_GRASPED;
    // }
}

void state_close_claw_enter(struct state_machine *state_machine) {
    state_close_claw_run(state_machine);
}
