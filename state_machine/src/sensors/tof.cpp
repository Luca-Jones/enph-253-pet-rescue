#include <sensors/ToF.h>
#include <math.h>

void tof_setup(ToF *tof, uint8_t i2c_addr) {
    tof->begin();
    tof->setAddress(i2c_addr);
    tof->setResolution(TOF_RESOLUTION);
    tof->setRangingFrequency(TOF_RANGING_FREQUENCY_HZ);
    tof->startRanging();
}

float tof_get_center_dist(const ToF_data *data) {

     /**
          0  1  2  3  4  5  6  7 
      0 .[ ][ ][ ][ ][ ][ ][ ][ ]
      1 .[ ][ ][ ][ ][ ][ ][ ][ ]
      2 .[ ][ ][ ][ ][ ][ ][ ][ ]
      3 .[ ][ ][ ][ ][ ][ ][ ][ ]
      4 .[ ][ ][ ][*][*][ ][ ][ ]
      5 .[ ][ ][ ][*][*][ ][ ][ ]
      6 .[ ][ ][ ][*][*][ ][ ][ ]
      7 .[ ][ ][ ][ ][ ][ ][ ][ ]
    **/

    return (
        data->distance_mm[8 * (7 - 4) + 3] + 
        data->distance_mm[8 * (7 - 4) + 3] + 
        data->distance_mm[8 * (7 - 5) + 3] + 
        data->distance_mm[8 * (7 - 5) + 4] + 
        data->distance_mm[8 * (7 - 6) + 4] + 
        data->distance_mm[8 * (7 - 6) + 4]
    ) * 0.167f;
}

float tof_get_center_reflectance(const ToF_data *data) {
    
     /**
          0  1  2  3  4  5  6  7 
      0 .[ ][ ][ ][ ][ ][ ][ ][ ]
      1 .[ ][ ][ ][ ][ ][ ][ ][ ]
      2 .[ ][ ][ ][ ][ ][ ][ ][ ]
      3 .[ ][ ][ ][ ][ ][ ][ ][ ]
      4 .[ ][ ][ ][*][*][ ][ ][ ]
      5 .[ ][ ][ ][*][*][ ][ ][ ]
      6 .[ ][ ][ ][*][*][ ][ ][ ]
      7 .[ ][ ][ ][ ][ ][ ][ ][ ]
    **/

    return (
        data->reflectance[8 * (7 - 4) + 3] + 
        data->reflectance[8 * (7 - 4) + 3] + 
        data->reflectance[8 * (7 - 5) + 3] + 
        data->reflectance[8 * (7 - 5) + 4] + 
        data->reflectance[8 * (7 - 6) + 4] + 
        data->reflectance[8 * (7 - 6) + 4]
    ) * 0.167f;

}

bool tof_cylindrical_object_detected(const ToF_data *data) {

    /**
          0  1  2  3  4  5  6  7 
      0 .[ ][ ][ ][ ][ ][ ][ ][ ]
      1 .[ ][ ][ ][ ][ ][ ][ ][ ]
      2 .[ ][ ][ ][ ][ ][ ][ ][ ]
      3 .[ ][ ][*][*][*][*][ ][ ]
      4 .[ ][ ][*][*][*][*][ ][ ]
      5 .[ ][ ][*][*][*][*][ ][ ]
      6 .[ ][ ][ ][ ][ ][ ][ ][ ]
      7 .[ ][ ][ ][ ][ ][ ][ ][ ]
    **/

    // key: [8 * row + col]
    float center_left_sum =  data->distance_mm[8 * (7 - 3) + 3] + data->distance_mm[8 * (7 - 4) + 3] + data->distance_mm[8 * (7 - 5) + 3];
    float center_right_sum = data->distance_mm[8 * (7 - 3) + 4] + data->distance_mm[8 * (7 - 4) + 4] + data->distance_mm[8 * (7 - 5) + 4];
    float center_mean = 0.167f * (center_left_sum + center_right_sum);
    float center_mean_diff = 0.333f * fabs(center_left_sum - center_right_sum);

    float side_left_sum =  data->distance_mm[8 * (7 - 3) + 2] + data->distance_mm[8 * (7 - 4) + 2] + data->distance_mm[8 * (7 - 5) + 2];
    float side_right_sum = data->distance_mm[8 * (7 - 3) + 5] + data->distance_mm[8 * (7 - 4) + 5] + data->distance_mm[8 * (7 - 5) + 5];
    float side_mean = 0.167f * (side_left_sum + side_right_sum);
    float side_mean_diff = 0.333f * fabs(side_left_sum - side_right_sum);

    float top_center_mean = 0.5f * (data->distance_mm[8 * (7 - 0) + 3] + data->distance_mm[8 * (7 - 0) + 4]);

    return (side_mean_diff <= TOF_MAX_SIDE_DIFF_MM && center_mean < side_mean && top_center_mean >= TOF_MAX_TOP_MM);
}
