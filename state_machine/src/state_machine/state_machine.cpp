#include <Arduino.h>
#include <assert.h>
#include "state_machine.h"

/*
This is a state machine implemented with enums and functions
states are linked through transitions, which are triggered by events.
This approach has been taken from Artful Bytes' youtube video.
video:  https://www.youtube.com/watch?v=NTEHRjiAY2I&t=1498s&ab_channel=ArtfulBytes
github: https://github.com/artfulbytes/nsumo_video

1. Process the input and return the appropriate event
2. Process the event and change the state accordingly
3. Run a step of that state's routine
4. Repeat

There exists a "nothing happened" event to keep the state the same.
Input is polling, and the data structure to store the data is stack-allocated
and passed around as a pointer for now.
I think we can have interrupts if we want, but then we need some global 
data storage (memory management issues?) and have interrupts store it in there.
If we want to keep a history of any data, we may need to implement a ring buffer.

Another issue is how the routines are run. The claw movement is quite simple, 
but pid needs the last iterations data (derivative as well as a tally (integral)
of the past data. A history would be overkill for pid, so maybe that stuff could
be taken care of in its data struct, but I still don't really understand how 
the data is being passed around here right now.

Alternative implementation:
Instead of writing new functions and uodating the main state_machine whenever 
we add a new state, we can take advantage of c++ classes, inheritance, and polymorphism.
The downside is that this could be more confuing and more difficult to debug.
It would be cleaner imo though.
*/

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

/* STRUCTS */

struct state_machine {
    state_e state;
    state_event_e internal_event;
    struct state_wait_data              wait_data;
    struct state_tape_following_data    tape_following_data;
    struct state_reach_data             reach_data;
    struct state_raise_arm_data         raise_arm_data;
    struct state_close_claw_data        close_claw_data;
    struct state_store_data             store_data;
    struct state_ramp_data              ramp_data;
    struct state_debris_data            debris_data;
    struct state_tape_sweep_data        tape_sweep_data;
    struct state_extend_cascade_data    extend_cascade_data;
    struct state_reverse_data           reverse_data;
};

struct state_transition {
    state_e previous_state;
    state_event_e event;
    state_e next_state;
};

// consult the diagram to understand these transitions
static const struct state_transition state_transitions[] = {
    {   STATE_WAIT,             STATE_EVENT_TAPE_DETECTED,          STATE_TAPE_FOLLOWING    },
    {   STATE_TAPE_FOLLOWING,   STATE_EVENT_PET_DETECTED,           STATE_REACH             },
    {   STATE_TAPE_FOLLOWING,   STATE_EVENT_PILLAR_DETECTED,        STATE_RAISE_ARM         },
    {   STATE_RAISE_ARM,        STATE_EVENT_PET_DETECTED,           STATE_REACH             },
    {   STATE_REACH,            STATE_EVENT_NEAR_PET,               STATE_CLOSE_CLAW        },
    {   STATE_CLOSE_CLAW,       STATE_EVENT_PET_GRASPED,            STATE_STORE             },
    {   STATE_STORE,            STATE_EVENT_PET_STORED,             STATE_TAPE_FOLLOWING    },
    {   STATE_TAPE_FOLLOWING,   STATE_EVENT_RAMP_DETECTED,          STATE_RAMP              },
    {   STATE_RAMP,             STATE_EVENT_FLAT_GROUND_DETECTED,   STATE_TAPE_SWEEP        },
    {   STATE_TAPE_FOLLOWING,   STATE_EVENT_DEBRIS_DETECTED,        STATE_DEBRIS            },
    {   STATE_TAPE_SWEEP,       STATE_EVENT_TAPE_DETECTED,          STATE_TAPE_FOLLOWING    },
    {   STATE_TAPE_FOLLOWING,   STATE_EVENT_EDGE_DETECTED,          STATE_EXTEND_CASCADE    },
    {   STATE_EXTEND_CASCADE,   STATE_EVENT_CASCADE_EXTENDED,       STATE_REVERSE           },
    {   STATE_REVERSE,          STATE_EVENT_ZIP_LINE_DETECTED,      STATE_WAIT              }, // tbd
};


/* FUNCTIONS */
static void          state_machine_init  (struct state_machine *data);
static state_event_e process_input       (struct state_machine *data);
static void          process_event       (struct state_machine *data, state_event_e next_event);
static void          state_enter         (struct state_machine *state_machine, struct state_transition transition);
static void          state_exit          (struct state_machine *state_machine, struct state_transition transition);

void state_machine_run(void) {

    struct state_machine state_machine; // stack allocated data that gets passed around

    state_machine_init(&state_machine);

    while(1) {
        const state_event_e event = process_input(&state_machine);
        process_event(&state_machine, event);
        delay(1000);
    }
    
}

