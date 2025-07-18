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

// UART2 pins for communication
#define UART2_RX 16
#define UART2_TX 17

SparkFun_VL53L5CX tof;
VL53L5CX_ResultsData result;

const int CLK_FREQUENCY = 400000; // 400 kHz I2C clock frequency
const int CLAW_TOF_I2C_ADDR = 0x29;
const int MAP_SIZE = 8; // 8x8 matrix
const int RANGING_FREQUENCY = 8;

int stablePillarCount = 0;
int stablePetCount = 0;

float prevCenter[3][2];
bool  hasPrevCenter    = false;
int   stableDistCount  = 0;


float meanArray(const float *data, int len) {
  float sum = 0;
  for (int i = 0; i < len; i++) sum += data[i];
  return sum / len;
}

float meanCenterReflectance(const float refl[MAP_SIZE][MAP_SIZE]) {
  
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
  float meanSideL = (distance[3][1] + distance[4][1] + distance[5][1]) / 3.0f;
  float meanSideR = (distance[3][6] + distance[4][6] + distance[5][6]) / 3.0f;
  float meanSide = (meanSideL + meanSideR) / 2.0f;
  float diffSide = fabs(meanSideL - meanSideR);

  // Checks if the top middle grids are more than threshold
  // This is to ensure that it does not detect zipline poles as a pet
  float meanCenterTop = (distance[0][3] + distance[0][4]) / 2.0f;

  return (diffSide <= 30.0f && meanCenter < meanSide && meanCenterTop >= 260.0f);
}


bool detectPillar(const float refl[MAP_SIZE][MAP_SIZE]) {

  return meanCenterReflectance(refl) <= 10.0f;
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

      float distMap[MAP_SIZE][MAP_SIZE];
      for (int row = 0; row < MAP_SIZE; row++) {
        for (int col = 0; col < MAP_SIZE; col++) {
          int i = row*8 + col;
          distMap[7-row][col] = result.distance_mm[i];
        }
      }

      // Extract center 3×2 block
      float centerBlock[3][2];
      float flatCenter[6];
      int k = 0;
      for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
          centerBlock[i][j] = distMap[4+i][3+j];
          flatCenter[k++] = centerBlock[i][j];
        }
      }

      float meanDistance = meanArray(flatCenter, 6);

      if (meanDistance >= 100.0f && meanDistance <= 240.0f) {
        if(detectCylindricalObject(distMap)) {

          float reflMap[MAP_SIZE][MAP_SIZE];
          for (int row = 0; row < MAP_SIZE; row++) {
            for (int col = 0; col < MAP_SIZE; col++) {
              int index = row * MAP_SIZE + col;
              reflMap[7 - row][col] = result.reflectance[index];
            }
          }

          if (detectPillar(reflMap)) {
            stablePillarCount++;
            stablePetCount = 0;  // reset other counter
            Serial.println("Pillar detected");

            if (stablePillarCount == 3) {
              Serial.println("*** Confirmed Pillar after 3 frames! ***");
              // Special behavior for confirmed pillar
              stablePillarCount = 0;
            }
          } else {
            stablePetCount++;
            stablePillarCount = 0;  // reset other counter
            Serial.println("Pet on ground");

            if (stablePetCount == 3) {
              Serial.println("*** Confirmed Pet after 3 frames! ***");
              // Special behavior for confirmed pet
              stablePetCount = 0;
            }
          }

        } else {
          stablePetCount = 0;
          stablePillarCount = 0;
          Serial.println("Not centered");
          // Serial2.write(0x03);
        }
      } else {
        // Out of desired range
        stablePetCount = 0;
        stablePillarCount = 0;
        // Serial2.write(0x00);
      }
    }
  }
  delay(0.2);
}
