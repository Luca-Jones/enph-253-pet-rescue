#ifndef STATE_CLOSE_CLAW_H
#define STATE_CLOSE_CLAW_H

#include "../state_machine.h"
#include "../states.h"

void state_close_claw_init(struct state_machine *state_machine);
void state_close_claw_enter(struct state_machine *state_machine, state_e from);
void state_close_claw_exit(struct state_machine *state_machine);

#endif