#include <Arduino.h>
#include <Wire.h>
#include <actuators/BaseGear.h>
#include <sensors/MagneticEncoder.h>
#include <config/pin_out.h>

BaseGear base_gear = BaseGear();

void setup() {
    Serial.begin(115200);
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    Wire.setClock(I2C_FRQ_HZ);
}

void loop() {
    
    base_gear.write(0);
    if (mag_get_status() == MAG_STATUS_OK) Serial.printf("angle = %d\n", mag_get_angle());
    delay(1000);

    base_gear.write(90);
    if (mag_get_status() == MAG_STATUS_OK) Serial.printf("angle = %d\n", mag_get_angle());
    delay(1000);
    
    base_gear.write(180);
    if (mag_get_status() == MAG_STATUS_OK) Serial.printf("angle = %d\n", mag_get_angle());
    delay(1000);
    
    base_gear.write(270);
    if (mag_get_status() == MAG_STATUS_OK) Serial.printf("angle = %d\n", mag_get_angle());
    delay(1000);
}