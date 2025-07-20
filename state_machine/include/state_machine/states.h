#ifndef STATES_H
#define STATES_H

typedef enum {
    STATE_WAIT,
    STATE_TAPE_FOLLOWING,
    STATE_REACH,
    STATE_CLOSE_CLAW,
    STATE_STORE,
    STATE_RETREAT,
    STATE_RETURN_PETS
} state_e;

#endif