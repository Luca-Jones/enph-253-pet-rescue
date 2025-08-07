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
Servo claw(180);
Motor base_gear_motor(PIN_BASE_GEAR_PWM, PWM_CHANNEL_BASE_GEAR, PWM_FRQ_HZ_BASE_GEAR, PWM_RESOLUTION_BASE_GEAR, PIN_BASE_GEAR_DIR);
BaseGear base_gear = BaseGear(&base_gear_motor);
Motor left_motor(PIN_MOTOR_LEFT_PWM, PWM_CHANNEL_MOTOR_LEFT, PWM_FRQ_HZ_MOTOR_LEFT, PWM_RESOLUTION_MOTOR_LEFT, PIN_MOTOR_LEFT_DIR);
Motor right_motor(PIN_MOTOR_RIGHT_PWM, PWM_CHANNEL_MOTOR_RIGHT, PWM_FRQ_HZ_MOTOR_RIGHT, PWM_RESOLUTION_MOTOR_RIGHT, PIN_MOTOR_RIGHT_DIR);
Motor cascade_motor(PIN_CASCADE_PWM, PWM_CHANNEL_CASCADE, PWM_FRQ_HZ_CASCADE, PWM_RESOLUTION_CASCADE, PIN_CASCADE_DIR);
#define TAPE_FOLLOWING_DEFAULT_SPEED 60

/* Sensors */
SemaphoreHandle_t i2c_mutex;
volatile state_event_e tof_reading;
// SemaphoreHandle_t tof_reading_mutex;
TaskHandle_t tof_task_handle = NULL;
TaskHandle_t dist_task_handle = NULL;
ToF tof_claw;
ToF tof_chassis;
ToF_data tof_data_claw;
ToF_data tof_data_chassis;
volatile bool switch_triggered = false;
volatile bool ramp_detected = false;
volatile int tape_following_base_speed;
volatile int last_tape_following_base_speed;

/* STATE TRANSITIONS */
struct state_transition {
    state_e previous_state;
    state_event_e event;
    state_e next_state;
};

// consult the diagram to understand these transitions
static const struct state_transition state_transitions[] = {
    
    {   STATE_TAPE_FOLLOWING,   EVENT_PET_DETECTED_LEFT,    STATE_REACH             },
    {   STATE_TAPE_FOLLOWING,   EVENT_PILLAR_DETECTED,      STATE_REACH             },
    {   STATE_TAPE_FOLLOWING,   EVENT_PET_DETECTED_RIGHT,   STATE_REACH             },
    {   STATE_REACH,            EVENT_PET_NEAR,             STATE_CLOSE_CLAW        },
    {   STATE_CLOSE_CLAW,       EVENT_PET_GRASPED,          STATE_STORE             },
    {   STATE_CLOSE_CLAW,       EVENT_FIRST_PET_GRASPED,    STATE_TAPE_FOLLOWING    },
    {   STATE_TAPE_FOLLOWING,   EVENT_RAMP,                 STATE_DROP_OFF          },
    {   STATE_DROP_OFF,         EVENT_PET_STORED,           STATE_TAPE_FOLLOWING    },
    {   STATE_STORE,            EVENT_PET_STORED,           STATE_TAPE_FOLLOWING    },
    {   STATE_TAPE_FOLLOWING,   EVENT_EDGE_DETECTED,        STATE_RETURN_PETS       },
    {   STATE_RETURN_PETS,      EVENT_PETS_RETURNED,        STATE_WAIT              },
};


/* FUNCTIONS */
static void state_enter (struct state_machine *state_machine, state_e next_state, state_event_e event);
static void state_exit  (struct state_machine *state_machine, state_e previous_state);

