#include <Arduino.h>
#include <assert.h>
#include <pin_out.h>
#include "state_machine.h"
#include "close_claw/state_close_claw.h"
#include "raise_arm/state_raise_arm.h"
#include "reach/state_reach.h"
#include "store/state_store.h"
#include "wait/state_wait.h"

/*
This is a state machine implemented with enums and functions.
The robot moves between states through Transitions, which are triggered by Events.
Events are determined by the sensors.
This approach has been taken from Artful Bytes' youtube video.
video:  https://www.youtube.com/watch?v=NTEHRjiAY2I&t=1498s&ab_channel=ArtfulBytes
github: https://github.com/artfulbytes/nsumo_video

1. Process the sensor input and return the appropriate event
2. Process the event and change the state accordingly
3. Run a step of that state's routine
4. Repeat

There exists a "nothing happened" event to keep the state the same.
Input is polling, and the data structure to store the state machine's 
information is stack-allocated (local variable) and passed around as a pointer for now.
I think we can have interrupts if we want, but then we need some global 
data storage (memory management issues?) and have interrupts store it in there.
If we want to keep a history of any data, we may need to implement a ring buffer.

Alternative implementation:
Instead of writing new functions and updating this file whenever 
we add a new state, we can take advantage of c++ classes, inheritance, and polymorphism.
The downside is that this could be more confuing and more difficult to debug.
It would be cleaner imo though.
*/

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

volatile bool button_pressed = false;

