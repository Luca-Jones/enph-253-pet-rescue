#include <Arduino.h>
#include <sensors/MagneticEncoder.h>
#include <config/pin_out.h>
#include <Wire.h>

void setup() {
    Serial.begin(115200);
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    Wire.setClock(I2C_FRQ_HZ);
}

void loop() {
    int status = mag_get_status();
    switch (status) {
    case MAG_STATUS_OK:
        Serial.println("Magnet detected!");
        break;
    case MAG_STATUS_TOO_CLOSE:
        Serial.println("Magnet too close!");
        break;
    case MAG_STATUS_TOO_FAR:
        Serial.println("Magnet too far!");
        break;
    default:
        break;
    }
    int angle = mag_get_angle();
    Serial.printf("angle = %d\n\n", angle);
    delay(300);
}