bool IRAM_ATTR timer_isr_callback (void *args) {
    ramp_detected = true;
    return true;
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
    state_machine->no_ir_counter            = 0;

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

    // set up I2C
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    Wire.setClock(I2C_FRQ_HZ);

    i2c_mutex = xSemaphoreCreateMutex();
    if (i2c_mutex == NULL) {
        #ifdef DEBUG
        Serial.println("Failed to create I2C mutex!");
        #endif
        return; // Exit initialization if mutex creation fails
    } else {
        #ifdef DEBUG
        Serial.println("I2C mutex created sucessfully!");
        #endif
    }
    
    tof_reading = EVENT_NONE;
    
    // set up ToF FIRST (before creating tasks that use them)
    #ifdef DEBUG
    Serial.println("Setting up ToF claw sensor...");
    #endif
    tof_setup(&tof_claw, TOF_CHANNEL_CLAW);
    
    #ifdef DEBUG
    Serial.println("Setting up ToF chassis sensor...");
    #endif
    tof_setup(&tof_chassis, TOF_CHANNEL_CHASSIS);

    BaseType_t dist_task_result = xTaskCreatePinnedToCore(
        dist_input_task,
        "dist Task",
        12288,                 // Increased stack size from 8192 to 12288 (12KB)
        NULL,
        1,
        &dist_task_handle,
        0
    );

    if (dist_task_result == pdPASS && dist_task_handle != NULL) {
        // Wait for task to actually start before suspending
        vTaskDelay(pdMS_TO_TICKS(50)); 
        vTaskSuspend(dist_task_handle);
        vTaskDelay(pdMS_TO_TICKS(10)); // Give time for suspension to take effect
        #ifdef DEBUG
        Serial.println("Distance task created successfully");
        #endif
        
        // Check suspension multiple times if needed
        int attempts = 0;
        while (eTaskGetState(dist_task_handle) != eSuspended && attempts < 5) {
            vTaskSuspend(dist_task_handle);
            vTaskDelay(pdMS_TO_TICKS(10));
            attempts++;
        }
        
        if (eTaskGetState(dist_task_handle) == eSuspended) {
            Serial.printf("dist task suspended after %d attempts.", attempts);
        } else {
            Serial.printf("dist task failed to suspend after %d attempts... state: %d\n", attempts, eTaskGetState(dist_task_handle));
        }
    } else {
        #ifdef DEBUG
        Serial.println("Failed to create distance task!");
        #endif
    }

    BaseType_t tof_task_result = xTaskCreatePinnedToCore(
        tof_input_task,        // Function
        "ToF Task",            // Name
        12288,                 // Increased stack size from 8192 to 12288 (12KB)
        NULL,                  // Parameters
        1,                     // Priority
        &tof_task_handle,      // Task handle
        0                      // Core 0
    );

    if (tof_task_result == pdPASS && tof_task_handle != NULL) {
        #ifdef DEBUG
        Serial.println("ToF task created successfully");
        #endif
    } else {
        #ifdef DEBUG
        Serial.println("Failed to create ToF task!");
        #endif
    }

    // set up sonar
    #ifdef DEBUG
    Serial.println("Setting up sonar...");
    #endif
    sonar_setup();

    // set up servos
    #ifdef DEBUG
    Serial.println("Setting up arm...");
    #endif
    arm.setup();
    
    #ifdef DEBUG
    Serial.println("Setting up claw servo...");
    #endif
    claw.attach(PIN_SERVO_3, PWM_CHANNEL_SERVO_3, 500, 2500);

    // set up IR
    pinMode(PIN_IR_SENSOR_LL, INPUT);
    pinMode(PIN_IR_SENSOR_L, INPUT);
    pinMode(PIN_IR_SENSOR_C, INPUT);
    pinMode(PIN_IR_SENSOR_R, INPUT);
    pinMode(PIN_IR_SENSOR_RR, INPUT);

    tape_following_base_speed = TAPE_FOLLOWING_DEFAULT_SPEED;
    last_tape_following_base_speed = TAPE_FOLLOWING_DEFAULT_SPEED;

    // set up motors
    left_motor.setup();
    right_motor.setup();
    cascade_motor.setup();
    base_gear.setup();      // sets up the motor and magnetic encoder internally

    #ifdef DEBUG
    print_event(state_machine->internal_event);
    print_state(state_machine->state);
    #endif

}

