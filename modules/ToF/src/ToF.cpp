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

// I2C pins
#define SDA_PIN 21
#define SCL_PIN 22

SparkFun_VL53L5CX tof;
VL53L5CX_ResultsData result;

const int CLK_FREQUENCY = 400000; // 100kHz~1MHz
const int CLAW_TOF_I2C_ADDR = 0x29;
const int CHASSIS_TOF_I2C_ADDR = 0x2A;
const int MAP_SIZE = 8; // 4*4 or 8*8 matrix
const int RANGING_FREQUENCY = 8; // 1~15 Hz

float distMap[MAP_SIZE][MAP_SIZE];
float reflMap[MAP_SIZE][MAP_SIZE];

int stablePillarCount = 0;
int stablePetCount = 0;


void CreateDistanceMap(float distMap[MAP_SIZE][MAP_SIZE], const VL53L5CX_ResultsData& result) {
  for (int row = 0; row < MAP_SIZE; row++) {
    for (int col = 0; col < MAP_SIZE; col++) {
      int i = row * MAP_SIZE + col;
      distMap[row][col] = result.distance_mm[i]; // vertically flipped
    }
  }
}

void createReflectanceMap(float reflMap[MAP_SIZE][MAP_SIZE], const VL53L5CX_ResultsData& result) {
  for (int row = 0; row < MAP_SIZE; row++) {
    for (int col = 0; col < MAP_SIZE; col++) {
      int i = row * MAP_SIZE + col;
      reflMap[row][col] = result.reflectance[i];  // vertically flipped
    }
  }
}

float getMeanCenterDistance(const float distMap[MAP_SIZE][MAP_SIZE]) {
  float sum = 0.0f;
  for (int i = 4; i < 7; i++) {
    for (int j = 3; j < 5; j++) {
      sum += distMap[i][j];
    }
  }
  return sum / 6.0f;
}

float getMeanCenterReflectance(const float refl[MAP_SIZE][MAP_SIZE]) {
  
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

bool detectCylindricalObject(const float distance[MAP_SIZE][MAP_SIZE]) {

  //Checks the difference between the mean of two center columns
  float meanCenterL = (distance[3][3] + distance[4][3] + distance[5][3]) / 3.0f;
  float meanCenterR = (distance[3][4] + distance[4][4] + distance[5][4]) / 3.0f;
  float meanCenter = (meanCenterL + meanCenterR) / 2.0f;
  float diffMiddle = fabs(meanCenterL - meanCenterR);

  //Checks the difference between the mean of two columns beside the center columns
  float meanSideL = (distance[3][2] + distance[4][2] + distance[5][2]) / 3.0f;
  float meanSideR = (distance[3][5] + distance[4][5] + distance[5][5]) / 3.0f;
  float meanSide = (meanSideL + meanSideR) / 2.0f;
  float diffSide = fabs(meanSideL - meanSideR);

  // Checks if the top middle grids are more than threshold
  // This is to ensure that it does not detect zipline poles as a pet
  float meanCenterTop = (distance[0][3] + distance[0][4]) / 2.0f;

  return (diffSide <= 30.0f && meanCenter < meanSide && meanCenterTop >= 260.0f);
}


void setup() {

  Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(CLK_FREQUENCY);

  Serial.println("Initializing VL53L5CX...");
  if (!tof.begin()) {
    Serial.println("Sensor not found!");
  }

  tof.setAddress(CLAW_TOF_I2C_ADDR);
  tof.setResolution(MAP_SIZE*MAP_SIZE);
  tof.setRangingFrequency(RANGING_FREQUENCY);
  tof.startRanging();
}

void loop() {
  if (tof.isDataReady()) {
    if (tof.getRangingData(&result)) {

      CreateDistanceMap(distMap, result);
      float meanDistance = getMeanCenterDistance(distMap);

      if (meanDistance >= 100.0f && meanDistance <= 240.0f) {
        if(detectCylindricalObject(distMap)) {

          createReflectanceMap(reflMap, result);

          if (getMeanCenterReflectance(reflMap) <= 10.0f) {
            stablePillarCount++;
            stablePetCount = 0;
            Serial.println("Pillar detected");

            if (stablePillarCount == 3) {
              Serial.println("*** Confirmed Pillar after 3 frames! ***");
              // TODO: Handle confirmed pillar detection
              stablePillarCount = 0;
            }
          } 
          else {
            stablePetCount++;
            stablePillarCount = 0;
            Serial.println("Pet on ground");

            if (stablePetCount == 3) {
              Serial.println("*** Confirmed Pet after 3 frames! ***");
              // TODO: Handle confirmed pet detection
              stablePetCount = 0;
            }
          }

        } else {
          stablePetCount = 0;
          stablePillarCount = 0;
          Serial.println("Not centered");
        }
      } else {
        // Out of desired range
        stablePetCount = 0;
        stablePillarCount = 0;
      }
    }
  }
  delay(0.2);
}
