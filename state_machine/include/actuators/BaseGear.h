#ifndef BASE_GEAR_H
#define BASE_GEAR_H

#define BASE_GEAR_TURNING_TIME_MS 1000

#define BASE_GEAR_STORE     185
#define BASE_GEAR_HOME      270
#define BASE_GEAR_LEFT      270
#define BASE_GEAR_FORWARD   0
#define BASE_GEAR_RIGHT     90

class BaseGear 
{
    public:
        BaseGear();
        ~BaseGear();
        void setup();
        void write(int angle);
    private:
};

#endif