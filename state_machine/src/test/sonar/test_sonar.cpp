#include <Arduino.h>
#include <sensors/Sonar.h>

void setup() {
    Serial.begin(115200);
    sonar_setup();
}

void loop() {
    int dist = sonar_get_distance_cm();
    Serial.printf("distance = %d\n", dist);
    delay(10);
    if (dist > 20) {
        Serial.println("Edge Detected!");
    }
}