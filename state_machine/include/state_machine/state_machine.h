#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <config/pin_out.h>
#include <config/pwm_config.h>

/*
    Each state should implement the following functions:
    
    <state>_enter(struct state_machine *sm, state_e from, state_event_e event)    handles things when first entering the state from any other state (including itself).
    <state>_exit(struct state_machine *sm)                                        handles things when exiting a state
    <state>_run(struct state_machine *sm)                                         one iteration of the state's routine. Post any internal events from here.
    
*/

#define DEBUG // comment out this line to remove logging through Serial and the OLED display


/* Actuators */
#include <actuators/Servo.h>
#include <actuators/Arm.h>
#include <actuators/Claw.h>
#include <actuators/BaseGear.h>

extern Servo servo_1;
extern Servo servo_2;
extern Arm arm;
extern Servo claw;
extern BaseGear base_gear;


/* Sensors */
#include <sensors/MagneticEncoder.h>
#include <sensors/RotaryEncoder.h>
#include <sensors/Sonar.h>
#include <sensors/ToF.h>

extern ToF tof_claw;
extern ToF tof_chassis;


/* STATE MACHINE */
#include "states.h"
#include "events.h"

// TODD: decide what should be included in the state machine struct
struct state_machine {
    state_e state;
    state_event_e internal_event;
    int pets;
    ToF_data tof_data_claw;
    ToF_data tof_data_chassis;
    int stable_pet_count;
    int stable_pillar_count;
};


/* FUNCTIONS */
void            state_machine_init  (struct state_machine *state_machine);
state_event_e   process_input       (struct state_machine *state_machine);
void            process_event       (struct state_machine *state_machine, state_event_e next_event);
void IRAM_ATTR  button_pressed_ISR  ();
void IRAM_ATTR  limit_switch_ISR    ();


#ifdef DEBUG

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 // shared with the esp32 reset pin
extern Adafruit_SSD1306 display_handler;    

void print_state(state_e state);
void print_event(state_event_e event);

#endif


#endif