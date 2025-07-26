#include <Arduino.h>
#include <Wire.h>
#include <actuators/BaseGear.h>
#include <sensors/MagneticEncoder.h>
#include <config/pin_out.h>
#include <config/pwm_config.h>

BaseGear base_gear = BaseGear();

void setup() {
    Serial.begin(115200);
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    Wire.setClock(I2C_FRQ_HZ);
    base_gear.setup();
}

void loop() {
    
    /* TEST DIRECTIONS */
    // Serial.println("LOW");
    
    // ledcWrite(PWM_CHANNEL_BASE_GEAR, 0);
    // delayMicroseconds(10);
    // digitalWrite(PIN_BASE_GEAR_DIR, LOW);
    // ledcWrite(PWM_CHANNEL_BASE_GEAR, 128);
    
    // delay(1000);

    // Serial.println("HIGH");

    // ledcWrite(PWM_CHANNEL_BASE_GEAR, 0);
    // delayMicroseconds(10);
    // digitalWrite(PIN_BASE_GEAR_DIR, HIGH);
    // ledcWrite(PWM_CHANNEL_BASE_GEAR, 128);

    // delay(1000);

    // Serial.println("target angle = 0");
    // Serial.printf("current angle = %d\n", mag_get_angle());
    // base_gear.write(0);
    // Serial.printf("real angle = %d\n\n", mag_get_angle());
    // delay(2000);
    
    base_gear.write(182);
    delay(1000);
    
    // Serial.println("target angle = 180");
    // Serial.printf("current angle = %d\n", mag_get_angle());
    // base_gear.write(180);
    // Serial.printf("real angle = %d\n\n", mag_get_angle());
    // delay(2000);
    
    // Serial.println("target angle = 270");
    // Serial.printf("current angle = %d\n", mag_get_angle());
    // base_gear.write(270);
    // Serial.printf("real angle = %d\n\n", mag_get_angle());
    // delay(2000);
} 