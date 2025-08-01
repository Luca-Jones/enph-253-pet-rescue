#ifndef TOF_H
#define TOF_H

#include <SparkFun_VL53L5CX_Library.h>

#define TOF_I2C_ADDRESS             0x29
#define MUX_I2C_ADDRESS             0x70
#define TOF_CHANNEL_CHASSIS         0x02
#define TOF_CHANNEL_CLAW            0x10

#define TOF_RESOLUTION              8*8
#define TOF_RANGING_FREQUENCY_HZ    15

#define TOF_MAX_SIDE_DIFF_MM                30.0f
#define TOF_MAX_TOP_MM                      260.0f

#define TOF_CENTER_DIST_LOWER_THRESHOLD_MM  100.0f
#define TOF_CENTER_DIST_UPPER_THRESHOLD_MM  240.0f
#define TOF_REFLECTANCE_THRESHOLD           10.0f

typedef SparkFun_VL53L5CX ToF;
typedef VL53L5CX_ResultsData ToF_data;

void    tof_setup(ToF *tof, uint8_t mux_channel);
bool    tof_get_data(ToF *tof, uint8_t channel, ToF_data *data);
float   tof_get_dist_to_object(const ToF_data *data);
float   tof_get_center_dist(const ToF_data *data);
float   tof_get_center_reflectance(const ToF_data *data);
bool    tof_left_cylinder_detected(const ToF_data *data);
bool    tof_right_cylinder_detected(const ToF_data *data);

#endif