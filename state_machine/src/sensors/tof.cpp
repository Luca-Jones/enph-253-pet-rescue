#include <sensors/ToF.h>
#include <math.h>

void select_mux(uint8_t channel) {
    Wire.beginTransmission(MUX_I2C_ADDRESS);
    Wire.write(channel);
    Wire.endTransmission();
    delayMicroseconds(10);
}

void tof_setup(ToF *tof, uint8_t channel) {
    select_mux(channel);
    tof->begin();
    tof->setResolution(TOF_RESOLUTION);
    tof->setRangingFrequency(15);
    tof->startRanging();
}

bool tof_get_data(ToF *tof, uint8_t channel, ToF_data *data) {
    select_mux(channel);
    bool ret = tof->getRangingData(data);
    return ret;
}

float tof_get_dist_to_object(const ToF_data *data) {

     /**
          0  1  2  3  4  5  6  7 
      0 .[ ][ ][ ][ ][ ][ ][ ][ ]
      1 .[ ][ ][ ][ ][ ][ ][ ][ ]
      2 .[ ][ ][ ][ ][ ][ ][ ][ ]
      3 .[ ][ ][ ][*][*][ ][ ][ ]
      4 .[ ][ ][ ][*][*][ ][ ][ ]
      5 .[ ][ ][ ][ ][ ][ ][ ][ ]
      6 .[ ][ ][ ][ ][ ][ ][ ][ ]
      7 .[ ][ ][ ][ ][ ][ ][ ][ ]
    **/

    return (
        data->distance_mm[8 * (7 - 3) + 3] + 
        data->distance_mm[8 * (7 - 3) + 4] + 
        data->distance_mm[8 * (7 - 4) + 3] + 
        data->distance_mm[8 * (7 - 4) + 4]
    ) * 0.25f;
}

float tof_get_left_center_dist(const ToF_data *data) {
    /**
          0  1  2  3  4  5  6  7 
      0 .[ ][ ][ ][ ][ ][ ][ ][ ]
      1 .[ ][ ][ ][ ][ ][ ][ ][ ]
      2 .[ ][ ][ ][ ][ ][ ][ ][ ]
      3 .[ ][ ][ ][ ][ ][ ][ ][ ]
      4 .[ ][ ][ ][ ][ ][ ][ ][ ]
      5 .[ ][ ][ ][*][*][ ][ ][ ]
      6 .[ ][ ][ ][*][*][ ][ ][ ]
      7 .[ ][ ][ ][*][*][ ][ ][ ]
    **/

    return (
        data->distance_mm[8 * (7 - 5) + 3] + 
        data->distance_mm[8 * (7 - 5) + 4] + 
        data->distance_mm[8 * (7 - 6) + 3] + 
        data->distance_mm[8 * (7 - 6) + 4] +
        data->distance_mm[8 * (7 - 7) + 3] + 
        data->distance_mm[8 * (7 - 7) + 4]
    ) * 0.167f;
}

float tof_get_right_center_dist(const ToF_data *data) {

    /**
          0  1  2  3  4  5  6  7 
      0 .[ ][ ][ ][ ][ ][ ][ ][ ]
      1 .[ ][ ][ ][ ][ ][ ][ ][ ]
      2 .[ ][ ][ ][ ][ ][ ][ ][ ]
      3 .[ ][ ][ ][*][*][ ][ ][ ]
      4 .[ ][ ][ ][*][*][ ][ ][ ]
      5 .[ ][ ][ ][*][*][ ][ ][ ]
      6 .[ ][ ][ ][ ][ ][ ][ ][ ]
      7 .[ ][ ][ ][ ][ ][ ][ ][ ]
    **/

    return (
        data->distance_mm[8 * (7 - 2) + 3] + 
        data->distance_mm[8 * (7 - 3) + 4] + 
        data->distance_mm[8 * (7 - 4) + 3] + 
        data->distance_mm[8 * (7 - 4) + 4] +
        data->distance_mm[8 * (7 - 5) + 3] + 
        data->distance_mm[8 * (7 - 5) + 4]
    ) * 0.167f;
}

float tof_get_center_reflectance(const ToF_data *data) {
    
    /**
          0  1  2  3  4  5  6  7 
      0 .[ ][ ][ ][ ][ ][ ][ ][ ]
      1 .[ ][ ][ ][ ][ ][ ][ ][ ]
      2 .[ ][ ][ ][ ][ ][ ][ ][ ]
      3 .[ ][ ][ ][ ][ ][ ][ ][ ]
      4 .[ ][ ][ ][ ][ ][ ][ ][ ]
      5 .[ ][ ][ ][ ][ ][ ][ ][ ]
      6 .[ ][ ][ ][*][*][ ][ ][ ]
      7 .[ ][ ][ ][*][*][ ][ ][ ]
    **/

    return (
        // data->reflectance[8 * (7 - 5) + 3] + 
        // data->reflectance[8 * (7 - 5) + 4] + 
        data->reflectance[8 * (7 - 6) + 3] + 
        data->reflectance[8 * (7 - 6) + 4] +
        data->reflectance[8 * (7 - 7) + 3] + 
        data->reflectance[8 * (7 - 7) + 4] 
    ) * 0.167f;

}

