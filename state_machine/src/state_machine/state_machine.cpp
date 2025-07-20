#include <Arduino.h>
#include <assert.h>

#include <state_machine/state_machine.h>
#include <state_machine/state_close_claw.h>
#include <state_machine/state_raise_arm.h>
#include <state_machine/state_reach.h>
#include <state_machine/state_store.h>
#include <state_machine/state_wait.h>
// TODO: make the rest of the states

/*
    This is a state machine implemented with enums and functions.
    The robot moves between states through Transitions, which are triggered by Events.
    The event that ocurred is determined by the sensors.
    

    1. Process the sensor input and return the appropriate event
    2. Process the event and change the state accordingly
    3. Run a step of that state's routine
    4. Repeat

*/

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

/* Actuators */
Servo servo_1(ARM_SERVO_1_ANGLE_MAX);
Servo servo_2(ARM_SERVO_2_ANGLE_MAX);
Arm arm(&servo_1, &servo_2);
Servo claw(CLAW_OPEN);

/* Sensors */
ToF tof;
volatile bool button_pressed = false;
volatile bool switch_triggered = false;

/* STATE MACHINE */

struct state_transition {
    state_e previous_state;
    state_event_e event;
    state_e next_state;
};

// consult the diagram to understand these transitions
static const struct state_transition state_transitions[] = {
    // {STATE_WAIT, STATE_EVENT_BUTTON_PRESSED, STATE_STORE},
    // {   STATE_WAIT,             EVENT_PILLAR_DETECTED,        STATE_RAISE_ARM         },
    // {   STATE_RAISE_ARM,        EVENT_ARM_RAISED,             STATE_REACH             },
    {   STATE_WAIT,             EVENT_PET_DETECTED,           STATE_REACH             },
    // {   STATE_REACH,            EVENT_NEAR_PET,               STATE_CLOSE_CLAW        },
    // {   STATE_CLOSE_CLAW,       EVENT_PET_GRASPED,            STATE_STORE             },
    // {   STATE_STORE,            EVENT_PET_STORED,             STATE_WAIT              },
};


/* FUNCTIONS */
static void state_enter (struct state_machine *state_machine, state_e next_state);
static void state_exit  (struct state_machine *state_machine, state_e previous_state);

void state_machine_init(struct state_machine *state_machine) {
    
    // set up state machine data
    state_machine->state = STATE_WAIT;
    state_machine->internal_event = EVENT_NONE;
    state_machine->pets = 0;

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

    // set up servos
    servo_1.attach(PIN_SERVO_1, PWM_CHANNEL_SERVO_1, 500, 2500);
    servo_2.attach(PIN_SERVO_2, PWM_CHANNEL_SERVO_2, 500, 2500);
    claw.attach(PIN_SERVO_3, PWM_CHANNEL_SERVO_3, 500, 2500);

    // TODO: set up motors

    // set up start button
    attachInterrupt(digitalPinToInterrupt(PIN_START_BUTTON), button_pressed_ISR, CHANGE);
    
    // set up claw switch
    attachInterrupt(digitalPinToInterrupt(PIN_LIMIT_SWITCH), limit_switch_ISR, CHANGE);

    // set up I2C
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    Wire.setClock(I2C_FRQ_HZ);

    // set up ToF
    tof.begin();
    tof.setAddress(TOF_I2C_ADDRESS);
    tof.setResolution(TOF_RESOLUTION);
    tof.setRangingFrequency(TOF_RANGING_FREQUENCY_HZ);
    tof.startRanging();
    
    #ifdef DEBUG
    print_event(state_machine->internal_event);
    print_state(state_machine->state);
    #endif

    // TODO: set up magnetic encoder
    // TODO: set up rotary encoder
    // TODO: set up Sonar

}

state_event_e process_input(struct state_machine *state_machine) {
    
    // internal events take precedence
    if (state_machine->internal_event != EVENT_NONE) {
        state_event_e ie = state_machine->internal_event;
        state_machine->internal_event = EVENT_NONE;
        return ie;
    }

    // interupt updates are next
    if (button_pressed) {
        button_pressed = false;
        return EVENT_BUTTON_PRESSED;
    }

    if (switch_triggered) {
        switch_triggered = false;
        return EVENT_PET_GRASPED;
    }

    // TODO: poll input from each sensor
    // - ToF sensors (x2)
    // - sonar sensor
    // - rotary encoder
    // - megnetic encoder

    // if (tof.isDataReady()) // always seems to return false?  

    if (tof.getRangingData(&state_machine->tof_data)) {

        float center_dist = tof_get_center_dist(&state_machine->tof_data);
        Serial.println(center_dist);
        
        if (center_dist >= 100.0f && center_dist <= 240.0f) {
            
            if (tof_pillar_detected(&state_machine->tof_data)) {
                return EVENT_PILLAR_DETECTED;
            }
            
            if (tof_pet_detected(&state_machine->tof_data)) {
                return EVENT_PET_DETECTED;
            }
            
        } else if (center_dist < 100.0f && tof_pet_detected(&state_machine->tof_data)) {
            return EVENT_NEAR_PET;
        }

    }

    return EVENT_NONE;
}

