/*
 * ToF Sensor Module for Pet Detection
 * Uses 2D array to store distance and reflectance data 
 * [0][0] [0][1] [0][2] ... [0][7]
 * [1][0] [1][1] [1][2] ... [1][7]
 * ...
 * [7][0] [7][1] [7][2] ... [7][7]
 * 
 * Example 8x8 distance matrix (mm)       Example 8x8 reflectance matrix (%)
 * 230 228 226 225 224 226 228 229        20 18 15 14 13 15 18 19
 * 235 233 230 228 227 229 232 233        25 23 20 19 18 20 23 24
 * 240 237 234 231 230 233 236 238        30 28 25 24 23 25 28 29
 * 245 242 239 235 235 238 241 243        35 33 30 29 28 30 33 34 
 * 250 247 243 240 240 243 247 249        40 38 35 34 33 35 38 39
 * 255 252 249 245 245 248 252 255        45 43 40 39 38 40 43 44
 * 260 257 254 250 250 253 257 259        50 48 45 44 43 45 48 49
 * 265 262 258 255 255 258 262 265        55 53 50 49 48 50 53 54
 */

#include <Arduino.h>
#include <Wire.h>
#include <SparkFun_VL53L5CX_Library.h>

#define SDA_PIN 21
#define SCL_PIN 22

#define MUX_ADDRESS   0x70  // I2C multiplexar address
#define TOF_ADDRESS   0x29

//Channel masks for the multiplexer
#define CHASSIS_TOF_CHANNEL 0x02
#define ARM_TOF_CHANNEL 0x10

#define RESOLUTION 8
#define CLK_FREQUENCY 400000
#define RANGING_FREQUENCY 15

#define DEBUG true

SparkFun_VL53L5CX armTof, chassisTof;
VL53L5CX_ResultsData armResults, chassisResults;

float distMap[RESOLUTION][RESOLUTION];
float reflMap[RESOLUTION][RESOLUTION];

int stablePillarCount = 0;
int stablePetLCount = 0;
int stablePetRCount = 0;

float getDistanceToObject(){
  return (distMap[3][3] + distMap[3][4] + distMap[4][3] + distMap[4][4]) * 0.25f; // Center distance
}

void createDistanceMap(float distMap[RESOLUTION][RESOLUTION], const VL53L5CX_ResultsData& result) {
  for (int row = 0; row < RESOLUTION; row++) {
    for (int col = 0; col < RESOLUTION; col++) {
      int i = row * RESOLUTION + col;
      distMap[7 - row][col] = result.distance_mm[i]; // vertically flipped
    }
  }
}

void createReflectanceMap(float reflMap[RESOLUTION][RESOLUTION], const VL53L5CX_ResultsData& result) {
  for (int row = 0; row < RESOLUTION; row++) {
    for (int col = 0; col < RESOLUTION; col++) {
      int i = row * RESOLUTION + col;
      reflMap[7 - row][col] = result.reflectance[i];  // vertically flipped
    }
  }
}

float getMeanCenterDistance(const float distMap[RESOLUTION][RESOLUTION]) {
  float sum = 0.0f;
  for (int i = 4; i < 7; i++) {
    for (int j = 3; j < 5; j++) {
      sum += distMap[i][j];
    }
  }
  return sum * 0.17f;
}

float getMeanCenterReflectance(const float refl[RESOLUTION][RESOLUTION]) {
  
  float sum = 0.0f;
  int count = 0;
  for (int i = 4; i < 7; i++) {
    for (int j = 3; j < 5; j++) {
      sum += refl[i][j];
      count++;
    }
  }
  return sum / count;
}

bool detectLeftCylindricalObject(const float distance[RESOLUTION][RESOLUTION]) {

  //Checks the difference between the mean of two center columns
  float meanCenterL = (distance[3][3] + distance[4][3] + distance[5][3]) * 0.33f;
  float meanCenterR = (distance[3][4] + distance[4][4] + distance[5][4]) * 0.33f;
  float meanCenter = (meanCenterL + meanCenterR) * 0.5f;
  float diffMiddle = fabs(meanCenterL - meanCenterR);

  //Checks the difference between the mean of two columns beside the center columns
  float meanSideL = (distance[3][2] + distance[4][2] + distance[5][2]) * 0.33f;
  float meanSideR = (distance[3][5] + distance[4][5] + distance[5][5]) * 0.33f;
  float meanSide = (meanSideL + meanSideR) * 0.5f;
  float diffSide = fabs(meanSideL - meanSideR);

  // Checks if the top middle grids are more than threshold
  // This is to ensure that it does not detect zipline poles as a pet
  float meanCenterTop = (distance[0][3] + distance[0][4]) * 0.5f;

  return (diffSide <= 30.0f && meanCenter < meanSide && meanCenterTop >= 260.0f);
}

