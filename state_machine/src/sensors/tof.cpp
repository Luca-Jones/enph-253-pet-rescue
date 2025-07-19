#include <tof.h>
#include <math.h>

float tof_get_center_dist(const ToF_data *data) {

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
        data->distance_mm[8 * 3 + 3] + 
        data->distance_mm[8 * 4 + 3] + 
        data->distance_mm[8 * 5 + 3] + 
        data->distance_mm[8 * 3 + 4] + 
        data->distance_mm[8 * 4 + 4] + 
        data->distance_mm[8 * 5 + 4]
    ) * 0.16f;
}

bool tof_pillar_detected(const ToF_data *data) {
    
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

    float mean_reflectance = (
        data->reflectance[8 * 3 + 3] + 
        data->reflectance[8 * 4 + 3] + 
        data->reflectance[8 * 5 + 3] + 
        data->reflectance[8 * 3 + 4] + 
        data->reflectance[8 * 4 + 4] + 
        data->reflectance[8 * 5 + 4]
    ) * 0.16f; // floating point multiplication is faster than division

    return mean_reflectance <= TOF_REFLECTANCE_THRESHOLD;
}

/* Requires that the mean center distance be between 100mm and 240mm */
bool tof_pet_detected(const ToF_data *data) {

    /**
          0  1  2  3  4  5  6  7 
      0 .[ ][ ][ ][*][ ][ ][ ][ ]
      1 .[ ][ ][ ][ ][ ][ ][ ][ ]
      2 .[ ][ ][ ][ ][ ][ ][ ][ ]
      3 .[ ][ ][*][*][*][*][ ][ ]
      4 .[ ][ ][*][*][*][*][ ][ ]
      5 .[ ][ ][ ][*][*][ ][ ][ ]
      6 .[ ][ ][ ][ ][ ][ ][ ][ ]
      7 .[ ][ ][ ][ ][ ][ ][ ][ ]
    **/

    // key: [8 * row + col]
    float center_left_sum =  data->distance_mm[8 * 3 + 3] + data->distance_mm[8 * 4 + 3] + data->distance_mm[8 * 5 + 3];
    float center_right_sum = data->distance_mm[8 * 3 + 4] + data->distance_mm[8 * 4 + 4] + data->distance_mm[8 * 5 + 4];
    float center_mean_diff = 0.33f * fabs(center_left_sum - center_right_sum);

    float side_left_sum =  data->distance_mm[8 * 3 + 2] + data->distance_mm[8 * 4 + 2];
    float side_right_sum = data->distance_mm[8 * 3 + 5] + data->distance_mm[8 * 4 + 5];
    float side_mean_diff = 0.5f * fabs(side_left_sum - side_right_sum);

    float top_center = data->distance_mm[8 * 0 + 3];

    Serial.printf("side = %.2f\t center = %.2f\t top = %.2f\n", side_mean_diff, center_mean_diff, top_center);

    return (side_mean_diff <= TOF_MAX_SIDE_DIFF_MM && center_mean_diff < side_mean_diff );//&& top_center >= TOF_MAX_TOP_MM);
}
