#ifndef SERVO_H
#define SERVO_H

#define SERVO_DEFAULT_MIN_US    500
#define SERVO_DEFAULT_MAX_US    2500

class Servo
{
public:

    Servo(int servo_max_angle);
    
    /**
     * Attaches a pin to the speciied PWM channel.
     * Pins 2,4,5,12-19,21-23,25-27,32-33 are recommended.
     * @param pin the GPIO pin to attach
     * @param channel the channel to attach the pin to (0-7)
     */
    void attach(int pin, int channel);

    /**
     * Attaches a pin to a free PWM channel
     * Pins 2,4,12-19,21-23,25-27,32-33 are recommended.
     * There is a minimum and maximum PWM duty cycle that 
     * correspond to the minimum and maximum angles on the servo. 
     * Check the datasheet for your servo for this information. 
     * @warning Settings the PWM duty cycle limits incorrectly and then 
     * driving the servo may cause damage to the servo. Read the datasheet carefully.
     * @param pin the GPIO pin to attach.
     * @param channel the channel to attach the pin to (0-15).
     * @param min_us the PWM duty cycle for the min angle in microseconds.
     * @param max_us the PWM duty cycle for the max angle in microseconds.
     */
    void attach(int pin, int channel, int min_us, int max_us);

    /**
     * Sets the angle of the servo in degrees.
     * The angle should stay within 0 to servo_max_angle degrees.
     * Inputs less than 0 are treated as 0,
     * and intputs above servo_max_angle are treated as servo_max_angle.
     * On a continuous rotation servo, this sets the speed of the servo. 
     * Typically, 0 is full-speed in one direction, 180 is full speed in the 
     * opposite direction, and 90 is no movement.
     */
    void write(int angle);

    /**
     * Sets the PWM duty cycle to be sent to the servo. This
     * determines the angle that the servo moves to or in the
     * case of continuous rotation servos, the speed of rotation.
     * The input should stay within the limiting values set in
     * attach(pin, min_us, max_us). If no limits are set, the 
     * defaults are used. Inputs below the minimum limit are treated 
     * as the minimum and inputs above the maximum are treated as 
     * the maximum.
     */
    void writeMicroseconds(int duty_cycle_us);

    /**
     * Reads the current setpoint of the servo in degrees. This is not
     * necessarily the same as the servo's current physical angle.
     * @returns Setpoint angle of the servo between 0 to servo_max_angle degrees.
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