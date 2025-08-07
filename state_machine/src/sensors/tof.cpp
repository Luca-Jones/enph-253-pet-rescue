#include <sensors/ToF.h>
#include <math.h>

void select_mux(uint8_t channel) {
    Wire.beginTransmission(MUX_I2C_ADDRESS);
    Wire.write(channel);
    Wire.endTransmission();
    delayMicroseconds(100);
}

void tof_setup(ToF *tof, uint8_t channel) {
    select_mux(channel);
    tof->begin();
    // tof->setAddress(TOF_I2C_ADDRESS);
    tof->setResolution(TOF_RESOLUTION);
    tof->setRangingFrequency(TOF_RANGING_FREQUENCY_HZ);
    tof->startRanging();
}

bool tof_get_data(ToF *tof, uint8_t channel, ToF_data *data) {

    select_mux(channel);

    bool data_ready = tof->isDataReady();
    
    if (!data_ready) return false;

    unsigned long before_get_data = millis();
    bool ret = tof->getRangingData(data);
    unsigned long after_get_data = millis();
    
    #ifdef DEBUG
    // Serial.printf("Timing breakdown: get_data=%lu \n", after_get_data - before_get_data);
    #endif

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

    float center_mean = (
        data->distance_mm[8 * (7 - 3) + (7 - 3)] + 
        data->distance_mm[8 * (7 - 4) + (7 - 3)] + 
        data->distance_mm[8 * (7 - 3) + (7 - 4)] + 
        data->distance_mm[8 * (7 - 4) + (7 - 4)]
    ) / 4.0f;

    Serial.printf("dist = %.1f\n", center_mean);

    return center_mean;
}

float tof_get_left_center_dist(const ToF_data *data) {
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
        data->distance_mm[8 * (7 - 5) + 3] + 
        data->distance_mm[8 * (7 - 6) + 3] + 

        data->distance_mm[8 * (7 - 4) + 4] +
        data->distance_mm[8 * (7 - 5) + 4] + 
        data->distance_mm[8 * (7 - 6) + 4]
    ) / 6.0f;
}

float tof_get_right_center_dist(const ToF_data *data) {

    /**
          0  1  2  3  4  5  6  7 
      0 .[ ][ ][ ][ ][ ][ ][ ][ ]
      1 .[ ][ ][ ][ ][ ][ ][ ][ ]
      2 .[ ][ ][ ][*][*][ ][ ][ ]
      3 .[ ][ ][ ][*][*][ ][ ][ ]
      4 .[ ][ ][ ][*][*][ ][ ][ ]
      5 .[ ][ ][ ][ ][ ][ ][ ][ ]
      6 .[ ][ ][ ][ ][ ][ ][ ][ ]
      7 .[ ][ ][ ][ ][ ][ ][ ][ ]
    **/

    return ( 
        data->distance_mm[8 * (7 - 2) + 3] + 
        data->distance_mm[8 * (7 - 3) + 3] + 
        data->distance_mm[8 * (7 - 4) + 3] + 
        // data->distance_mm[8 * (7 - 5) + 3] + 
        // data->distance_mm[8 * (7 - 6) + 3] + 
        data->distance_mm[8 * (7 - 2) + 4] + 
        data->distance_mm[8 * (7 - 3) + 4] + 
        data->distance_mm[8 * (7 - 4) + 4]
        // data->distance_mm[8 * (7 - 5) + 4] + 
        // data->distance_mm[8 * (7 - 6) + 4]
    ) / 6.0f;
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
      6 .[ ][ ][ ][ ][ ][ ][ ][ ]
      7 .[ ][ ][ ][*][*][ ][ ][ ]
    **/

    return (
        // data->reflectance[8 * (7 - 5) + 3] + 
        // data->reflectance[8 * (7 - 6) + 3] + 
        data->reflectance[8 * (7 - 7) + 3] + 

        // data->reflectance[8 * (7 - 5) + 4] +
        // data->reflectance[8 * (7 - 6) + 4] + 
        data->reflectance[8 * (7 - 7) + 4]
    ) / 2.0f;

}

