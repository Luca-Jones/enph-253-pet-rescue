#ifndef SONAR_H
#define SONAR_H

#define SPEED_OF_SOUND_CM_PER_US 0.0343

#define SONAR_EDGE_DISTANCE_CM 20

void sonar_setup();
int sonar_get_distance_cm();

#endif