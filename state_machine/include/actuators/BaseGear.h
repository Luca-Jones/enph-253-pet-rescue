#ifndef BASE_GEAR_H
#define BASE_GEAR_H

#include <actuators/Motor.h>

#define BASE_GEAR_TURNING_TIME_MS 1000

#define BASE_GEAR_STORE     170
#define BASE_GEAR_HOME      273
#define BASE_GEAR_RIGHT     86

class BaseGear 
{
    public:
        BaseGear(Motor *motor);
        ~BaseGear();
        void setup();
        void write(int angle);
    private:
        Motor *motor;
};

#endif