bool tof_right_something_ahead(const ToF_data *data) {

    /**
          0  1  2  3  4  5  6  7 
      0 .[ ][ ][ ][ ][ ][ ][ ][ ]
      1 .[ ][ ][ ][ ][ ][ ][ ][ ]
      2 .[*][+][ ][ ][ ][ ][x][x]
      3 .[*][+][ ][ ][ ][ ][x][x]
      4 .[*][+][ ][ ][ ][ ][x][x]
      5 .[ ][ ][ ][ ][ ][ ][ ][ ]
      6 .[ ][ ][ ][ ][ ][ ][ ][ ]
      7 .[ ][ ][ ][ ][ ][ ][ ][ ]
    **/

    float ahead_mean = (
                            data->distance_mm[8 * (7 - 2) + (7 - 0)] + 
                            data->distance_mm[8 * (7 - 3) + (7 - 0)] +
                            data->distance_mm[8 * (7 - 4) + (7 - 0)]
                        ) / 3.0f;

    float left_left_mean = (
                            data->distance_mm[8 * (7 - 2) + (7 - 1)] + 
                            data->distance_mm[8 * (7 - 3) + (7 - 1)] +
                            data->distance_mm[8 * (7 - 4) + (7 - 1)]
                        ) / 3.0f;
    
    float behind_mean = (
                            data->distance_mm[8 * (7 - 2) + (7 - 6)] + 
                            data->distance_mm[8 * (7 - 3) + (7 - 6)] +
                            data->distance_mm[8 * (7 - 4) + (7 - 6)] +
                            data->distance_mm[8 * (7 - 2) + (7 - 7)] + 
                            data->distance_mm[8 * (7 - 3) + (7 - 7)] +
                            data->distance_mm[8 * (7 - 4) + (7 - 7)]
                        ) / 6.0f;

    return ahead_mean > 50 && ahead_mean < 240 && behind_mean > ahead_mean + 100;

}

bool tof_left_something_ahead(const ToF_data *data) {
    
    /**
          0  1  2  3  4  5  6  7 
      0 .[ ][ ][ ][ ][ ][ ][ ][ ]
      1 .[ ][ ][ ][ ][ ][ ][ ][ ]
      2 .[ ][ ][ ][ ][ ][ ][ ][ ]
      3 .[ ][ ][ ][ ][ ][ ][ ][ ]
      4 .[x][x][ ][ ][ ][ ][ ][*]
      5 .[x][x][ ][ ][ ][ ][ ][*]
      6 .[x][x][ ][ ][ ][ ][ ][*]
      7 .[ ][ ][ ][ ][ ][ ][ ][ ]
    **/

    float ahead_mean = (
                            data->distance_mm[8 * (7 - 4) + (7 - 7)] + 
                            data->distance_mm[8 * (7 - 5) + (7 - 7)] +
                            data->distance_mm[8 * (7 - 6) + (7 - 7)]
                        ) / 3.0f;
    
    float behind_mean = (
                            data->distance_mm[8 * (7 - 4) + (7 - 0)] + 
                            data->distance_mm[8 * (7 - 5) + (7 - 0)] +
                            data->distance_mm[8 * (7 - 6) + (7 - 0)] +
                            data->distance_mm[8 * (7 - 4) + (7 - 1)] + 
                            data->distance_mm[8 * (7 - 5) + (7 - 1)] +
                            data->distance_mm[8 * (7 - 6) + (7 - 1)]
                        ) / 6.0f;
                            
    return ahead_mean > 50 && ahead_mean < 240 && behind_mean > ahead_mean + 100;

}

