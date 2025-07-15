#ifndef ARM_H
#define ARM_H

#include <Servo.h>
#include <esp32-hal.h>

/* All angles are CW positive! */

#define ARM_SMALL_LENGTH 40
#define ARM_LARGE_LENGTH 180
#define CLAW_LENGTH 120

#define ARM_SERVO_1_PIN 19
#define ARM_SERVO_1_CHANNEL 1
#define ARM_SERVO_1_ANGLE_MAX 270
#define ARM_SERVO_1_ANGLE_OFFSET -90

#define ARM_SERVO_2_PIN 25
#define ARM_SERVO_2_CHANNEL 2
#define ARM_SERVO_2_ANGLE_MAX 270
#define ARM_SERVO_2_ANGLE_OFFSET 0


bool is_valid_pos(int x, int y);
void pos_to_angle(const int x, const int y, int *theta1, int *theta2); // theta1: big arm theta_2: little arm

class Arm
{
private:
    Servo *servo_1;
    Servo *servo_2;
public:
    Arm(Servo *servo_1, Servo *servo_2);
    ~Arm();
    void setup();
    int move_to_pos(int x, int y);                  // -1 is illegal 
    void move_to_angle(int theta_1, int theta_2);    // for testing
    void move_to_phi(float phi_1, float phi_2);          // for testing
    void log_pos(int *theta_1, int *theta_2, float *phi_1, float *phi_2, int *x, int *y);  // for testing
    void get_pos(int *x, int *y);
    void lerp_to_pos(int x, int y, int time_ms);
    // void move_by_vertical(const int dx);
    // void move_by_horizontal(const int dy);
};


#endif