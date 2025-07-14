#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

typedef enum {
    STATE_WAIT,
    STATE_TAPE_FOLLOWING,
    STATE_REACH,
    STATE_RAISE,
    STATE_CLOSE_CLAW,
    STATE_STORE,
    STATE_RAMP,
    STAE_DEBRIS,
    STATE_TAPE_SWEEP,
    STATE_EXTEND_CASCADE,
    STATE_REVERSE,
} state_e;

typedef enum {
    STATE_EVENT_NONE,
    STATE_EVENT_TAPE_DETECTED,
    STATE_EVENT_PET_DETECTED,
    STATE_EVENT_PILLAR_DETECTED,
    STATE_EVENT_PET_GRASPED,
    STATE_EVENT_PET_STORED,
    STATE_EVENT_RAMP_DETECTED,
    STATE_EVENT_DEBRIS_DETECTED,
    STATE_EVENT_FLAT_GROUND_DETECTED,
    STATE_EVENT_EDGE_DETECTED,
    // STATE_EVENT_ZIP_LINE_DETECTED, // tbd
} state_event_e;

void state_machine_run(void);

/*
    Each state must implement the following functions:
    
    init(state_data)                        intiializes the data for the state.
    enter(state_data, state_from, event)    handles things when first entering the state from any other state (including itself).
    run(state_data)                         one iteration of the state's routine. Post any internal events here.
    
*/

#endif