void tof_input_task(void *pvParameters) {

    // Wait a bit to ensure everything is initialized
    vTaskDelay(pdMS_TO_TICKS(6500));

    float mean_distance_mm;
    float distMap[8][8];
    int close_iterations = 0;

    for (;;) {

        // Monitor stack usage to detect potential overflow (ALWAYS enabled for crash debugging)
        UBaseType_t stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        if (stackHighWaterMark < 1024) {  // Less than 1024 bytes free
            #ifdef DEBUG
            Serial.printf("CRITICAL: ToF task low stack: %d bytes free\n", stackHighWaterMark);
            #endif
        }

        if (i2c_mutex != NULL && xSemaphoreTake(i2c_mutex, pdMS_TO_TICKS(500)) == pdTRUE) {
            
            if (tof_get_data(&tof_claw, TOF_CHANNEL_CLAW, &tof_data_claw)) {
                
                // Serial.println("Claw grid:");
                // for (int row = 0; row < 8; row++) {
                //     for (int col = 0; col < 8; col++) {
                //         int i = row * 8 + col;
                //         distMap[7 - row][col] = tof_data_claw.distance_mm[i]; // vertically flipped
                //     }
                // }

                // for (int row = 0; row < 8; row ++) {
                //     for (int col = 0; col < 8; col++) {
                //         Serial.printf("%.0f ", distMap[row][col]);
                //     }
                //     Serial.println("");
                // }
                // Serial.println("");

                mean_distance_mm = tof_get_left_center_dist(&tof_data_claw);
                
                if (tof_left_something_ahead(&tof_data_claw)) {
                    #ifdef DEBUG
                    Serial.println("Something ahead!");
                    #endif
                    tape_following_base_speed = 20;
                }

                if (tape_following_base_speed == 20) {
                    close_iterations++;
                    if (close_iterations >= 50) {
                        tape_following_base_speed = TAPE_FOLLOWING_DEFAULT_SPEED;
                        close_iterations = 0;
                    }
                }

                if (
                    mean_distance_mm >= TOF_CENTER_DIST_LOWER_THRESHOLD_MM && 
                    mean_distance_mm <= TOF_CENTER_DIST_UPPER_THRESHOLD_MM
                ) {

                    if (tof_get_center_reflectance(&tof_data_claw) <= 10.0f) {

                        tof_reading = EVENT_PILLAR_DETECTED;
                        // Don't break - suspend this task instead
                        #ifdef DEBUG
                        Serial.println("pillar detected! ToF task suspended!");
                        #endif

                        xSemaphoreGive(i2c_mutex);
                        vTaskSuspend(NULL);
                    } else if (tof_left_cylinder_detected(&tof_data_claw)) {

                        tof_reading = EVENT_PET_DETECTED_LEFT;
                        // Don't break - suspend this task instead
                        #ifdef DEBUG
                        Serial.println("pet detected left! ToF task suspended!");
                        #endif  
                        
                        xSemaphoreGive(i2c_mutex);
                        vTaskSuspend(NULL);
                    }
                }
            }

            xSemaphoreGive(i2c_mutex);
        } else {
            // Mutex timeout - continue to avoid deadlock
            #ifdef DEBUG
            Serial.println("ToF task: I2C mutex timeout, continuing...");
            #endif
        }

        if (i2c_mutex != NULL && xSemaphoreTake(i2c_mutex, pdMS_TO_TICKS(500)) == pdTRUE) {
            if (tof_get_data(&tof_chassis, TOF_CHANNEL_CHASSIS, &tof_data_chassis)) {
                mean_distance_mm = tof_get_right_center_dist(&tof_data_chassis);

                Serial.println("Chassis grid:");
                for (int row = 0; row < 8; row++) {
                    for (int col = 0; col < 8; col++) {
                        int i = row * 8 + col;
                        distMap[7 - row][7 - col] = tof_data_chassis.distance_mm[i];
                    }
                }

                for (int row = 0; row < 8; row ++) {
                    for (int col = 0; col < 8; col++) {
                        Serial.printf("%.0f ", distMap[row][col]);
                    }
                    Serial.println("");
                }
                Serial.println("");

                if (tof_right_something_ahead(&tof_data_chassis)) {
                    #ifdef DEBUG
                    Serial.println("Something ahead!");
                    #endif
                    tape_following_base_speed = 20;
                }

                if (tape_following_base_speed == 20) {
                    close_iterations++;
                    if (close_iterations >= 50) {
                        tape_following_base_speed = TAPE_FOLLOWING_DEFAULT_SPEED;
                        close_iterations = 0;
                    }
                }
                
                if (
                    mean_distance_mm >= 50 &&
                    mean_distance_mm <= 240
                ) {
                    // tape_following_base_speed = 20;
                    if (tof_right_cylinder_detected(&tof_data_chassis)) {
                        tof_reading = EVENT_PET_DETECTED_RIGHT;
                        // Don't break - suspend this task instead
                        #ifdef DEBUG
                        Serial.println("pet detected right! ToF task suspended!");
                        #endif
                        
                        xSemaphoreGive(i2c_mutex);
                        vTaskSuspend(NULL);
                    }
                }
            }

            xSemaphoreGive(i2c_mutex);

        } else {
            // Mutex timeout - continue to avoid deadlock  
            #ifdef DEBUG
            Serial.println("ToF task: I2C mutex timeout on chassis read, continuing...");
            #endif
        }        
    }

}

