#ifndef MAGNETIC_ENCODER_H
#define MAGNETIC_ENCODER_H

#define MAG_I2C_ADDRESS 0x36
#define MAG_REGISTER_STATUS 0x0B
#define MAG_MD_BIT 5
#define MAG_ML_BIT 4
#define MAG_MH_BIT 3
#define MAG_REGISTER_RAW_ANGLE_LOW  0x0D
#define MAG_REGISTER_RAW_ANGLE_HIGH 0x0C

#define MAG_RAW_TO_ANGLE 0.08789f

#define MAG_STATUS_OK           0
#define MAG_STATUS_TOO_CLOSE    1
#define MAG_STATUS_TOO_FAR      2
#define MAG_STATUS_ERROR        3

/**
 * @returns the status of the magnetic encoder
 */
int mag_get_status();


void mag_setup();


/**
 * @returns an angle from 0 to 360 degrees
 */
int mag_get_angle();

#endif