#ifndef SERVO_H
#define SERVO_H

#define SERVO_DEFAULT_MIN_US 500
#define SERVO_DEFAULT_MAX_US 2500
#define SERVO_PWM_PERIOD_US 20000   // 50 Hz
#define SERVO_BIT_RESOLUTION 12     // 12 bits is enough for the PWM resolution
#define SERVO_PWM_WIDTH 4096        // 2 ^ 12 

class Servo
{
public:

    Servo(int servo_max_angle);
    
    /**
     * Attaches a pin to a free PWM channel.
     * Pins 2,4,5,12-19,21-23,25-27,32-33 are available for servos.
     * @param pin the GPIO pin to attach
     * @param channel the channel to attach the pin to (0-15)
     */
    void attach(int pin, int channel);

    /**
     * Attaches a pin to a free PWM channel
     * Pins 2,4,12-19,21-23,25-27,32-33 are recommended.
     * Check the datasheet for your servo. The PWM duty cycle
     * has a minimum and maximum value that correspond to
     * minimum and maximum angles on the servo.
     * @warning Settings these limits incorrectly and driving the
     * servo with
     * @param pin the GPIO pin to attach
     * @param channel the channel to attach the pin to (0-15)
     * @param min_us PWM duty cycle for the min angle in microseconds.
     * @param max_us PWM duty cycle for the max angle in microseconds.
     */
    void attach(int pin, int channel, int min_us, int max_us);

    /**
     * Sets the angle of the servo in degrees.
     * The angle should stay within 0 to 180 degrees.
     * Inputs outside less than 0 are treated as 0,
     * and intputs above 180 are treated as 180.
     * On a continuous rotation servo, this sets the speed of the servo. 
     * 0 is full-speed in one direction, 180 is full speed in the 
     * opposite direction, and 90 is no movement.
     */
    void write(int angle);

    /**
     * Sets the PWM duty cycle to be sent to the servo. This
     * determines the angle that the servo moves to or in the
     * case of continuous rotation servos, the speed of rotation.
     * The input should stay within the limiting values set in
     * attach(pin, min_us, max_us). The defualt limits are 1000 us
     * and 2000 us. Inputs below the min limit are treated as min
     * and inputs above max are treated as max.
     */
    void writeMicroseconds(int duty_cycle_us);

    /**
     * Reads the current setpoint of the servo in degrees. This is not
     * necessarily the same as the servo's current physical angle.
     * @returns Setpoint angle of the servo between 0 to 180 degrees.
     */
    int read();

    /**
     * @returns true if this servo object is attached to a pin, false otherwise
     */
    bool attached();

    /**
     * Detaches this servo object from its pin.
     */
    void detach();

private:
    int servo_max_angle;
    int min_us;
    int max_us;
    int pin;
    int channel;
    bool is_attached;
};

#endif