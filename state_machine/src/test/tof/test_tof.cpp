#include <Arduino.h>
#include <sensors/ToF.h>
#include <config/pin_out.h>

ToF tof_claw;
ToF tof_chassis;

ToF_data tof_data_claw;
ToF_data tof_data_chassis;

void setup() {
    Serial.begin(115200);

    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    Wire.setClock(I2C_FRQ_HZ);

    tof_setup(&tof_claw, TOF_I2C_ADDRESS_CLAW);
    tof_setup(&tof_chassis, TOF_I2C_ADDRESS_CHASSIS);
}

void loop() {
    
    float dist_claw = tof_get_center_dist(&tof_data_claw);
    Serial.printf("center dist (claw) = %f\n", dist_claw);
    float dist_chassis = tof_get_center_dist(&tof_data_chassis);
    Serial.printf("center dist (chasis) = %f\n", dist_chassis);
    
    float refl_claw = tof_get_center_reflectance(&tof_data_claw);
    Serial.printf("relfectance (claw) = %f\n", refl_claw);
    float refl_chassis = tof_get_center_reflectance(&tof_data_chassis);
    Serial.printf("relfectance (chassis) = %f\n", refl_chassis);

    if (tof_cylindrical_object_detected(&tof_data_claw)) {
        Serial.println("cylindrical object detected at claw!");
    }

    if (tof_cylindrical_object_detected(&tof_data_chassis)) {
        Serial.println("cylindrical object detected at chassis!");
    }

}