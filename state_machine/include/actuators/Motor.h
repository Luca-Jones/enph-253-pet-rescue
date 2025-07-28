#ifndef MOTOR_H
#define MOTOR_H

#include "esp32-hal-ledc.h"

/*
    Just a thin wrapper around the ledc pin setup calls to make things easier.
    Automatically includes a short pause when changing directions to not fry the h-bridge.
    I got lazy on the absraction for this one, there's no need to hide the implementation really.
*/

class Motor 
{
public:

    /**
     * The Motor object is instantiated with these parameters. 
     * The hardware is not set up until a call to Motor::setup()
     * is made.
     */
    Motor(uint8_t pwm_pin, uint8_t pwm_channel, int pwm_frequency, uint8_t pwm_resolution, uint8_t dir_pin);

    /**
     * Sets up the motor with the parameters specified in the constructor
     */
    void setup();

    /**
     * @param speed is from 0 to 2^pwm_resolution
     * @param direction is either HIGH or LOW. 
     * It is up to the user to understand what direction
     * HIGH and LOW correspond to on their motor configuration.
     */
    void write(int speed, int direction);

    /**
     * Sets the motor speed to zero.
     * No need to specify direction.
     */
    void stop();

    ~Motor();

private:
    uint8_t pwm_pin;
    uint8_t pwm_channel;
    int pwm_frequency;
    uint8_t pwm_resolution;
    uint8_t pwm_width;      // 2 ^ pwm_resolution
    uint8_t dir_pin;
    int last_dir;
};

#endif