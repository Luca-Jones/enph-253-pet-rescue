#include <state_machine/state_retreat.h>
#include <actuators/Claw.h>
#include <actuators/Arm.h>

#define RETREAT_ATTEMPTS 3

void state_retreat_run(struct state_machine *state_machine) {
    int x,y;
    arm.get_pos(&x, &y);
    arm.lerp_to_pos(ARM_HOME_X, y, 500);
    claw.write(state_machine->claw_open_angle);
    state_machine->attempts++;
    state_machine->internal_event = (state_machine->attempts < RETREAT_ATTEMPTS) ? EVENT_ARM_READY : EVENT_PET_FAILED;
}

void state_retreat_enter(struct state_machine *state_machine) {
    state_retreat_run(state_machine);
}
