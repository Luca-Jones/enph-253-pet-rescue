#include <Arduino.h>
#include <assert.h>
#include "state_machine.h"
#include "wait/state_wait.h"
#include "tape_following/state_tape_following.h"

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
    struct state_wait_data wait_data;
    struct state_tape_following_data tape_following_data;
    // data for other states...
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
    // {   STATE_REVERSE,          STATE_EVENT_ZIP_LINE_DETECTED,      STATE_WAIT              }, // tbd
};


/* FUNCTIONS */
static void          state_machine_init  (struct state_machine *data);
static state_event_e process_input       (struct state_machine *data);
static void          process_event       (struct state_machine *data, state_event_e next_event);
static void          state_enter         (struct state_machine *state_machine, struct state_transition state_transition);


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

    // tape following state
    sm->tape_following_data.state_machine = sm;

    // other states...

}

static state_event_e process_input(struct state_machine *data) {
    
    // TODO: poll input from each sensor

    // TODO: update data from globals modified from interrupts

    return STATE_EVENT_NONE;
}

static void process_event(struct state_machine *state_machine, state_event_e next_event) {
    for (int i = 0 ; i < ARRAY_SIZE(state_transitions); i ++) {
        if(state_machine->state == state_transitions[i].previous_state && next_event == state_transitions[i].event) {
            state_enter(state_machine, state_transitions[i]);
            return;
        }
    }
    // TODO: deal with STATE_EVENT_NONE
    assert(0); // throw an error
}

static void state_enter(struct state_machine *state_machine, struct state_transition state_transition) {
    
    if (state_transition.previous_state != state_transition.next_state) {
        // clear any timers
        state_machine->state = state_transition.next_state;
        // log the state
    }

    switch (state_transition.next_state) {
    case STATE_WAIT:
        state_wait_enter(&state_machine->wait_data, state_transition.previous_state, state_transition.event);
        break;
    case STATE_TAPE_FOLLOWING:
        state_tape_following_enter(&state_machine->tape_following_data, state_transition.previous_state, state_transition.event);
        break;
    // other states...
    }
    // no default case means a compile error if this switch 
    // doesn't cover all elements of the state_e enum
}
