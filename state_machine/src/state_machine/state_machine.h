#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#define DEBUG 1
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 // shared with the esp32 reset pin

#include "wait/state_wait.h"
#include "tape_following/state_tape_following.h"
#include "reach/state_reach.h"
#include "raise_arm/state_raise_arm.h"
#include "close_claw/state_close_claw.h"
#include "store/state_store.h"
#include "ramp/state_ramp.h"
#include "debris/state_debris.h"
#include "tape_sweep/state_tape_sweep.h"
#include "extend_cascade/state_extend_cascade.h"
#include "reverse/state_reverse.h"

/* creates a global instance of the display object */
Adafruit_SSD1306 display_handler(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

typedef enum {
    STATE_WAIT,
    STATE_TAPE_FOLLOWING,
    STATE_REACH,
    STATE_RAISE_ARM,
    STATE_CLOSE_CLAW,
    STATE_STORE,
    STATE_RAMP,
    STATE_DEBRIS,
    STATE_TAPE_SWEEP,
    STATE_EXTEND_CASCADE,
    STATE_REVERSE,
} state_e;

typedef enum {
    STATE_EVENT_NONE,
    STATE_EVENT_TAPE_DETECTED,
    STATE_EVENT_PET_DETECTED,
    STATE_EVENT_PILLAR_DETECTED,
    STATE_EVENT_NEAR_PET,
    STATE_EVENT_PET_GRASPED,
    STATE_EVENT_PET_STORED,
    STATE_EVENT_RAMP_DETECTED,
    STATE_EVENT_DEBRIS_DETECTED,
    STATE_EVENT_FLAT_GROUND_DETECTED,
    STATE_EVENT_EDGE_DETECTED,
    STATE_EVENT_CASCADE_EXTENDED,
    STATE_EVENT_ZIP_LINE_DETECTED, // probably a timer
} state_event_e;

void state_machine_run(void);

/*
    Each state must implement the following functions:
    
    init(state_data)                        intiializes the data for the state.
    enter(state_data, state_from, event)    handles things when first entering the state from any other state (including itself).
    exit(state_data, state_to, event)       handles things when exiting a state into any other state
    run(state_data)                         one iteration of the state's routine. Post any internal events here.
    
*/

#endif