#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <config/pin_out.h>
#include <config/pwm_config.h>

/*
    Each state should implement the following functions:
    
    <state>_enter(struct state_machine *sm, state_e from, state_event_e event)    handles things when first entering the state from any other state (including itself).
    <state>_exit(struct state_machine *sm)                                        handles things when exiting a state
    <state>_run(struct state_machine *sm)                                         one iteration of the state's routine. Post any internal events from here.
    
    Most states will only need to implement the first and third functions.

*/


#define DEBUG // comment out this line to remove logging through Serial and the OLED display


/* Actuators */
#include <actuators/Arm.h>
#include <actuators/Servo.h>
#include <actuators/Claw.h>
#include <actuators/BaseGear.h>

extern Arm arm;
extern Servo claw;
extern Motor left_motor;
extern Motor right_motor;
extern Motor cascade_motor;
extern BaseGear base_gear;


/* Sensors */
#include <sensors/MagneticEncoder.h>
#include <sensors/Sonar.h>
#include <sensors/ToF.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

extern SemaphoreHandle_t i2c_mutex;
extern volatile state_event_e tof_reading;
// extern SemaphoreHandle_t tof_reading_mutex;
extern TaskHandle_t tof_task_handle;
extern TaskHandle_t dist_task_handle;
extern ToF tof_claw;
extern ToF tof_chassis;
extern ToF_data tof_data_claw;
extern ToF_data tof_data_chassis;
extern volatile bool ramp_detected;
#define RAMP_TIME_S 10


/* STATE MACHINE */
#include "states.h"
#include "events.h"

struct state_machine {

    state_e state;
    state_event_e internal_event;
    
    int stable_pet_count_left;
    int stable_pet_count_right;
    int stable_pillar_count;
    
    int pets_stored;
    bool arm_in_start_pos;
    bool claw_closed;
    int claw_open_angle;         // either CLAW_OPEN or CLAW_SEMI_OPEN
    int attempts;

    float last_error;
    unsigned long last_pid_time; // set to zero when leaving the state
    bool last_ir_ll, last_ir_l, last_ir_c, last_ir_r, last_ir_rr;

};


/* FUNCTIONS */
void            state_machine_init  (struct state_machine *state_machine);
state_event_e   process_input       (struct state_machine *state_machine);
void            tof_input_task      (void *pvParameters);
void            dist_input_task     (void *pvParameters);
void            process_event       (struct state_machine *state_machine, state_event_e next_event);
bool IRAM_ATTR  timer_isr_callback  (void *args);


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