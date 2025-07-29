#include <Arduino.h>
#include <sensors/ToF.h>
#include <config/pin_out.h>

ToF tof_claw;
ToF tof_chassis;

ToF_data tof_data_claw;
ToF_data tof_data_chassis;

int stablePillarCount = 0;
int stablePetLCount = 0;
int stablePetRCount = 0;

void read_sensor(ToF *sensor, uint8_t mask, ToF_data *result) {
    if (sensor->isDataReady()) {
        if (tof_get_data(sensor, mask, result)) {

            float meanDistance = tof_get_center_dist(result);

            if (meanDistance >= 100.0f && meanDistance <= 240.0f) {
                if(mask == TOF_CHANNEL_CLAW && tof_left_cylinder_detected(result)) {
                    

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
                } else if (mask == TOF_CHANNEL_CHASSIS && tof_right_cylinder_detected(result)) {
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
}

void loop() {
    read_sensor(&tof_claw, TOF_CHANNEL_CLAW, &tof_data_claw);
    delay(67);
    read_sensor(&tof_chassis, TOF_CHANNEL_CHASSIS, &tof_data_chassis);
    delay(67);
}