void dist_input_task(void *pvParamters) {

    // Wait a bit to ensure everything is initialized
    vTaskDelay(pdMS_TO_TICKS(1000)); // gives me time to suspend the task before it aquires the i2c_mutex
    
    for (;;) {
        // Monitor stack usage (ALWAYS enabled for crash debugging)
        UBaseType_t stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        if (stackHighWaterMark < 1024) {  // Less than 1024 bytes free
            #ifdef DEBUG
            Serial.printf("CRITICAL: Dist task low stack: %d bytes free\n", stackHighWaterMark);
            #endif
        }

        if (i2c_mutex != NULL && xSemaphoreTake(i2c_mutex, pdMS_TO_TICKS(500)) == pdTRUE) {
            if (
               tof_get_data(&tof_claw, TOF_CHANNEL_CLAW, &tof_data_claw) && tof_get_dist_to_object(&tof_data_claw) <= 35  
            ) {

                tof_reading = EVENT_PET_NEAR;
                #ifdef DEBUG
                Serial.println("pet near the claw! dist task suspended!");
                #endif
                
                xSemaphoreGive(i2c_mutex);
                vTaskSuspend(NULL);
            }

            xSemaphoreGive(i2c_mutex);

        } else {
            // Mutex timeout - continue to avoid deadlock
            #ifdef DEBUG
            Serial.println("Dist task: I2C mutex timeout, continuing..."); 
            #endif
        }
        
    }
    
}

state_event_e process_input(struct state_machine *state_machine) {
    
    // internal events take precedence
    if (state_machine->internal_event != EVENT_NONE) {
        state_event_e ie = state_machine->internal_event;
        state_machine->internal_event = EVENT_NONE;
        return ie;
    }

    // ramp timer goes off
    if (ramp_detected) {
        ramp_detected = false;
        return EVENT_RAMP;
    }
    
    // platform edge detection
    int sonar_dist = sonar_get_distance_cm();
    if (sonar_dist > 30) {
        return EVENT_EDGE_DETECTED;
    }

    // poll the tof background task
    if (tof_reading != EVENT_NONE) {
        state_event_e event = tof_reading;
        tof_reading = EVENT_NONE;
        tape_following_base_speed = TAPE_FOLLOWING_DEFAULT_SPEED;
        last_tape_following_base_speed = TAPE_FOLLOWING_DEFAULT_SPEED;
        return event;
    }

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

void print_state(state_e state) {
    Serial.print("STATE: ");
    switch(state) {
        case STATE_WAIT: 
            Serial.println("WAIT");
            break;
        case STATE_TAPE_FOLLOWING:
            Serial.println("TAPE FOLLOWING");
            break;
        case STATE_REACH:
            Serial.println("REACH");
            break;
        case STATE_CLOSE_CLAW:
            Serial.println("CLOSE CLAW");
            break;
        case STATE_STORE:
            Serial.println("STORE");
            break;
        case STATE_DROP_OFF:
            Serial.println("DROP OFF");
            break;
        case STATE_RETREAT:
            Serial.println("RETREAT");
            break;
        case STATE_RETURN_PETS:
            Serial.println("RETURN PETS");
            break;
        default:
            break;
    }
}

void print_event(state_event_e event) { 
    
    Serial.print("EVENT: ");
    switch (event) {
        case EVENT_NONE:
            Serial.println("NONE");
            break;
        case EVENT_PET_DETECTED_LEFT:
            Serial.println("PET DETECTED LEFT");
            break;
        case EVENT_PET_DETECTED_RIGHT:
            Serial.println("PET DETECTED RIGHT");
            break;
        case EVENT_PILLAR_DETECTED:
            Serial.println("PILLAR DETECTED");
            break;
        case EVENT_PET_NEAR:
            Serial.println("NEAR PET");
            break;
        case EVENT_PET_MISSED:
            Serial.println("PET MISSED");
            break;
        case EVENT_ARM_READY:
            Serial.println("ARM READY");
            break;
        case EVENT_PET_FAILED:
            Serial.println("PET FAILED");
            break;
        case EVENT_PET_GRASPED:
            Serial.println("PET GRASPED");
            break;
        case EVENT_PET_STORED:
            Serial.println("PET STORED");
            break;
        case EVENT_EDGE_DETECTED:
            Serial.println("EDGE DETECTED");
            break;
        case EVENT_PETS_RETURNED:
            Serial.println("PETS RETURNED");
            break;
        case EVENT_RAMP:
            Serial.println("RAMP DETECTED");
            break;
        case EVENT_FIRST_PET_GRASPED:
            Serial.println("FIRST PET GRASPED");
            break;
        default:
            break;
    }
    
}

#endif
