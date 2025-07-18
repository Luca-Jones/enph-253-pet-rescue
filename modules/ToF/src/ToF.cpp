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

const int MAP_SIZE = 8; // 8x8 grid
const int MIDDLE_L_COL = 3;
const int MIDDLE_R_COL = 4;
const int SIDE_L_COL = 2;
const int SIDE_R_COL = 5;

float prevCenter[3][2];
bool  hasPrevCenter    = false;
int   stableDistCount  = 0;


float meanArray(const float *data, int len) {
  float sum = 0;
  for (int i = 0; i < len; i++) sum += data[i];
  return sum / len;
}


bool detectPetGround(const float distance[MAP_SIZE][MAP_SIZE]) {

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
  // This is to ensure that it does not detect zipline poles
  float meanCenterTop = (distance[0][3] + distance[0][4]) / 2.0f;

  return (diffSide <= 25.0f && meanCenter < meanSide && meanCenterTop >= 260.0f);
}


bool detectPetPillar(const float center[3][2], const float meanReflectance) {

  if (meanReflectance <= 10.0f){
    if (hasPrevCenter) {
      float diffs[6];
      int idx = 0;
      for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
          diffs[idx++] = fabs(center[i][j] - prevCenter[i][j]);
        }
      }
      float meanDiff = meanArray(diffs, 6);
      if (meanDiff < 50.0f) {
        stableDistCount++;
      } else {
        stableDistCount = 0;
      }
    }

    memcpy(prevCenter, center, sizeof(prevCenter));
    hasPrevCenter = true;

    if (stableDistCount >= 3) {
      stableDistCount = 0;
      return true;
    }
  } else {
    hasPrevCenter = false;
    stableDistCount = 0;
  }
  return false;
}

void setup() {

  Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(400000);

  Serial.println("Initializing VL53L5CX...");
  if (!tof.begin()) {
    Serial.println("Sensor not found!");
  }

  int deviceAddress = 0x29;
  tof.setAddress(deviceAddress);
    /*while (Serial.available() == false) //Clear incoming key presses
      ; //Do nothing

    if (tof.setAddress(deviceAddress) == false)
    {
      Serial.println(F("Device failed to set new address. Please try again. Freezing..."));
      while (1);
    }

    delay(50);

    int newAddress = tof.getAddress();

    Serial.print(F("New address is: 0x"));
    Serial.println(newAddress, HEX);
    */

  tof.setResolution(8*8);
  tof.setRangingFrequency(8);
  tof.startRanging();

  /*Serial2.begin(115200, SERIAL_8N1, UART2_RX, UART2_TX);
  Serial.println("Sensor ready and UART2 open");*/
}

void loop() {
  if (tof.isDataReady()) {
    if (tof.getRangingData(&result)) {

      float dist[MAP_SIZE][MAP_SIZE];
      for (int row = 0; row < MAP_SIZE; row++) {
        for (int col = 0; col < MAP_SIZE; col++) {
          int i = row*8 + col;
          dist[7-row][col] = result.distance_mm[i];
        }
      }

      // Extract center 3×2 block
      float centerBlock[3][2];
      float flatCenter[6];
      int k = 0;
      for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
          centerBlock[i][j] = dist[4+i][3+j];
          flatCenter[k++] = centerBlock[i][j];
        }
      }

      float meanDistance = meanArray(flatCenter, 6);

      if (meanDistance >= 100.0f && meanDistance <= 240.0f) {

        float refl[MAP_SIZE][MAP_SIZE];
        for (int row = 0; row < MAP_SIZE; row++) {
          for (int col = 0; col < MAP_SIZE; col++) {
            int index = row*8 + col;
            refl[7-row][col] = result.reflectance[index];
          }
        }

        float middleRefl[6];
        int k = 0;
        for (int i = 0; i < 3; i++) {
          for (int j = 0; j < 2; j++) {
            middleRefl[k++] = refl[4+i][3+j];
          }
        }

        float meanReflectance = meanArray(middleRefl, 6);

        if (detectPetPillar(centerBlock, meanReflectance)) {
          Serial.println("Pillar detected");
          //Serial2.write(0x02);
        } else if (detectPetGround(dist)) {
          Serial.println("Pet on ground");
          //Serial2.write(0x01);
        } else {
          Serial.println("Not centered");
          //Serial2.write(0x03);
        }
      } else {
        // Out of desired range
        //Serial2.write(0x00);
      }
    }
  }
  delay(0.2);
}
