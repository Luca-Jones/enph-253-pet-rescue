#include <actuators/Servo.h>
#include <esp32-hal-ledc.h>

/* These work on pretty much every servo */
#define SERVO_PWM_FREQUENCY     50
#define SERVO_PWM_PERIOD_US     20000       // 50 Hz
#define SERVO_PWM_RESOLUTION    12
#define SERVO_PWM_WIDTH         4096        // 2 ^ 12

Servo::Servo(int servo_max_angle) {
    this->servo_max_angle = servo_max_angle;
    this->is_attached = false;
}

void Servo::attach(int pin, int channel) {
    attach(pin, channel, SERVO_DEFAULT_MIN_US, SERVO_DEFAULT_MAX_US);
}

void Servo::attach(int pin, int channel, int min_us, int max_us) {
    
    this->min_us = min_us;
    this->max_us = max_us;
    this->is_attached = false;
    this->pin = pin;
    this->channel = channel;

    // Setup LEDC channel first, then attach pin
    ledcSetup(channel, SERVO_PWM_FREQUENCY, SERVO_PWM_RESOLUTION);
    ledcAttachPin(pin, channel);
    this->is_attached = true;
}

void Servo::write(int angle) {
    if (angle < 0)
        angle = 0;
    else if (angle > servo_max_angle)
        angle = servo_max_angle;
    float m = (float) (max_us - min_us) / servo_max_angle;
    int duty_us = min_us + m * angle;
    writeMicroseconds(duty_us);
}

void Servo::writeMicroseconds(int duty_cycle_us) {
    
    // puts within bounds
    int duty_us = duty_cycle_us; 
    if (duty_us < min_us) 
        duty_us = min_us;
    else if (duty_us > max_us)
        duty_us = max_us;

    // converts from us to bits
    int duty_bits = (float) duty_us / SERVO_PWM_PERIOD_US * (SERVO_PWM_WIDTH);

    ledcWrite(this->channel, duty_bits);
}

int Servo::read() {
    // bits to us
    int duty_bits = ledcRead(channel);
    int duty_us = duty_bits * SERVO_PWM_PERIOD_US / SERVO_PWM_WIDTH ;

    if (duty_us < min_us) 
        duty_us = min_us;
    else if (duty_us > max_us)
        duty_us = max_us;
    
    // us to angle
    float slope = (float) servo_max_angle / (max_us - min_us);
    return slope * (duty_us - min_us);
}

bool Servo::attached() {
    return this->is_attached;
}

void Servo::detach() {
    ledcDetachPin(this->pin);
    this->is_attached = false;
}