Adafruit_SSD1306 display_handler(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Servo servo_1(ARM_SERVO_1_ANGLE_MAX);
Servo servo_2(ARM_SERVO_2_ANGLE_MAX);
Arm arm(&servo_1, &servo_2);
Servo servo_3(CLAW_OPEN);
Claw claw(&servo_3);

/* STRUCTS */
struct state_transition {
    state_e previous_state;
    state_event_e event;
    state_e next_state;
};

// consult the diagram to understand these transitions
static const struct state_transition state_transitions[] = {
    {   STATE_WAIT,             STATE_EVENT_PET_DETECTED,           STATE_REACH             },
    {   STATE_REACH,            STATE_EVENT_NEAR_PET,               STATE_CLOSE_CLAW        },
    {   STATE_CLOSE_CLAW,       STATE_EVENT_PET_GRASPED,            STATE_STORE             },
    {   STATE_STORE,            STATE_EVENT_PET_STORED,             STATE_WAIT              },
};


/* FUNCTIONS */
static state_event_e process_input       (struct state_machine *state_machine);
static void          process_event       (struct state_machine *state_machine, state_event_e next_event);
static void          state_enter         (struct state_machine *state_machine, state_e next_state);
static void          state_exit          (struct state_machine *state_machine, state_e previous_state);

void state_machine_init(struct state_machine *sm) {

    sm->state = STATE_WAIT;
    sm->internal_event = STATE_EVENT_NONE;

    // set up the servos
    servo_1.attach(PIN_ARM_SERVO_1, CHANNEL_ARM_SERVO_1, 500, 2500);
    servo_2.attach(PIN_ARM_SERVO_2, CHANNEL_ARM_SERVO_2, 500, 2500);
    servo_3.attach(PIN_CLAW, CHANNEL_CLAW, 500, 2500);
    
    // wait state

    // reach state

    // raise state

    // close claw state

    // store state

}

void state_machine_run(struct state_machine *state_machine) {

    while(1) {
        const state_event_e event = process_input(state_machine);
        process_event(state_machine, event);
    }

}

static state_event_e process_input(struct state_machine *state_machine) {
    
    // TODO: poll input from each sensor
    // - ToF sensors (x2)
    // - sonar sensors (x2)
    // - rotary encoder?
    // - megnetic encoder?

    // state_machine->reach_data.distance = 0; // update with the ToF average distance
    // TODO: read ToF if there is a pet
    if (0) {
        return STATE_EVENT_PET_DETECTED;
    }

    // update data from globals modified from interrupts
    if (button_pressed) {
        button_pressed = false;
        return STATE_EVENT_BUTTON_PRESSED;
    }

    return STATE_EVENT_NONE;
}

static void process_event(struct state_machine *state_machine, state_event_e next_event) {
    
    if (next_event == STATE_EVENT_NONE) {
        state_enter(state_machine, state_machine->state); // stay in the same state
    }

    for (int i = 0 ; i < ARRAY_SIZE(state_transitions); i ++) {
        if(state_machine->state == state_transitions[i].previous_state && next_event == state_transitions[i].event) {
            
            if (DEBUG) {
                print_event(next_event);
                print_state(state_transitions[i].next_state);
            }
            
            state_exit(state_machine, state_transitions[i].previous_state);
            state_enter(state_machine, state_transitions[i].next_state);
            return;
        }
    }
    assert(0); // throw an error
}

static void state_enter(struct state_machine *state_machine, state_e next_state) {
    
    switch (next_state) {
    case STATE_WAIT:
        state_wait_enter(state_machine, state_machine->state);
        break;
    case STATE_REACH:
        state_reach_enter(state_machine, state_machine->state);
        break;
    case STATE_RAISE_ARM:
        state_raise_arm_enter(state_machine, state_machine->state);
        break;
    case STATE_CLOSE_CLAW:
        state_close_claw_enter(state_machine, state_machine->state);
        break;
    case STATE_STORE:
        state_store_enter(state_machine, state_machine->state);
        break;
    default:
        break;
    }
}

static void state_exit(struct state_machine *state_machine, state_e previous_state) {

    switch (previous_state) {
    case STATE_WAIT:
        state_wait_exit(state_machine);
        break;
    case STATE_REACH:
        state_reach_exit(state_machine);
        break;
    case STATE_RAISE_ARM:
        state_raise_arm_exit(state_machine);
        break;
    case STATE_CLOSE_CLAW:
        state_close_claw_exit(state_machine);
        break;
    case STATE_STORE:
        state_store_exit(state_machine);
        break;
    default:
        break;
    }

}

void print_state(state_e state) {
    switch(state) {
        case STATE_WAIT: 
        display_handler.println("WAIT");
        break;
    case STATE_TAPE_FOLLOWING:
        display_handler.println("TAPE FOLLOWING");
        break;
    case STATE_REACH:
        display_handler.println("REACH");
        break;
    case STATE_RAISE_ARM:
        display_handler.println("RAISE ARM");
        break;
    case STATE_CLOSE_CLAW:
        display_handler.println("CLOSE ARM");
        break;
    case STATE_STORE:
        display_handler.println("STORE");
        break;
    case STATE_RAMP:
        display_handler.println("RAMP");
        break;
    case STATE_DEBRIS:
        display_handler.println("DEBRIS");
        break;
    case STATE_TAPE_SWEEP:
        display_handler.println("TAPE SWEEP");
        break;
    case STATE_EXTEND_CASCADE:
        display_handler.println("CASCADE");
        break;
    case STATE_REVERSE:
        display_handler.println("REVERSE");
        break;
    default:
        break;
    }
    display_handler.display();
}

void print_event(state_event_e event) {
    display_handler.clearDisplay();
    display_handler.setCursor(0, 0);
    switch (event) {
    case STATE_EVENT_NONE:
        display_handler.println("NONE");
        break;
    case STATE_EVENT_TAPE_DETECTED:
        display_handler.println("TAPE DETECTED");
        break;
    case STATE_EVENT_PET_DETECTED:
        display_handler.println("PET DETECTED");
        break;
    case STATE_EVENT_PILLAR_DETECTED:
        display_handler.println("PILLAR DETECTED");
        break;
    case STATE_EVENT_NEAR_PET:
        display_handler.println("NEAR PET");
        break;
    case STATE_EVENT_PET_GRASPED:
        display_handler.println("PET GRASPED");
        break;
    case STATE_EVENT_PET_STORED:
        display_handler.println("PET STORED");
        break;
    case STATE_EVENT_RAMP_DETECTED:
        display_handler.println("RAMP DETECTED");
        break;
    case STATE_EVENT_DEBRIS_DETECTED:
        display_handler.println("DEBRIS DETECTED");
        break;
    case STATE_EVENT_FLAT_GROUND_DETECTED:
        display_handler.println("FLAT GROUND DETECTED");
        break;
    case STATE_EVENT_EDGE_DETECTED:
        display_handler.println("EDGE DETECTED");
        break;
    case STATE_EVENT_CASCADE_EXTENDED:
        display_handler.println("CASCADE EXTENDED");
        break;
    case STATE_EVENT_ZIP_LINE_DETECTED:
        display_handler.println("ZIP LINE");
        break;
    default:
        break;
    }
    display_handler.display();
}

// IRAM safe code
void IRAM_ATTR button_pressed_ISR() {
    button_pressed = true;
}
