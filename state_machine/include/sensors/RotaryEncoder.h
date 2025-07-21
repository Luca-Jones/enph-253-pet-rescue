#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

void rot_setup();

/** 
 * @returns the current angle of the rotary encoder in degrees
 */ 
int rot_get_angle();

#endif