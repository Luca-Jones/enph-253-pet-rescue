#ifndef MAGNETIC_ENCODER_H
#define MAGNETIC_ENCODER_H

#define MAG_STATUS_OK           0
#define MAG_STATUS_TOO_CLOSE    1
#define MAG_STATUS_TOO_FAR      2

/**
 * @returns the status of the magnetic encoder
 */
int mag_get_status();


/**
 * @returns an angle from 0 to 360 degrees
 */
int mag_get_angle();

#endif