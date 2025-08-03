#include <Arduino.h>
#include <driver/timer.h>

#include <state_machine/state_machine.h>
#include <state_machine/state_close_claw.h>
#include <state_machine/state_reach.h>
#include <state_machine/state_retreat.h>
#include <state_machine/state_return_pets.h>
#include <state_machine/state_store.h>
#include <state_machine/state_tape_following.h>
#include <state_machine/state_wait.h>
#include <state_machine/state_drop_off.h>

#include <config/dir_config.h>

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

/*
    Author: Luca Jones
    Date:   July 2025

    This is a state machine implemented with enums and functions.
    The robot moves between states through Transitions, which are triggered by Events.
    The event that ocurred is determined by the sensors.

    1. Process the sensor input and return the appropriate event
    2. Process the event and change the state accordingly
    3. Run a step of that state's routine
    4. Repeat

*/

/* Actuators */
Servo servo_1(ARM_SERVO_1_ANGLE_MAX);
Servo servo_2(ARM_SERVO_2_ANGLE_MAX);
Arm arm(&servo_1, &servo_2);
Servo claw(CLAW_OPEN);
Motor base_gear_motor(PIN_BASE_GEAR_PWM, PWM_CHANNEL_BASE_GEAR, PWM_FRQ_HZ_BASE_GEAR, PWM_RESOLUTION_BASE_GEAR, PIN_BASE_GEAR_DIR);
BaseGear base_gear = BaseGear(&base_gear_motor);
Motor left_motor(PIN_MOTOR_LEFT_PWM, PWM_CHANNEL_MOTOR_LEFT, PWM_FRQ_HZ_MOTOR_LEFT, PWM_RESOLUTION_MOTOR_LEFT, PIN_MOTOR_LEFT_DIR);
Motor right_motor(PIN_MOTOR_RIGHT_PWM, PWM_CHANNEL_MOTOR_RIGHT, PWM_FRQ_HZ_MOTOR_RIGHT, PWM_RESOLUTION_MOTOR_RIGHT, PIN_MOTOR_RIGHT_DIR);
Motor cascade_motor(PIN_CASCADE_PWM, PWM_CHANNEL_CASCADE, PWM_FRQ_HZ_CASCADE, PWM_RESOLUTION_CASCADE, PIN_CASCADE_DIR);

/* Sensors */
ToF tof_claw;
ToF tof_chassis;
ToF_data tof_data_claw;
ToF_data tof_data_chassis;
volatile bool switch_triggered = false;
volatile bool ramp_detected = false;

/* STATE TRANSITIONS */
struct state_transition {
    state_e previous_state;
    state_event_e event;
    state_e next_state;
};

// consult the diagram to understand these transitions
static const struct state_transition state_transitions[] = {
    // {STATE_TAPE_FOLLOWING, EVENT_PET_DETECTED_LEFT, STATE_WAIT},
    // {STATE_TAPE_FOLLOWING, EVENT_PET_DETECTED_RIGHT, STATE_WAIT},
    // {STATE_TAPE_FOLLOWING, EVENT_PILLAR_DETECTED, STATE_WAIT},
    // {STATE_WAIT, EVENT_PET_GRASPED, STATE_TAPE_FOLLOWING},
    {STATE_TAPE_FOLLOWING, EVENT_EDGE_DETECTED, STATE_WAIT},
    // {   STATE_TAPE_FOLLOWING,   EVENT_PET_DETECTED_LEFT,    STATE_REACH             },
    // {   STATE_TAPE_FOLLOWING,   EVENT_PET_DETECTED_RIGHT,   STATE_REACH             },
    // {   STATE_TAPE_FOLLOWING,   EVENT_PILLAR_DETECTED,      STATE_REACH             },
    // {   STATE_REACH,            EVENT_PET_NEAR,             STATE_CLOSE_CLAW        },
    // {   STATE_CLOSE_CLAW,       EVENT_PET_MISSED,           STATE_RETREAT           },
    // {   STATE_RETREAT,          EVENT_ARM_READY,            STATE_REACH             },
    // {   STATE_RETREAT,          EVENT_PET_FAILED,           STATE_TAPE_FOLLOWING    },
    // {   STATE_CLOSE_CLAW,       EVENT_FIRST_PET,            STATE_TAPE_FOLLOWING    },
    // {   STATE_TAPE_FOLLOWING,   EVENT_RAMP,                 STATE_DROP_OFF          },
    // {   STATE_CLOSE_CLAW,       EVENT_PET_GRASPED,          STATE_STORE             },
    // {   STATE_STORE,            EVENT_PET_STORED,           STATE_TAPE_FOLLOWING    },
    // {   STATE_TAPE_FOLLOWING,   EVENT_EDGE_DETECTED,        STATE_RETURN_PETS       },
    // {   STATE_RETURN_PETS,      EVENT_PETS_RETURNED,        STATE_WAIT              },
};


