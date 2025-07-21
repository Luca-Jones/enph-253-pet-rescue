#ifndef TOF_H
#define TOF_H

#include <SparkFun_VL53L5CX_Library.h>

#define TOF_I2C_ADDRESS_CLAW 0x29
#define TOF_I2C_ADDRESS_CHASSIS 0x2A
#define TOF_RESOLUTION 8*8
#define TOF_RANGING_FREQUENCY_HZ 8

#define TOF_MAX_SIDE_DIFF_MM            25.0f
#define TOF_MAX_TOP_MM                  200.0f
#define TOF_CENTER_DIST_LOWER_BOUND_MM  100.0f
#define TOF_CENTER_DIST_UPPER_BOUND_MM  100.0f
#define TOF_REFLECTANCE_THRESHOLD       10.0f

typedef SparkFun_VL53L5CX ToF;
typedef VL53L5CX_ResultsData ToF_data;

void tof_setup(ToF *tof, uint8_t i2c_addr);
float tof_get_center_dist(const ToF_data *data);
bool tof_pillar_detected(const ToF_data *data);
bool tof_pet_detected(const ToF_data *data);

#endif