bool tof_right_cylinder_detected(const ToF_data *data) {

    /**
          0  1  2  3  4  5  6  7 
      0 .[ ][ ][ ][x][x][ ][ ][ ]
      1 .[ ][ ][ ][ ][ ][ ][ ][ ]
      2 .[/][-][+][*][*][+][-][\]
      3 .[/][-][+][*][*][+][-][\]
      4 .[ ][ ][ ][ ][ ][ ][ ][ ]
      5 .[ ][ ][ ][ ][ ][ ][ ][ ]
      6 .[ ][ ][ ][ ][ ][ ][ ][ ]
      7 .[ ][ ][ ][ ][ ][ ][ ][ ]
    **/

    // key: [8 * row + col]
    float center_mean = (   
                            data->distance_mm[8 * (7 - 2) + (7 - 3)] + 
                            data->distance_mm[8 * (7 - 3) + (7 - 3)] +
                            // data->distance_mm[8 * (7 - 4) + (7 - 3)] +
                            // data->distance_mm[8 * (7 - 5) + (7 - 3)] +
                            // data->distance_mm[8 * (7 - 6) + (7 - 3)] +
                            data->distance_mm[8 * (7 - 2) + (7 - 4)] + 
                            data->distance_mm[8 * (7 - 3) + (7 - 4)]
                            // data->distance_mm[8 * (7 - 4) + (7 - 4)] 
                            // data->distance_mm[8 * (7 - 5) + (7 - 4)] +
                            // data->distance_mm[8 * (7 - 6) + (7 - 4)]
                        ) / 4.0f;

    float far_left_mean = (    
                                data->distance_mm[8 * (7 - 2) + (7 - 0)] + 
                                data->distance_mm[8 * (7 - 3) + (7 - 0)]
                                // data->distance_mm[8 * (7 - 4) + (7 - 0)]
                                // data->distance_mm[8 * (7 - 5) + (7 - 0)] +
                                // data->distance_mm[8 * (7 - 6) + (7 - 0)]
                            ) / 2.0f;

    float far_right_mean = (   
                                data->distance_mm[8 * (7 - 2) + (7 - 7)] + 
                                data->distance_mm[8 * (7 - 3) + (7 - 7)]
                                // data->distance_mm[8 * (7 - 4) + (7 - 7)]
                                // data->distance_mm[8 * (7 - 5) + (7 - 7)] +
                                // data->distance_mm[8 * (7 - 6) + (7 - 7)]
                            ) / 2.0f;

    float left_mean = (    
                            data->distance_mm[8 * (7 - 2) + (7 - 2)] + 
                            data->distance_mm[8 * (7 - 3) + (7 - 2)]
                            // data->distance_mm[8 * (7 - 4) + (7 - 2)]
                            // data->distance_mm[8 * (7 - 5) + (7 - 2)] +
                            // data->distance_mm[8 * (7 - 6) + (7 - 2)]
                        ) / 2.0f;

    float right_mean = (        
                            data->distance_mm[8 * (7 - 2) + (7 - 5)] + 
                            data->distance_mm[8 * (7 - 3) + (7 - 5)]
                            // data->distance_mm[8 * (7 - 4) + (7 - 5)]
                            // data->distance_mm[8 * (7 - 5) + (7 - 5)] +
                            // data->distance_mm[8 * (7 - 6) + (7 - 5)]
                        ) / 2.0f;

    float left_left_mean = (    
                            data->distance_mm[8 * (7 - 2) + (7 - 1)] + 
                            data->distance_mm[8 * (7 - 3) + (7 - 1)]
                            // data->distance_mm[8 * (7 - 4) + (7 - 1)]
                            // data->distance_mm[8 * (7 - 5) + (7 - 1)] +
                            // data->distance_mm[8 * (7 - 6) + (7 - 1)]
                        ) / 2.0f;

    float right_right_mean = (        
                            data->distance_mm[8 * (7 - 2) + (7 - 6)] + 
                            data->distance_mm[8 * (7 - 3) + (7 - 6)]
                            // data->distance_mm[8 * (7 - 4) + (7 - 6)] 
                            // data->distance_mm[8 * (7 - 5) + (7 - 6)] +
                            // data->distance_mm[8 * (7 - 6) + (7 - 6)]
                        ) / 2.0f;
    
    float top_mean = (data->distance_mm[8 * (7 - 0) + 3] + data->distance_mm[8 * (7 - 0) + 4]) / 2.0f;




    Serial.printf("LLL = %0.1f, LL = %.1f, L = %.1f, C = %.1f, R = %.1f, RR = %.1f RRR = %0.1f             top = %.1f,\n", 
                far_left_mean, left_left_mean, left_mean, center_mean, right_mean, right_right_mean, far_right_mean,         top_mean);




    if (center_mean < 125.0f) {
        return (     
            right_right_mean                < 125 &&
            left_left_mean                  < 125 &&
            far_left_mean - center_mean     > 10  && 
            far_right_mean - center_mean    > 10
        );
    }

    if (center_mean < 140.0f) {
        return (
            far_right_mean - center_mean > 90.0f && 
            far_left_mean - center_mean > 90.0f 
            // && fabs(left_mean - right_mean) < 35.0f
        );
    }

    return (    
        far_right_mean - center_mean > 90.0f && 
        far_left_mean - center_mean > 90.0f && 
        (top_mean > center_mean + 50.0f)  // (top_mean < 200 || top_mean > center_mean + 100.0f) && 
        // && fabs(left_mean - right_mean) < 35.0f
    );

}