/* FUNCTIONS */
static void state_enter (struct state_machine *state_machine, state_e next_state, state_event_e event);
static void state_exit  (struct state_machine *state_machine, state_e previous_state);

bool IRAM_ATTR timer_isr_callback (void *args) {
    ramp_detected = true;
}

void state_machine_init(struct state_machine *state_machine) {
    
    // set up state machine data
    state_machine->state = STATE_TAPE_FOLLOWING;
    state_machine->internal_event = EVENT_NONE;

    state_machine->stable_pet_count_left    = 0;
    state_machine->stable_pet_count_right   = 0;
    state_machine->stable_pillar_count      = 0;

    state_machine->pets_stored              = 0;
    state_machine->arm_in_start_pos         = true;
    state_machine->claw_closed              = false;
    state_machine->claw_open_angle          = CLAW_OPEN;
    state_machine->attempts                 = 0;

    state_machine->last_error               = 0;
    state_machine->last_pid_time            = 0;
    state_machine->last_ir_ll               = false;
    state_machine->last_ir_l                = false;
    state_machine->last_ir_c                = false;
    state_machine->last_ir_r                = false;
    state_machine->last_ir_rr               = false;

    timer_config_t timer_config = {
        .alarm_en = TIMER_ALARM_EN,
        .counter_en = TIMER_PAUSE,
        .counter_dir = TIMER_COUNT_UP,
        .auto_reload = TIMER_AUTORELOAD_DIS,
        .divider = 16,
    };
    timer_init(TIMER_GROUP_0, TIMER_0, &timer_config);
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, RAMP_TIME_S * TIMER_BASE_CLK / 16);
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_isr_callback_add(TIMER_GROUP_0, TIMER_0, timer_isr_callback, 0, 0);

    #ifdef DEBUG
    // set up display for debugging
    display_handler.begin(SSD1306_SWITCHCAPVCC, 0x3C); // 3.3V at the default i2c addr
    display_handler.setTextSize(1);
    display_handler.setTextColor(SSD1306_WHITE);
    display_handler.clearDisplay();
    display_handler.setCursor(0, 0);
    display_handler.println("Initializing State Machine...");
    display_handler.display();
    Serial.println("Initializing State Machine...");
    #endif

    // set up I2C
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    Wire.setClock(I2C_FRQ_HZ);

    // set up ToF
    // tof_setup(&tof_claw, TOF_CHANNEL_CLAW);
    // tof_setup(&tof_chassis, TOF_CHANNEL_CHASSIS);

    // set up sonar
    sonar_setup();

    // set up servos
    arm.setup();
    claw.attach(PIN_SERVO_3, PWM_CHANNEL_SERVO_3);

    // set up IR
    pinMode(PIN_IR_SENSOR_LL, INPUT);
    pinMode(PIN_IR_SENSOR_L, INPUT);
    pinMode(PIN_IR_SENSOR_C, INPUT);
    pinMode(PIN_IR_SENSOR_R, INPUT);
    pinMode(PIN_IR_SENSOR_RR, INPUT);

    // set up motors
    left_motor.setup();
    right_motor.setup();
    cascade_motor.setup();
    // base_gear.setup();      // sets up the motor and magnetic encoder internally

    // set up claw switch
    pinMode(PIN_LIMIT_SWITCH, INPUT_PULLUP);

    #ifdef DEBUG
    print_event(state_machine->internal_event);
    print_state(state_machine->state);
    #endif

}

