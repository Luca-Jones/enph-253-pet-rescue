#ifndef TOF_H
#define TOF_H

#define TOF_CLOCK_FREQUENCY 400000
#define TOF_I2C_ADDRESS 0x29
#define TOF_RESOLUTION 8*8
#define TOF_RANGING_FREQUENCY 15

typedef SparkFun_VL53L5CX ToF;
typedef VL53L5CX_ResultsData ToF_data;

bool tof_pillar_detected(const ToF_data *data);
bool tof_pet_detected(const ToF_data *data);
bool tof_near_pet(const ToF_data *data);

#endif