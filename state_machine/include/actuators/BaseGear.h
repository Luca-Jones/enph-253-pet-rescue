#ifndef BASE_GEAR_H
#define BASE_GEAR_H

#define BASE_GEAR_TURNING_TIME_MS 1000

#define BASE_GEAR_STORE     270
#define BASE_GEAR_HOME      180
#define BASE_GEAR_LEFT      180
#define BASE_GEAR_FORWARD   90
#define BASE_GEAR_RIGHT     0

class BaseGear 
{
    public:
        BaseGear();
        ~BaseGear();
        void write(int angle);
    private:
};

#endif