state_event_e process_tof_inputs(struct state_machine *state_machine) {

    float mean_distance_mm;

    if (tof_claw.isDataReady() && tof_get_data(&tof_claw, TOF_CHANNEL_CLAW, &tof_data_claw)) {

        mean_distance_mm = tof_get_left_center_dist(&tof_data_claw);

        if (
            mean_distance_mm >= TOF_CENTER_DIST_LOWER_THRESHOLD_MM && 
            mean_distance_mm <= TOF_CENTER_DIST_UPPER_THRESHOLD_MM &&
            tof_left_cylinder_detected(&tof_data_claw)
        ) {
            if (tof_get_center_reflectance(&tof_data_claw) <= TOF_REFLECTANCE_THRESHOLD) {
                // state_machine->stable_pillar_count++;
                // state_machine->stable_pet_count_left = 0;
                // if (state_machine->stable_pillar_count >= 2) {
                //     state_machine->stable_pillar_count = 0;
                    return EVENT_PILLAR_DETECTED;
                // }
            } else {
                // state_machine->stable_pet_count_left++;
                // state_machine->stable_pillar_count = 0;
                // if (state_machine->stable_pet_count_left >= 2) {
                //     state_machine->stable_pet_count_left = 0;
                    return EVENT_PET_DETECTED_LEFT;
                // }
            }
        } else {
            // state_machine->stable_pet_count_left = 0;
            // state_machine->stable_pillar_count = 0;
        }
    } else {
        // state_machine->stable_pet_count_left = 0;
        // state_machine->stable_pillar_count = 0;
    }

    if (tof_chassis.isDataReady() && tof_get_data(&tof_chassis, TOF_CHANNEL_CHASSIS, &tof_data_chassis)) {

        mean_distance_mm = tof_get_right_center_dist(&tof_data_chassis);

        if (
            mean_distance_mm >= TOF_CENTER_DIST_LOWER_THRESHOLD_MM &&
            mean_distance_mm <= TOF_CENTER_DIST_UPPER_THRESHOLD_MM &&
            tof_right_cylinder_detected(&tof_data_chassis)
        ) {
            // state_machine->stable_pet_count_right++;
            // if (state_machine->stable_pet_count_right >= 2) {
            //     state_machine->stable_pet_count_right = 0;
                return EVENT_PET_DETECTED_RIGHT;
            // }
        } else {
            // state_machine->stable_pet_count_right = 0;
        }
    } else {
        // state_machine->stable_pet_count_right = 0;
    }

    return EVENT_NONE;
    
}

state_event_e process_input(struct state_machine *state_machine) {
    
    // internal events take precedence
    if (state_machine->internal_event != EVENT_NONE) {
        state_event_e ie = state_machine->internal_event;
        state_machine->internal_event = EVENT_NONE;
        return ie;
    }

    if (ramp_detected) {
        ramp_detected = false;
        return EVENT_RAMP;
    }

    if (state_machine->claw_closed) {
        if (digitalRead(PIN_LIMIT_SWITCH) == SWITCH_CLOSED && !switch_triggered) {
            if (state_machine->pets_stored == 0) {
                timer_start(TIMER_GROUP_0, TIMER_0);
                // move the arm back home
                base_gear.write(BASE_GEAR_HOME);
                arm.move_to_pos(ARM_HOME_X, ARM_HOME_Y);
                return EVENT_FIRST_PET_GRASPED;
            } else {
                return EVENT_PET_GRASPED;
            }
            switch_triggered = true;
        } else if (digitalRead(PIN_LIMIT_SWITCH) == SWITCH_OPEN && switch_triggered) {
            return EVENT_PET_MISSED;
            switch_triggered = false;
        }
    }
    
    // platform edge detection
    if (sonar_get_distance_cm() > SONAR_EDGE_DISTANCE_CM) {
        return EVENT_EDGE_DETECTED;
    }

    // both tof inputs, EVENT_NONE is returned if neither sees anything
    // return process_tof_inputs(state_machine);
    return EVENT_NONE;
}

void process_event(struct state_machine *state_machine, state_event_e next_event) {
    
    if (next_event == EVENT_NONE) {
        state_enter(state_machine, state_machine->state, EVENT_NONE); // stay in the same state
    }

    for (int i = 0 ; i < ARRAY_SIZE(state_transitions); i ++) {
        if(state_machine->state == state_transitions[i].previous_state && next_event == state_transitions[i].event) {
            
            #ifdef DEBUG
            print_event(next_event);
            print_state(state_transitions[i].next_state);
            #endif
            
            state_exit(state_machine, state_transitions[i].previous_state);
            state_enter(state_machine, state_transitions[i].next_state, next_event);
            return;
        }
    }

    // if the current state + event is not recognized as a valid transition, stay in the same state
    state_enter(state_machine, state_machine->state, EVENT_NONE); // stay in the same state

}

