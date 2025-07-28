#include <actuators/Arm.h>
#include <esp32-hal.h>
#include <math.h>
#include <config/pin_out.h>
#include <config/pwm_config.h>

/*  
    x and y denote the tip of the claw.    
    The angles denoted theta are the angles of each servo.
    The angles denoted phi are the angles made with the horizontal 
    when looking at the claw as it faces to the right.
    The sign of theta is postive for servo_1 and counter-clockwise
    for servo_2. The phi angles are both positive counter-clockwise.
*/


/* Helper functions */
bool is_valid_pos(int x, int y);
void pos_to_angle(const int x, const int y, int *theta_1, int *theta_2);
void angle_to_pos(const int theta_1, const int theta_2, int *x, int *y);
void theta_to_phi(int theta_1, int theta_2, float *phi_1, float *phi_2);
void phi_to_theta(float phi_1, float phi_2, int *theta_1, int *theta_2);


Arm::Arm(Servo *servo_1, Servo *servo_2) {
    this->servo_1 = servo_1;
    this->servo_2 = servo_2;
}

Arm::~Arm() {
    this->servo_1->detach();
    this->servo_2->detach();
}

void Arm::setup() {
    servo_1->attach(PIN_SERVO_1, PWM_CHANNEL_SERVO_1);
    servo_2->attach(PIN_SERVO_2, PWM_CHANNEL_SERVO_2);
}

void Arm::get_pos(int *x, int *y) {
    
    // forwards kinematics
    int theta_1 = servo_1->read();
    int theta_2 = servo_2->read();

    float angle_hor_1, angle_hor_2;
    theta_to_phi(theta_1, theta_2, &angle_hor_1, &angle_hor_2);
    
    float psi = 90 - (angle_hor_2 - angle_hor_1) / 2;
    int r = 2 * ARM_LARGE_LENGTH * cos(M_PI * psi / 180);
    float gamma = angle_hor_1 - psi;

    *x = r * cos(M_PI * gamma / 180) + CLAW_LENGTH;
    *y = r * sin(M_PI * gamma / 180);

}

int Arm::move_to_pos(int x, int y) {
    int theta_1, theta_2;
    if (is_valid_pos(x,y)) {
        pos_to_angle(x, y, &theta_1, &theta_2);
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

// the valid region is a right semicircle offset to the right by the length of the claw
bool is_valid_pos(int x, int y) {
    return (
        x > CLAW_LENGTH && 
        (x - CLAW_LENGTH) * (x - CLAW_LENGTH) + y * y < 2 * ARM_LARGE_LENGTH * 2 * ARM_LARGE_LENGTH
    );
}

// inverse kinematics
void pos_to_angle(const int x, const int y, int *theta_1, int *theta_2) {

    float r = sqrt( (x - CLAW_LENGTH) * (x - CLAW_LENGTH) + y * y);
    float psi = acos(r / (2 * ARM_LARGE_LENGTH)) * 180 / M_PI;
    float gamma = atan((float) y / (x - CLAW_LENGTH)) * 180 / M_PI;

    float angle_hor_1 = psi + gamma;
    float angle_hor_2 = 2 * (90 - psi) + angle_hor_1;

    phi_to_theta(angle_hor_1, angle_hor_2, theta_1, theta_2);
}

// ground angles to servo angles
void phi_to_theta(float phi_1, float phi_2, int *theta_1, int *theta_2) {
    *theta_1 = 360 + ARM_SERVO_1_ANGLE_OFFSET - phi_1;
    *theta_2 = phi_2 - ARM_SERVO_2_ANGLE_OFFSET;
}

// forwards kinematics
void angle_to_pos(const int theta_1, const int theta_2, int *x, int *y) {
    
    float phi_1, phi_2;
    theta_to_phi(theta_1, theta_2, &phi_1, &phi_2);
    
    *x = ARM_LARGE_LENGTH * (cos(phi_1 * M_PI / 180) - cos(phi_2 * M_PI / 180)) + CLAW_LENGTH;
    *y = ARM_LARGE_LENGTH * (sin(phi_1 * M_PI / 180) - sin(phi_2 * M_PI / 180));
}

// servo angles to ground angles
void theta_to_phi(int theta_1, int theta_2, float *phi_1, float *phi_2) {
    *phi_1 = 360 + ARM_SERVO_1_ANGLE_OFFSET - theta_1;
    *phi_2 = theta_2 + ARM_SERVO_2_ANGLE_OFFSET;
}









// TODO: remove
/* FOR TESTING */
void Arm::move_to_angle(int theta_1, int theta_2) {
    servo_1->write(theta_1);
    servo_2->write(theta_2);
}

void Arm::move_to_phi(float phi_1, float phi_2) {
    int theta_1, theta_2;
    phi_to_theta(phi_1, phi_2, &theta_1, &theta_2);
    move_to_angle(theta_1, theta_2);
}

void Arm::log_pos(int *theta_1, int *theta_2, float *phi_1, float *phi_2, int *x, int *y) {
    *theta_1 = servo_1->read();
    *theta_2 = servo_2->read();
    theta_to_phi(*theta_1, *theta_2, phi_1, phi_2);
    angle_to_pos(*theta_1, *theta_2, x, y);
}