bool detectRightCylindricalObject(const float distance[RESOLUTION][RESOLUTION]) {
  // Checks the difference between the mean of two center columns
  float meanCenterL = (distance[3][4] + distance[4][4] + distance[5][4]) * 0.33f;
  float meanCenterR = (distance[3][5] + distance[4][5] + distance[5][5]) * 0.33f;
  float meanCenter = (meanCenterL + meanCenterR) * 0.5f;
  float diffMiddle = fabs(meanCenterL - meanCenterR);

  // Checks the difference between the mean of two columns beside the center columns
  float meanSideL = (distance[3][3] + distance[4][3] + distance[5][3]) * 0.33f;
  float meanSideR = (distance[3][6] + distance[4][6] + distance[5][6]) * 0.33f;
  float meanSide = (meanSideL + meanSideR) * 0.5f;
  float diffSide = fabs(meanSideL - meanSideR);

  return (diffSide <= 30.0f && meanCenter < meanSide);
}

void selectMuxChannel(uint8_t mask) {
  Wire.beginTransmission(MUX_ADDRESS);
  Wire.write(mask);
  Wire.endTransmission();
  delay(10);
}

void scanMux(uint8_t mask) {
  selectMuxChannel(mask);
  Wire.beginTransmission(TOF_ADDRESS);
  uint8_t err = Wire.endTransmission();
  Serial.printf("Scan mask 0x%02X: %s\n",
                mask,
                err == 0 ? "FOUND" : "NOT FOUND");
}

bool initTof(SparkFun_VL53L5CX &sensor, uint8_t mask) {
  selectMuxChannel(mask);

  if (!sensor.begin()) {            
    Serial.printf("✗ Failed to init sensor at 0x%02X\n", mask);
    return false;
  }

  if (DEBUG) {
    Serial.printf("✓ Initialized sensor at 0x%02X\n", mask);
  }

  sensor.setResolution(RESOLUTION * RESOLUTION);
  sensor.setRangingFrequency(RANGING_FREQUENCY);
  sensor.startRanging();
  return true;
}

void readSensor(SparkFun_VL53L5CX &sensor, uint8_t mask, VL53L5CX_ResultsData &result) {
  selectMuxChannel(mask);
  delay(10);
unsigned long startTime = millis();
  if (sensor.isDataReady()) {
      
      if (sensor.getRangingData(&result)) {
        if(DEBUG){
          Serial.printf("Sensor 0x%02X data ready in %lu ms\n", mask, millis() - startTime);
        }
        createDistanceMap(distMap, result);
        float meanDistance = getMeanCenterDistance(distMap);

        if (meanDistance >= 100.0f && meanDistance <= 240.0f) {
          if(mask == ARM_TOF_CHANNEL && detectLeftCylindricalObject(distMap)) {
            createReflectanceMap(reflMap, result);

            if (getMeanCenterReflectance(reflMap) <= 10.0f) {
              stablePillarCount++;
              stablePetLCount = 0;

              if(DEBUG){
                Serial.println("Pillar detected");
              }

              if (stablePillarCount == 2) {
                if(DEBUG){
                  Serial.println("*** Confirmed Pillar! ***");
                }
            
                // TODO: Handle confirmed pillar detection
                stablePillarCount = 0;
              }
            } 
            else {
              stablePetLCount++;
              stablePillarCount = 0;

              if(DEBUG){
                Serial.println("Pet on left");
              }

              if (stablePetLCount == 2) {

                if(DEBUG){
                  Serial.println("*** Confirmed Pet on left! ***");
                }
                // TODO: Handle confirmed pet detection
                stablePetLCount = 0;
              }
            }
          } 
          else if(mask == CHASSIS_TOF_CHANNEL && detectRightCylindricalObject(distMap)) {
            stablePetRCount++;

            if(DEBUG){
              Serial.println("Pet on right");
            }

            if (stablePetRCount == 2) {
              if(DEBUG){
                Serial.println("*** Confirmed Pet on right! ***");
              }
              // TODO: Handle confirmed pet detection
            }
          }
          else {

            if(DEBUG){
              Serial.println("Not centered");
            }

            stablePetLCount = 0;
            stablePetRCount = 0;
            stablePillarCount = 0;

            
          }
        } else {
          stablePetLCount = 0;
          stablePetRCount = 0;
          stablePillarCount = 0;
        }
      }
    }
}


void setup() {

  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  if(DEBUG){
    scanMux(ARM_TOF_CHANNEL);
    scanMux(CHASSIS_TOF_CHANNEL); 
  }

  initTof(armTof, ARM_TOF_CHANNEL);
  initTof(chassisTof, CHASSIS_TOF_CHANNEL);
}

void loop() {

  unsigned long startTime = millis();
  readSensor(armTof, ARM_TOF_CHANNEL, armResults);
  Serial.printf("Time taken for arm sensor: %lu ms\n", millis() - startTime);

  startTime = millis();
  readSensor(chassisTof, CHASSIS_TOF_CHANNEL, chassisResults);
  Serial.printf("Time taken for chassis sensor: %lu ms\n", millis() - startTime);
}