static void state_enter(struct state_machine *state_machine, state_e next_state, state_event_e event) {
    
    state_machine->state = next_state;

    switch (next_state) {
        case STATE_WAIT:
            state_wait_enter(state_machine);
            break;
        case STATE_TAPE_FOLLOWING:
            state_tape_following_enter(state_machine, event);
            break;
        case STATE_REACH:
            state_reach_enter(state_machine, event);
            break;
        case STATE_CLOSE_CLAW:
            state_close_claw_enter(state_machine);
            break;
        case STATE_STORE:
            state_store_enter(state_machine);
            break;
        case STATE_RETREAT:
            state_retreat_enter(state_machine);
            break;
        case STATE_RETURN_PETS:
            state_return_pets_enter(state_machine);
            break;
        case STATE_DROP_OFF:
            state_drop_off_enter(state_machine);
            break;
        default:
            break;
    }
}

static void state_exit(struct state_machine *state_machine, state_e previous_state) {

    switch (previous_state) {
        case STATE_TAPE_FOLLOWING:
            state_tape_following_exit(state_machine);
            break;
        case STATE_WAIT:
        case STATE_REACH:
        case STATE_CLOSE_CLAW:
        case STATE_STORE:
        case STATE_RETREAT:
        case STATE_RETURN_PETS:
        case STATE_DROP_OFF:
        default:
            break;
    }

}

#ifdef DEBUG

Adafruit_SSD1306 display_handler(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void print_state(state_e state) {
    display_handler.print("STATE: ");
    Serial.print("STATE: ");
    switch(state) {
        case STATE_WAIT: 
            display_handler.println("WAIT");
            Serial.println("WAIT");
            break;
        case STATE_TAPE_FOLLOWING:
            display_handler.println("TAPE FOLLOWING");
            Serial.println("TAPE FOLLOWING");
            break;
        case STATE_REACH:
            display_handler.println("REACH");
            Serial.println("REACH");
            break;
        case STATE_CLOSE_CLAW:
            display_handler.println("CLOSE ARM");
            Serial.println("CLOSE ARM");
            break;
        case STATE_STORE:
            display_handler.println("STORE");
            Serial.println("STORE");
            break;
        case STATE_DROP_OFF:
            display_handler.println("DROP OFF");
            Serial.println("DROP OFF");
            break;
        case STATE_RETREAT:
            display_handler.println("RETREAT");
            Serial.println("RETREAT");
            break;
        case STATE_RETURN_PETS:
            display_handler.println("RETURN PETS");
            Serial.println("RETURN PETS");
            break;
        default:
            break;
    }
    display_handler.display();
}

void print_event(state_event_e event) { 
    display_handler.clearDisplay();
    display_handler.setCursor(0, 0);
    display_handler.print("EVENT: ");
    Serial.print("EVENT: ");
    switch (event) {
        case EVENT_NONE:
            display_handler.println("NONE");
            Serial.println("NONE");
            break;
        case EVENT_PET_DETECTED_LEFT:
            display_handler.println("PET DETECTED LEFT");
            Serial.println("TAPE DETECTED");
            break;
        case EVENT_PET_DETECTED_RIGHT:
            display_handler.println("PET DETECTED RIGHT");
            Serial.println("PET DETECTED RIGHT");
            break;
        case EVENT_PILLAR_DETECTED:
            display_handler.println("PILLAR DETECTED");
            Serial.println("PILLAR DETECTED");
            break;
        case EVENT_PET_NEAR:
            display_handler.println("NEAR PET");
            Serial.println("NEAR PET");
            break;
        case EVENT_PET_MISSED:
            display_handler.println("PET MISSED");
            Serial.println("PET MISSED");
            break;
        case EVENT_ARM_READY:
            display_handler.println("ARM READY");
            Serial.println("ARM READY");
            break;
        case EVENT_PET_FAILED:
            display_handler.println("PET FAILED");
            Serial.println("PET FAILED");
            break;
        case EVENT_PET_GRASPED:
            display_handler.println("PET GRASPED");
            Serial.println("PET GRASPED");
            break;
        case EVENT_PET_STORED:
            display_handler.println("PET STORED");
            Serial.println("PET STORED");
            break;
        case EVENT_EDGE_DETECTED:
            display_handler.println("EDGE DETECTED");
            Serial.println("EDGE DETECTED");
            break;
        case EVENT_PETS_RETURNED:
            display_handler.println("PETS RETURNED");
            Serial.println("PETS RETURNED");
            break;
        case EVENT_RAMP:
            display_handler.println("RAMP DETECTED");
            Serial.println("RAMP DETECTED");
            break;
        case EVENT_FIRST_PET_GRASPED:
            display_handler.println("1st PET GRASPED");
            Serial.println("1st PET GRASPED");
            break;
        default:
            break;
    }
    display_handler.display();
}

#endif
