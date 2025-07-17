#ifndef CLAW_H
#define CLAW_H

#include <Servo.h>
#include <esp32-hal.h>

#define CLAW_OPEN 180
#define CLAW_SEMI_OPEN 160
#define CLAW_CLOSED 100


class Claw
{
private:
    Servo *servo;
public:
    Claw(Servo *servo);
    ~Claw();
    void open();
    void close();
};

#endif