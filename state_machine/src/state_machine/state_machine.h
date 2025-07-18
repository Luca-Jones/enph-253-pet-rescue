#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#define DEBUG

#ifdef DEBUG

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 // shared with the esp32 reset pin
extern Adafruit_SSD1306 display_handler;    

#endif

/* Actuators */
#include <Arm.h>
#include <Claw.h>

extern Servo servo_1;
extern Servo servo_2;
extern Servo servo_3;
extern Arm arm;
extern Claw claw;

/* Sensors */
#include <tof.h>

extern ToF tof;


/* STATE MACHINE */
#include "states.h"
#include "events.h"

struct state_machine {
    state_e state;
    state_event_e internal_event;
    int pets;
    ToF_data tof_data;
};

/* FUNCTIONS */
void state_machine_init(struct state_machine *state_machine);
state_event_e process_input (struct state_machine *state_machine);
void process_event (struct state_machine *state_machine, state_event_e next_event);
void IRAM_ATTR button_pressed_ISR();
#ifdef DEBUG
void print_state(state_e state);
void print_event(state_event_e event);
#endif

/*
    Each state must implement the following functions:
    
    <state>_enter(struct state_machine *sm, state_e from)    handles things when first entering the state from any other state (including itself).
    <state>_exit(struct state_machine *sm)                   handles things when exiting a state into any other state
    <state>_run(struct state_machine *sm)                    one iteration of the state's routine. Post any internal events here.
    
*/

#endif