#ifndef ARM_MOVEMENTS_H
#define ARM_MOVEMENTS_H

#include "Arm.h"

void grab_ground_pet(Arm *arm, Servo claw);
void grab_pillar_pet(Arm *arm, Servo claw);
void grab_hollow_pillar_pet(Arm *arm, Servo claw);
void grab_debris_pet(Arm *arm, Servo claw);

#endif