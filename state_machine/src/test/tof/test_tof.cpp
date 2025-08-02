#include <Arduino.h>
#include <sensors/ToF.h>
#include <actuators/Arm.h>
#include <config/pwm_config.h>
#include <config/pin_out.h>

ToF tof_claw;
ToF tof_chassis;

ToF_data tof_data_claw;
ToF_data tof_data_chassis;

Servo servo_1(ARM_SERVO_1_ANGLE_MAX);
Servo servo_2(ARM_SERVO_2_ANGLE_MAX);
Arm arm(&servo_1, &servo_2);

int stablePillarCount = 0;
int stablePetLCount = 0;
int stablePetRCount = 0;

void read_sensor(ToF *sensor, uint8_t mask, ToF_data *result) {
    if (sensor->isDataReady() && tof_get_data(sensor, mask, result)) {

        float meanDistance;

        if (mask == TOF_CHANNEL_CLAW) {
            meanDistance = tof_get_left_center_dist(result);
            if (meanDistance >= TOF_CENTER_DIST_LOWER_THRESHOLD_MM && meanDistance <= TOF_CENTER_DIST_UPPER_THRESHOLD_MM) {
                if (tof_left_cylinder_detected(result)) {
                    if (tof_get_center_reflectance(result) <= 10.0f) {
                        stablePillarCount++;
                        stablePetLCount = 0;
                        Serial.println("Pillar detected");
                        if (stablePillarCount == 2) {
                            Serial.println("*** Confirmed Pillar! ***");
                            stablePillarCount = 0;
                        }
                    } else {
                        stablePetLCount++;
                        stablePillarCount = 0;
                        Serial.println("Pet on left");
                    
                        if (stablePetLCount == 2) {
                            Serial.println("*** Confirmed Pet on left! ***");
                            stablePetLCount = 0;
                        }
                    }
                } else {
                    Serial.println("Not centered");
                    stablePetLCount = 0;
                    stablePetRCount = 0;
                    stablePillarCount = 0;
                }
            } else {
                Serial.println("Too far or too close...");
                stablePetLCount = 0;
                stablePetRCount = 0;
                stablePillarCount = 0;
            }

        } else if (mask == TOF_CHANNEL_CHASSIS) {
            meanDistance = tof_get_right_center_dist(result);

            if (meanDistance >= TOF_CENTER_DIST_LOWER_THRESHOLD_MM && meanDistance <= TOF_CENTER_DIST_UPPER_THRESHOLD_MM) {
                if (tof_right_cylinder_detected(result)) {
                    stablePetRCount++;
                    Serial.println("Pet on right");

                    if (stablePetRCount == 2) {
                        Serial.println("*** Confirmed Pet on right! ***");
                    }
                } else {
                    Serial.println("Not centered");
                    stablePetLCount = 0;
                    stablePetRCount = 0;
                    stablePillarCount = 0;
                }
            } else {
                // Serial.println("Too far or too close...");
                stablePetLCount = 0;
                stablePetRCount = 0;
                stablePillarCount = 0;
            }
        }
    }
}

void setup() {
    Serial.begin(115200);
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    Wire.setClock(I2C_FRQ_HZ);
    tof_setup(&tof_claw, TOF_CHANNEL_CLAW);
    tof_setup(&tof_chassis, TOF_CHANNEL_CHASSIS);
    servo_1.attach(PIN_SERVO_1, PWM_CHANNEL_SERVO_1, 500, 2500);
    servo_2.attach(PIN_SERVO_2, PWM_CHANNEL_SERVO_2, 500, 2500);
    arm.move_to_pos(ARM_HOME_X, ARM_HOME_Y);
}

void loop() {
    read_sensor(&tof_claw, TOF_CHANNEL_CLAW, &tof_data_claw);
    delay(67);
    read_sensor(&tof_chassis, TOF_CHANNEL_CHASSIS, &tof_data_chassis);
    delay(67);
}