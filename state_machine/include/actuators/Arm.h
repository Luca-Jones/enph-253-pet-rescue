#ifndef ARM_H
#define ARM_H

#include <actuators/Servo.h>

/* All angles are CW positive! */

#define ARM_SMALL_LENGTH 40
#define ARM_LARGE_LENGTH 180
#define CLAW_LENGTH 120

#define ARM_SERVO_1_ANGLE_MAX 270
#define ARM_SERVO_1_ANGLE_OFFSET -96

#define ARM_SERVO_2_ANGLE_MAX 270
#define ARM_SERVO_2_ANGLE_OFFSET 0

#define ARM_HOME_X 150
#define ARM_HOME_Y 50

#define ARM_PILLAR_X 190
#define ARM_PILLAR_Y 170

#define ARM_RAISED_X 190
#define ARM_RAISED_Y 250

#define ARM_ILLEGAL_POS -1

class Arm
{
private:
    Servo *servo_1;
    Servo *servo_2;
public:
    // note: servos need to be set up outside this class
    Arm(Servo *servo_1, Servo *servo_2); 
    ~Arm();
    void setup();
    void get_pos(int *x, int *y);
    int move_to_pos(int x, int y);                  // returns ARM_ILLEGAL_POS if the position is not possible to move to
    void lerp_to_pos(int x, int y, int time_ms);

    // TODO: remove testing functions
    void move_to_angle(int theta_1, int theta_2);
    void move_to_phi(float phi_1, float phi_2);
    void log_pos(int *theta_1, int *theta_2, float *phi_1, float *phi_2, int *x, int *y);
};


#endif