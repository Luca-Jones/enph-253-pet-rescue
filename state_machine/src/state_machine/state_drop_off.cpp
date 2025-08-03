#include <state_machine/state_drop_off.h>
#include <actuators/Arm.h>
#include <actuators/Servo.h>
#include <actuators/Claw.h>

static void state_drop_off_run(state_machine *state_machine) {
    int x,y;
    arm.get_pos(&x, &y);
    arm.lerp_to_pos(ARM_DROP_OFF_X, ARM_DROP_OFF_Y, 500);
    claw.write(CLAW_OPEN);
    delay(1000);
    arm.lerp_to_pos(ARM_HOME_X, ARM_HOME_Y, 500);
    state_machine->internal_event = EVENT_PET_STORED; // continue tape following
}

void state_drop_off_enter(struct state_machine *state_machine) {
    state_drop_off_run(state_machine);
}