static void state_machine_init(struct state_machine *sm) {

    sm->state = STATE_WAIT;
    sm->internal_event = STATE_EVENT_NONE;
    
    // wait state
    sm->wait_data.state_machine = sm;
    state_wait_init(&sm->wait_data);

    // tape following state
    sm->tape_following_data.state_machine = sm;
    state_tape_following_init(&sm->tape_following_data);

    // reach state
    sm->reach_data.state_machine = sm;
    state_reach_init(&sm->reach_data);

    // raise state
    sm->raise_arm_data.state_machine = sm;
    state_raise_arm_init(&sm->raise_arm_data);

    // close claw state
    sm->close_claw_data.state_machine = sm;
    state_close_claw_init(&sm->close_claw_data);

    // store state
    sm->store_data.state_machine = sm;
    state_store_init(&sm->store_data);

    // ramp state
    sm->ramp_data.state_machine = sm;
    state_ramp_init(&sm->ramp_data);

    // debris state
    sm->debris_data.state_machine = sm;
    state_debris_init(&sm->debris_data);

    // tape sweep state
    sm->tape_sweep_data.state_machine = sm;
    state_tape_sweep_init(&sm->tape_sweep_data);

    // extend cascade state
    sm->extend_cascade_data.state_machine = sm;
    state_extend_cascade_init(&sm->extend_cascade_data);

    // reverse state
    sm->reach_data.state_machine = sm;
    state_reverse_init(&sm->reverse_data);

}

static state_event_e process_input(struct state_machine *data) {
    
    // TODO: poll input from each sensor
    // - ToF sensors (x2)
    // - sonar sensors (x2)
    // - rotary encoder?
    // - megnetic encoder?

    // TODO: update data from globals modified from interrupts

    return STATE_EVENT_NONE;
}

static void process_event(struct state_machine *state_machine, state_event_e next_event) {
    // TODO: deal with STATE_EVENT_NONE
    for (int i = 0 ; i < ARRAY_SIZE(state_transitions); i ++) {
        if(state_machine->state == state_transitions[i].previous_state && next_event == state_transitions[i].event) {
            state_exit(state_machine, state_transitions[i]);
            state_enter(state_machine, state_transitions[i]);
            return;
        }
    }
    assert(0); // throw an error
}

static void state_enter(struct state_machine *state_machine, struct state_transition transition) {
    
    if (transition.previous_state != transition.next_state) {
        // clear any timers
        state_machine->state = transition.next_state;
    }

    switch (transition.next_state) {
    case STATE_WAIT:
        state_wait_enter(&state_machine->wait_data, transition.previous_state, transition.event);
        break;
    case STATE_TAPE_FOLLOWING:
        state_tape_following_enter(&state_machine->tape_following_data, transition.previous_state, transition.event);
        break;
    case STATE_REACH:
        state_reach_enter(&state_machine->reach_data, transition.previous_state, transition.event);
        break;
    case STATE_RAISE_ARM:
        state_raise_arm_enter(&state_machine->raise_arm_data, transition.previous_state, transition.event);
        break;
    case STATE_CLOSE_CLAW:
        state_close_claw_enter(&state_machine->close_claw_data, transition.previous_state, transition.event);
        break;
    case STATE_STORE:
        state_store_enter(&state_machine->store_data, transition.previous_state, transition.event);
        break;
    case STATE_RAMP:
        state_ramp_enter(&state_machine->ramp_data, transition.previous_state, transition.event);
        break;
    case STATE_DEBRIS:
        state_debris_enter(&state_machine->debris_data, transition.previous_state, transition.event);
        break;
    case STATE_TAPE_SWEEP:
        state_tape_sweep_enter(&state_machine->tape_sweep_data, transition.previous_state, transition.event);
        break;
    case STATE_EXTEND_CASCADE:
        state_extend_cascade_enter(&state_machine->extend_cascade_data, transition.previous_state, transition.event);
        break;
    case STATE_REVERSE:
        state_reverse_enter(&state_machine->reverse_data, transition.previous_state, transition.event);
        break;
    }
    // no default case means a compile error if this switch 
    // doesn't cover all elements of the state_e enum
}

static void state_exit(struct state_machine *state_machine, struct state_transition transition) {

    switch (transition.next_state) {
    case STATE_WAIT:
        state_wait_exit(&state_machine->wait_data, transition.next_state, transition.event);
        break;
    case STATE_TAPE_FOLLOWING:
        state_tape_following_exit(&state_machine->tape_following_data, transition.next_state, transition.event);
        break;
    case STATE_REACH:
        state_reach_exit(&state_machine->reach_data, transition.next_state, transition.event);
        break;
    case STATE_RAISE_ARM:
        state_raise_arm_exit(&state_machine->raise_arm_data, transition.next_state, transition.event);
        break;
    case STATE_CLOSE_CLAW:
        state_close_claw_exit(&state_machine->close_claw_data, transition.next_state, transition.event);
        break;
    case STATE_STORE:
        state_store_exit(&state_machine->store_data, transition.next_state, transition.event);
        break;
    case STATE_RAMP:
        state_ramp_exit(&state_machine->ramp_data, transition.next_state, transition.event);
        break;
    case STATE_DEBRIS:
        state_debris_exit(&state_machine->debris_data, transition.next_state, transition.event);
        break;
    case STATE_TAPE_SWEEP:
        state_tape_sweep_exit(&state_machine->tape_sweep_data, transition.next_state, transition.event);
        break;
    case STATE_EXTEND_CASCADE:
        state_extend_cascade_exit(&state_machine->extend_cascade_data, transition.next_state, transition.event);
        break;
    case STATE_REVERSE:
        state_reverse_exit(&state_machine->reverse_data, transition.next_state, transition.event);
        break;
    }
    // no default case means a compile error if this switch 
    // doesn't cover all elements of the state_e enum

}