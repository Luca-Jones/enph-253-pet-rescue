#include <sensors/MagneticEncoder.h>
#include <Wire.h>

int mag_get_status() {

    Wire.beginTransmission(MAG_I2C_ADDRESS);
    Wire.write(MAG_REGISTER_STATUS);
    Wire.endTransmission();
    Wire.requestFrom(MAG_I2C_ADDRESS, 1); // request 1 byte
    while (Wire.available() == 0);

    int mag_status = Wire.read();
    if (mag_status & MAG_MD_BIT) {
        return MAG_STATUS_OK;
    } else if (mag_status & MAG_ML_BIT) {
        return MAG_STATUS_TOO_FAR;
    } else if (mag_status & MAG_MH_BIT) {
        return MAG_STATUS_TOO_CLOSE;
    }

    return MAG_STATUS_ERROR;
}

void mag_setup() {
    // any calibration needed goes here
}

int mag_get_angle() {

    Wire.beginTransmission(MAG_I2C_ADDRESS);
    Wire.write(MAG_REGISTER_RAW_ANGLE_LOW);
    Wire.endTransmission();
    Wire.requestFrom(MAG_I2C_ADDRESS, 1);  // request 1 byte
    while (Wire.available() == 0); // wait until there is data to receive
    
    int low_byte = Wire.read();

    Wire.beginTransmission(MAG_I2C_ADDRESS);
    Wire.write(MAG_REGISTER_RAW_ANGLE_HIGH);
    Wire.endTransmission();
    Wire.requestFrom(MAG_I2C_ADDRESS, 1);
    while (Wire.available() == 0);

    int high_byte = Wire.read();

    int raw_angle = (high_byte << 8) | low_byte;
    return raw_angle * MAG_RAW_TO_ANGLE;
}