bool tof_left_cylinder_detected(const ToF_data *data) {

    /**
          0  1  2  3  4  5  6  7 
      0 .[ ][ ][ ][*][*][ ][ ][ ]
      1 .[ ][ ][ ][ ][ ][ ][ ][ ]
      2 .[ ][ ][ ][ ][ ][ ][ ][ ]
      3 .[ ][ ][ ][ ][ ][ ][ ][ ]
      4 .[ ][ ][ ][ ][ ][ ][ ][ ]
      5 .[ ][ ][/][*][*][\][ ][ ]
      6 .[ ][ ][/][*][*][\][ ][ ]
      7 .[ ][ ][/][*][*][\][ ][ ]
    **/

    // key: [8 * row + col]
    float center_left_mean = (data->distance_mm[8 * (7 - 5) + 3] + data->distance_mm[8 * (7 - 6) + 3] + data->distance_mm[8 * (7 - 7) + 3]) * 0.33f;
    float center_right_mean = (data->distance_mm[8 * (7 - 5) + 4] + data->distance_mm[8 * (7 - 6) + 4] + data->distance_mm[8 * (7 - 7) + 4]) * 0.33f;
    float center_mean = 0.5f * (center_left_mean + center_right_mean);

    float side_left_mean = (data->distance_mm[8 * (7 - 5) + 2] + data->distance_mm[8 * (7 - 6) + 2] + data->distance_mm[8 * (7 - 7) + 2]) * 0.33f;
    float side_right_mean = (data->distance_mm[8 * (7 - 5) + 5] + data->distance_mm[8 * (7 - 6) + 5] + data->distance_mm[8 * (7 - 7) + 5]) * 0.33f;
    float side_mean = 0.5f * (side_left_mean + side_right_mean);
    float side_mean_diff = fabs(side_left_mean - side_right_mean);

    float top_center_mean = 0.5f * (data->distance_mm[8 * (7 - 0) + 3] + data->distance_mm[8 * (7 - 0) + 4]);

    return (side_mean_diff <= TOF_MAX_SIDE_DIFF_MM && center_mean < side_mean);// && top_center_mean >= TOF_MAX_TOP_MM);
}

bool tof_right_cylinder_detected(const ToF_data *data) {

    /**
          0  1  2  3  4  5  6  7 
      0 .[ ][ ][ ][ ][ ][ ][ ][ ]
      1 .[ ][ ][ ][ ][ ][ ][ ][ ]
      2 .[ ][ ][ ][ ][ ][ ][ ][ ]
      3 .[ ][ ][/][*][*][\][ ][ ]
      4 .[ ][ ][/][*][*][\][ ][ ]
      5 .[ ][ ][/][*][*][\][ ][ ]
      6 .[ ][ ][ ][ ][ ][ ][ ][ ]
      7 .[ ][ ][ ][ ][ ][ ][ ][ ]
    **/

    // key: [8 * row + col]
    float center_left_mean = (data->distance_mm[8 * (7 - 3) + 3] + data->distance_mm[8 * (7 - 4) + 3] + data->distance_mm[8 * (7 - 5) + 3]) * 0.33f;
    float center_right_mean = (data->distance_mm[8 * (7 - 3) + 4] + data->distance_mm[8 * (7 - 4) + 4] + data->distance_mm[8 * (7 - 5) + 4]) * 0.33f;
    float center_mean = 0.5f * (center_left_mean + center_right_mean);

    float side_left_mean = (data->distance_mm[8 * (7 - 3) + 2] + data->distance_mm[8 * (7 - 4) + 2] + data->distance_mm[8 * (7 - 5) + 2]) * 0.33f;
    float side_right_mean = (data->distance_mm[8 * (7 - 3) + 5] + data->distance_mm[8 * (7 - 4) + 5] + data->distance_mm[8 * (7 - 5) + 5]) * 0.33f;
    float side_mean = 0.5f * (side_left_mean + side_right_mean);
    float side_mean_diff = fabs(side_left_mean - side_right_mean);

    return (side_mean_diff <= TOF_MAX_SIDE_DIFF_MM && center_mean < side_mean);

}
