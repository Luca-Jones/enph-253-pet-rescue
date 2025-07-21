#ifndef BASE_GEAR_H
#define BASE_GEAR_H

#define BASE_GEAR_TURNING_TIME_MS 1000

#define BASE_GEAR_STORE 270
#define BASE_GEAR_HOME 180

class BaseGear 
{
    public:
        BaseGear();
        ~BaseGear();
        void write(int angle);
    private:
};

#endif