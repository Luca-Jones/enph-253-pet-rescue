#include <Arm.h>
#include <math.h>
#include <Servo.h>

/*  
    x and y denote the tip of the claw    
    The angle is the angle of the servo. 
    CW is positive and there is an offset for each.
    In order for the servos to turn counter-clock-wise,
    the extreme position must be set to max angle not zero
*/
void pos_to_angle(const int x, const int y, int *theta_1, int *theta_2) {

    // inverse kinematics
    float r = sqrt( (x - CLAW_LENGTH) * (x - CLAW_LENGTH) + y * y);
    float psi = acos(r / (2 * ARM_LARGE_LENGTH)) * 180 / M_PI;
    float gamma = atan((float) y / (x - CLAW_LENGTH)) * 180 / M_PI;

    float angle_hor_1 = psi + gamma;
    float angle_hor_2 = 2 * (90 - psi) + angle_hor_1;

    *theta_1 = 360 - ARM_SERVO_1_ANGLE_OFFSET - angle_hor_1;
    *theta_2 = angle_hor_2 + ARM_SERVO_2_ANGLE_OFFSET;
}

/* Stays wihtin a right semicircle offset by the length of the claw */
bool is_valid_pos(int x, int y) {
    return (x > CLAW_LENGTH) && ((x - CLAW_LENGTH) * (x - CLAW_LENGTH) + y * y < 4 * ARM_LARGE_LENGTH * ARM_LARGE_LENGTH);
}

Arm::Arm(Servo *servo_1, Servo *servo_2) {
    this->servo_1 = servo_1;
    this->servo_2 = servo_2;
}

void Arm::setup() {
    servo_1->attach(ARM_SERVO_1_PIN, ARM_SERVO_1_CHANNEL, 500, 2500);
    servo_2->attach(ARM_SERVO_2_PIN, ARM_SERVO_2_CHANNEL, 500, 2500);
}

// for testing only really
void Arm::move_to_angle(int theta_1, int theta_2) {
    servo_1->write(theta_1);
    servo_2->write(theta_2);
}

int Arm::move_to_pos(int x, int y) {
    int theta_1 = 0;
    int theta_2 = 0;
    pos_to_angle(x, y, &theta_1, &theta_2);
    if (is_valid_pos(x,y)) {
        servo_1->write(theta_1);
        servo_2->write(theta_2);
        return 0;
    }
    return -1;
}

void Arm::lerp_to_pos(int x, int y, int time_ms) {
    int x_0,y_0;
    get_pos(&x_0, &y_0);

    for (float t = 0; t <= 1; t += 0.2) {
        move_to_pos(x_0 + (x - x_0) * t, y_0 + (y - y_0) * t);
        delay(time_ms / 5);
    }
}

void Arm::get_pos(int *x, int *y) {
    
    // forwards kinematics
    int theta_1 = servo_1->read();
    int theta_2 = servo_2->read();
    
    float angle_hor_1 = 360 - ARM_SERVO_1_ANGLE_OFFSET - theta_1;
    float angle_hor_2 = theta_2 - ARM_SERVO_2_ANGLE_OFFSET;
    
    float psi = 90 - (angle_hor_2 - angle_hor_1) / 2;
    int r = 2 * ARM_LARGE_LENGTH * cos(M_PI * psi / 180);
    float gamma = angle_hor_1 - psi;

    *x = r * cos(M_PI * gamma / 180) + CLAW_LENGTH;
    *y = r * sin(M_PI * gamma / 180);

}

Arm::~Arm(){}