#ifndef SONAR_H
#define SONAR_H

#define SONAR_STABILIZING_DELAY_US 2
#define SONAR_HIGH_TIME_US 10
#define SPEED_OF_SOUND_CM_PER_US 0.0343

#define SONAR_EDGE_DISTANCE_CM 150

void sonar_setup();
int sonar_get_distance_cm();

#endif