bool tof_left_cylinder_detected(const ToF_data *data) {

    /**
          0  1  2  3  4  5  6  7 
      0 .[ ][ ][ ][ ][ ][ ][ ][ ]
      1 .[ ][ ][ ][x][x][ ][ ][ ]
      2 .[ ][ ][ ][ ][ ][ ][ ][ ]
      3 .[ ][ ][ ][ ][ ][ ][ ][ ]
      4 .[/][-][+][*][*][+][-][\]
      5 .[/][-][+][*][*][+][-][\]
      6 .[ ][ ][ ][ ][ ][ ][ ][ ]
      7 .[ ][ ][ ][ ][ ][ ][ ][ ]
    **/

    // key: [8 * row + col]
    float center_mean = (   
                            data->distance_mm[8 * (7 - 4) + (7 - 3)] + 
                            data->distance_mm[8 * (7 - 5) + (7 - 3)] +
                            // data->distance_mm[8 * (7 - 6) + (7 - 3)] +
                            // data->distance_mm[8 * (7 - 5) + (7 - 3)] +
                            // data->distance_mm[8 * (7 - 6) + (7 - 3)] +
                            data->distance_mm[8 * (7 - 4) + (7 - 4)] + 
                            data->distance_mm[8 * (7 - 5) + (7 - 4)]
                            // data->distance_mm[8 * (7 - 6) + (7 - 4)] 
                            // data->distance_mm[8 * (7 - 5) + (7 - 4)] +
                            // data->distance_mm[8 * (7 - 6) + (7 - 4)]
                        ) / 4.0f;

    float far_left_mean = (    
                                data->distance_mm[8 * (7 - 4) + (7 - 0)] + 
                                data->distance_mm[8 * (7 - 5) + (7 - 0)]
                                // data->distance_mm[8 * (7 - 6) + (7 - 0)]
                                // data->distance_mm[8 * (7 - 5) + (7 - 0)] +
                                // data->distance_mm[8 * (7 - 6) + (7 - 0)]
                            ) / 2.0f;

    float far_right_mean = (   
                                data->distance_mm[8 * (7 - 4) + (7 - 7)] + 
                                data->distance_mm[8 * (7 - 5) + (7 - 7)]
                                // data->distance_mm[8 * (7 - 6) + (7 - 7)]
                                // data->distance_mm[8 * (7 - 5) + (7 - 7)] +
                                // data->distance_mm[8 * (7 - 6) + (7 - 7)]
                            ) / 2.0f;

    float left_mean = (    
                            data->distance_mm[8 * (7 - 4) + (7 - 2)] + 
                            data->distance_mm[8 * (7 - 5) + (7 - 2)]
                            // data->distance_mm[8 * (7 - 6) + (7 - 2)]
                            // data->distance_mm[8 * (7 - 5) + (7 - 2)] +
                            // data->distance_mm[8 * (7 - 6) + (7 - 2)]
                        ) / 2.0f;

    float right_mean = (        
                            data->distance_mm[8 * (7 - 4) + (7 - 5)] + 
                            data->distance_mm[8 * (7 - 5) + (7 - 5)]
                            // data->distance_mm[8 * (7 - 6) + (7 - 5)]
                            // data->distance_mm[8 * (7 - 5) + (7 - 5)] +
                            // data->distance_mm[8 * (7 - 6) + (7 - 5)]
                        ) / 2.0f;

    float left_left_mean = (    
                            data->distance_mm[8 * (7 - 4) + (7 - 1)] + 
                            data->distance_mm[8 * (7 - 5) + (7 - 1)]
                            // data->distance_mm[8 * (7 - 6) + (7 - 1)]
                            // data->distance_mm[8 * (7 - 5) + (7 - 1)] +
                            // data->distance_mm[8 * (7 - 6) + (7 - 1)]
                        ) / 2.0f;

    float right_right_mean = (        
                            data->distance_mm[8 * (7 - 4) + (7 - 6)] + 
                            data->distance_mm[8 * (7 - 5) + (7 - 6)] 
                            // data->distance_mm[8 * (7 - 6) + (7 - 6)] 
                            // data->distance_mm[8 * (7 - 5) + (7 - 6)] +
                            // data->distance_mm[8 * (7 - 6) + (7 - 6)]
                        ) / 2.0f;
    
    float top_mean = (data->distance_mm[8 * (7 - 1) + 3] + data->distance_mm[8 * (7 - 1) + 4]) / 2.0f;


    Serial.printf("LLL = %0.1f, LL = %.1f, L = %.1f, C = %.1f, R = %.1f, RR = %.1f RRR = %0.1f             top = %.1f,\n", 
                far_left_mean, left_left_mean, left_mean, center_mean, right_mean, right_right_mean, far_right_mean,         top_mean);


    if (center_mean < 125.0f) {
        return (     
            right_right_mean                < 125 &&
            left_left_mean                  < 125 &&
            far_left_mean - center_mean     > 10  && 
            far_right_mean - center_mean    > 10
        );
    }

    if (center_mean < 170.0f) {
        return (
            far_right_mean - center_mean > 90.0f && 
            far_left_mean - center_mean > 90.0f 
            // && fabs(left_mean - right_mean) < 35.0f
        );
    }

    return (    
        far_right_mean - center_mean > 90.0f && 
        far_left_mean - center_mean > 90.0f && 
        (top_mean > center_mean + 50.0f)  // (top_mean < 200 || top_mean > center_mean + 100.0f) && 
        // && fabs(left_mean - right_mean) < 35.0f
    );

}

