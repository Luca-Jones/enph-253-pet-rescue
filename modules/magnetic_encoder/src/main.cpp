#include <Arduino.h>
#include <Wire.h>

#define MAG_PIN 25
int mag_reading = 0;
bool magnet_found = false;

float mag_get_angle() {

    // we want a 12 bit number, but we can only request 8 bits
    // at a time, so we grab 2 bytes from the right registers
    // and shift appropriately!

    Wire.beginTransmission(0x36);
    Wire.write(0x0D);           // acess RAW ANGLE register
    Wire.endTransmission();
    Wire.requestFrom(0x36, 1);  // request 1 byte

    while (Wire.available() == 0); // wait until there is data to receive
    int low_byte = Wire.read();

    Wire.beginTransmission(0x36);
    Wire.write(0x0C);
    Wire.endTransmission();
    Wire.requestFrom(0x36, 1);

    while (Wire.available() == 0);
    int high_byte = Wire.read();

    int raw_angle = (high_byte << 8) | low_byte;
    return raw_angle * 0.087890625f;

}

bool check_magnet() {
    Wire.beginTransmission(0x36);
    Wire.write(0x0B);
    Wire.endTransmission();
    Wire.requestFrom(0x36, 1);

    while (Wire.available() == 0);
    int mag_status = Wire.read();
    Serial.println(mag_status, BIN);
    if (mag_status & 0b00100000) {
        Serial.println("Magnet detected!");
    } else if (mag_status & 0b00010000) {
        Serial.println("Magnet too weak...");
    } else if (mag_status & 0b00001000) {
        Serial.println("Magnet too strong...");
    }
    return mag_status & 0b00100000;
}

void setup() {
    Serial.begin(115200);
    Wire.begin();
    Wire.setClock(800000L);
}

void loop() {
    // mag_reading = analogRead(MAG_PIN);
    magnet_found = check_magnet();
    if (magnet_found) {
        Serial.println(mag_get_angle());
    }
    delay(1000);
}

