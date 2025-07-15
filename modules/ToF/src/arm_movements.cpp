#include "arm_movements.h"

void grab_ground_pet(Arm *arm, Servo claw) {
    claw.write(180);
    arm->lerp_to_pos(250,50,250);
    delay(500);
    arm->lerp_to_pos(350,50,250);
    delay(500);
    claw.write(100);
    delay(500);
    arm->lerp_to_pos(180,300,250);
    delay(500);
    claw.write(180);
    delay(5000);
}

void grab_pillar_pet(Arm *arm, Servo claw) {
    claw.write(180);
    arm->lerp_to_pos(250,50,250);
    delay(1000);
    arm->lerp_to_pos(250,200,250);
    delay(500);
    arm->lerp_to_pos(350,200,250);
    delay(500);
    claw.write(95);
    delay(500);
    arm->lerp_to_pos(220,210,250);
    delay(500);
    arm->lerp_to_pos(220,300,250);
    delay(500);
    claw.write(180);
    delay(5000);
}

void grab_hollow_pillar_pet(Arm *arm, Servo claw) {
    claw.write(140);
    arm->lerp_to_pos(250,50,250);
    delay(1000);
    arm->lerp_to_pos(250,200,250);
    delay(500);
    arm->lerp_to_pos(350,200,250);
    delay(500);
    claw.write(95);
    delay(500);
    arm->lerp_to_pos(220,210,250);
    delay(500);
    arm->lerp_to_pos(220,300,250);
    delay(500);
    claw.write(180);
    delay(5000);
}

void grab_debris_pet(Arm *arm, Servo claw) {
    claw.write(180);
    arm->lerp_to_pos(250,50,250);
    delay(1500);
    arm->lerp_to_pos(250,100,250);
    delay(500);
    arm->lerp_to_pos(350,100,250);
    delay(500);
    claw.write(100);
    delay(500);
    arm->lerp_to_pos(350,300,250);
    delay(500);
    arm->lerp_to_pos(180,300,250);
    delay(500);
    claw.write(180);
    delay(5000);
}