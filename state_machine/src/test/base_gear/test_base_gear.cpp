#include <Arduino.h>
#include <Wire.h>
#include <actuators/BaseGear.h>
#include <sensors/MagneticEncoder.h>
#include <config/pin_out.h>
#include <config/pwm_config.h>

Motor base_gear_motor(PIN_BASE_GEAR_PWM, PWM_CHANNEL_BASE_GEAR, PWM_FRQ_HZ_BASE_GEAR, PWM_RESOLUTION_BASE_GEAR, PIN_BASE_GEAR_DIR);
BaseGear base_gear = BaseGear(&base_gear_motor);

void setup() {
    Serial.begin(115200);
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    Wire.setClock(I2C_FRQ_HZ);
    base_gear.setup();
}

void loop() {
    
    Serial.println("target angle = 0");
    Serial.printf("current angle = %d\n", mag_get_angle());
    base_gear.write(0);
    Serial.printf("real angle = %d\n\n", mag_get_angle());
    delay(2000);

    Serial.println("target angle = 90");
    Serial.printf("current angle = %d\n", mag_get_angle());
    base_gear.write(90);
    Serial.printf("real angle = %d\n\n", mag_get_angle());
    delay(2000);
    
    Serial.println("target angle = 170");
    Serial.printf("current angle = %d\n", mag_get_angle());
    base_gear.write(170);
    Serial.printf("real angle = %d\n\n", mag_get_angle());
    delay(2000);
    
    Serial.println("target angle = 270");
    Serial.printf("current angle = %d\n", mag_get_angle());
    base_gear.write(270);
    Serial.printf("real angle = %d\n\n", mag_get_angle());
    delay(2000);
} 