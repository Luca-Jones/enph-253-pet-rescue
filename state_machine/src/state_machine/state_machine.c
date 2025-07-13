#include "state_machine.h"
#include "state_common.h"
#include "state_tape_following.h"
#include "state_wait.h"
#include <assert.h>
#include <Arduino.h>

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))


/*
    This is a state machine implemented with enums and functions
    states are linked through transitions, which are triggered by events.
    This approach has been taken from Artful Bytes' youtube video.
    video:  https://www.youtube.com/watch?v=NTEHRjiAY2I&t=1498s&ab_channel=ArtfulBytes
    github: https://github.com/artfulbytes/nsumo_video

    1. Process the input and return the appropriate event
    2. Process the event and change the state accordingly
    3. Run a step of that state's routine
    3. Repeat

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


struct state_machine_data {
    state_e state;
    struct state_common_data common_data;
    struct state_wait_data wait_data;
    struct state_tape_following_data tape_following_data;
    state_event_e internal_event;
    // data for other states...
    // data history?
};

struct state_transition {
    state_e from;
    state_event_e event;
    state_e to;
};

// consult the diagram to understand these transitions
static const struct state_transition state_transitions[] = {
    {STATE_WAIT,    STATE_EVENT_TAPE_SPOTTED,   STATE_TAPE_FOLLOWING},
};




/* Functions */

static inline state_event_e take_internal_event(struct state_machine_data *data) {
    const state_event_e event = data->internal_event;
    data->internal_event = STATE_EVENT_NONE;
    return event;
}

void state_machine_post_internal_event(struct state_machine_data *data, state_event_e event) {
    data->internal_event = event;
}

static void state_enter(struct state_machine_data *data, state_e from, state_event_e event, state_e to) {
    
    if (from != to) {
        // clear any timers
        data->state = to;
        // log the state
    }

    switch (to) {
    case STATE_WAIT:
        // call the wait states enter function
        state_wait_enter(&(data->wait_data), from, event);
        break;
    case STATE_TAPE_FOLLOWING:
        state_tape_following_enter(&(data->tape_following_data), from, event);
        break;
    }
    // no default case means a compile error if this switch 
    // does not cover all elements of the state_e enum
}

static inline void process_event(struct state_machine_data *data, state_event_e next_event) {
    for (int i = 0 ; i < ARRAY_SIZE(state_transitions); i ++) {
        if(data->state == state_transitions[i].from && next_event == state_transitions[i].event) {
            state_enter(data, state_transitions[i].from, next_event, state_transitions[i].to);
            return;
        }
    }
    assert(0); // throw an error
}

static inline state_event_e process_input(struct state_machine_data *data) {
    // get input
    // save the input to the history
    return STATE_EVENT_NONE;
}

static inline void state_machine_init(struct state_machine_data *data) {
    data->state = STATE_WAIT;
    data->common_data.state_machine_data = data;
    // internal events
    // input history

}

void state_machine_run(void) {

    struct state_machine_data data;

    state_machine_init(&data);

    while(1) {
        const state_event_e event = process_input(&data);
        process_event(&data, event);
        delay(1000);
    }
    
    // allocate and initialize state machine data
    // loop:
    //  process input
    //  process event
}