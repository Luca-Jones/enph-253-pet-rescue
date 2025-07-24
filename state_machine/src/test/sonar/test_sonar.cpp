#include <Arduino.h>
#include <sensors/Sonar.h>

void setup() {
    Serial.begin(115200);
    sonar_setup();
}

void loop() {
    int dist = sonar_get_distance_mm();
    Serial.printf("distance = %d\n", dist);
    delay(1000);
}