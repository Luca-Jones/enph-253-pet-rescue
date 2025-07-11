#include <Servo.h>
#include <esp32-hal-ledc.h>
#include <esp32-hal-log.h>

bool pin_has_pwm(int pin);
bool has_channel(int channel);

Servo::Servo(int servo_max_angle) {
    this->servo_max_angle = servo_max_angle;
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

    if (!pin_has_pwm(pin)) {
        ESP_LOGE("ESP32Servo", "This pin can not be a servo: %d Servo available on: 2,4,5,12-19,21-23,25-27,32-33",pin);
        return;
    } else if (!has_channel(channel)) {
        ESP_LOGE("ESP32Servo", "Channel %d does not exist. The available channels aare numbered 0 to 15 (includsive)",channel);
        return;
    }

    if (ledcSetup(channel, 1e6 / SERVO_PWM_PERIOD_US, SERVO_BIT_RESOLUTION) == 0) {
        return;
    }

    ledcAttachPin(pin, channel);
    is_attached = true;
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
    int duty_bits = (float) duty_us / SERVO_PWM_PERIOD_US * (SERVO_PWM_WIDTH); // do not remove brackets

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
    return is_attached;
}

void Servo::detach() {
    ledcDetachPin(pin);
    is_attached = false;
}

bool pin_has_pwm(int pin) {
    return (
        pin == 2 || pin == 4 || pin == 5 ||
		(pin >= 12 && pin <= 19) ||
		(pin >= 21 && pin <= 23) ||
		(pin >= 25 && pin <= 27) ||
		(pin == 32) || (pin == 33)
    );
}

bool has_channel(int channel) {
    return channel >= 0 && channel <= 15;
}

/*
    Notes: 

    What is a channel?
    A channel is not a physical thing. Think of a channel as a PWM generator 
    that needs to be "wired" to a pin to do anything.

    From esp32-hal-ledc.h:

    This group/channel/timmer mapping is for information only;
    the details are handled by lower-level code

    LEDC Chan to Group/Channel/Timer Mapping
    ledc: 0  => Group: 0, Channel: 0, Timer: 0
    ledc: 1  => Group: 0, Channel: 1, Timer: 0
    ledc: 2  => Group: 0, Channel: 2, Timer: 1
    ledc: 3  => Group: 0, Channel: 3, Timer: 1
    ledc: 4  => Group: 0, Channel: 4, Timer: 2
    ledc: 5  => Group: 0, Channel: 5, Timer: 2
    ledc: 6  => Group: 0, Channel: 6, Timer: 3
    ledc: 7  => Group: 0, Channel: 7, Timer: 3
    ledc: 8  => Group: 1, Channel: 0, Timer: 0
    ledc: 9  => Group: 1, Channel: 1, Timer: 0
    ledc: 10 => Group: 1, Channel: 2, Timer: 1
    ledc: 11 => Group: 1, Channel: 3, Timer: 1
    ledc: 12 => Group: 1, Channel: 4, Timer: 2
    ledc: 13 => Group: 1, Channel: 5, Timer: 2
    ledc: 14 => Group: 1, Channel: 6, Timer: 3
    ledc: 15 => Group: 1, Channel: 7, Timer: 3

*/