void process_event(struct state_machine *state_machine, state_event_e next_event) {
    
    if (next_event == EVENT_NONE) {
        state_enter(state_machine, state_machine->state); // stay in the same state
    }

    for (int i = 0 ; i < ARRAY_SIZE(state_transitions); i ++) {
        if(state_machine->state == state_transitions[i].previous_state && next_event == state_transitions[i].event) {
            
            #ifdef DEBUG
            print_event(next_event);
            print_state(state_transitions[i].next_state);
            #endif
            
            state_exit(state_machine, state_transitions[i].previous_state);
            state_enter(state_machine, state_transitions[i].next_state);
            return;
        }
    }
}

static void state_enter(struct state_machine *state_machine, state_e next_state) {
    
    state_e previous_state = state_machine->state;
    state_machine->state = next_state;

    switch (next_state) {
    case STATE_WAIT:
        state_wait_enter(state_machine, previous_state);
        break;
    case STATE_REACH:
        state_reach_enter(state_machine, previous_state);
        break;
    case STATE_RAISE_ARM:
        state_raise_arm_enter(state_machine, previous_state);
        break;
    case STATE_CLOSE_CLAW:
        state_close_claw_enter(state_machine, previous_state);
        break;
    case STATE_STORE:
        state_store_enter(state_machine, previous_state);
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

void IRAM_ATTR button_pressed_ISR() {
    button_pressed = true;
}

void IRAM_ATTR limit_switch_ISR() {
    switch_triggered = gpio_get_level((gpio_num_t) PIN_LIMIT_SWITCH);
}

#ifdef DEBUG

Adafruit_SSD1306 display_handler(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void print_state(state_e state) {
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
    case STATE_RAISE_ARM:
        display_handler.println("RAISE ARM");
        Serial.println("RAISE ARM");
        break;
    case STATE_CLOSE_CLAW:
        display_handler.println("CLOSE ARM");
        Serial.println("CLOSE ARM");
        break;
    case STATE_STORE:
        display_handler.println("STORE");
        Serial.println("STORE");
        break;
    case STATE_RAMP:
        display_handler.println("RAMP");
        Serial.println("RAMP");
        break;
    case STATE_DEBRIS:
        display_handler.println("DEBRIS");
        Serial.println("DEBRIS");
        break;
    case STATE_TAPE_SWEEP:
        display_handler.println("TAPE SWEEP");
        Serial.println("TAPE SWEEP");
        break;
    case STATE_EXTEND_CASCADE:
        display_handler.println("CASCADE");
        Serial.println("CASCADE");
        break;
    case STATE_REVERSE:
        display_handler.println("REVERSE");
        Serial.println("REVERSE");
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
    case EVENT_NONE:
        display_handler.println("NONE");
        Serial.println("NONE");
        break;
    case EVENT_BUTTON_PRESSED:
        display_handler.println("BUTTON PRESSED");
        Serial.println("BUTTON PRESSED");
        break;
    case EVENT_TAPE_DETECTED:
        display_handler.println("TAPE DETECTED");
        Serial.println("TAPE DETECTED");
        break;
    case EVENT_PET_DETECTED:
        display_handler.println("PET DETECTED");
        Serial.println("PET DETECTED");
        break;
    case EVENT_PILLAR_DETECTED:
        display_handler.println("PILLAR DETECTED");
        Serial.println("PILLAR DETECTED");
        break;
    case EVENT_ARM_RAISED:
        display_handler.println("ARM RAISED");
        Serial.println("ARM RAISED");
        break;
   case EVENT_NEAR_PET:
        display_handler.println("NEAR PET");
        Serial.println("NEAR PET");
        break;
    case EVENT_PET_GRASPED:
        display_handler.println("PET GRASPED");
        Serial.println("PET GRASPED");
        break;
    case EVENT_PET_STORED:
        display_handler.println("PET STORED");
        Serial.println("PET STORED");
        break;
    case EVENT_RAMP_DETECTED:
        display_handler.println("RAMP DETECTED");
        Serial.println("RAMP DETECTED");
        break;
    case EVENT_DEBRIS_DETECTED:
        display_handler.println("DEBRIS DETECTED");
        Serial.println("DEBRIS DETECTED");
        break;
    case EVENT_FLAT_GROUND_DETECTED:
        display_handler.println("FLAT GROUND DETECTED");
        Serial.println("FLAT GROUND DETECTED");
        break;
    case EVENT_EDGE_DETECTED:
        display_handler.println("EDGE DETECTED");
        Serial.println("EDGE DETECTED");
        break;
    case EVENT_CASCADE_EXTENDED:
        display_handler.println("CASCADE EXTENDED");
        Serial.println("CASCADE EXTENDED");
        break;
    case EVENT_ZIP_LINE_DETECTED:
        display_handler.println("ZIP LINE");
        Serial.println("ZIP LINE");
        break;
    default:
        break;
    }
    display_handler.display();
}

#endif
