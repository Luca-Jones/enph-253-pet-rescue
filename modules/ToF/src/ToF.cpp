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

float prevCenter[3][2];
bool  hasPrevCenter    = false;
int   stableDistCount  = 0;


float meanArray(const float *data, int len) {
  float sum = 0;
  for (int i = 0; i < len; i++) sum += data[i];
  return sum / len;
}


bool detectPetGround(const float d[8][8]) {
  float middleL = (d[3][3] + d[4][3] + d[5][3]) / 3.0f;
  float middleR = (d[3][4] + d[4][4] + d[5][3]) / 3.0f;
  float diffMiddle = fabs(middleL - middleR);

  float sideL = (d[3][2] + d[4][2]) * 0.5f;
  float sideR = (d[3][5] + d[4][5]) * 0.5f;
  float diffSide = fabs(sideL - sideR);

  float topMid = d[0][3];

  return (diffSide <= 25.0f && diffMiddle < diffSide && topMid >= 200.0f);
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
      float dist[8][8];
      for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
          int idx = r*8 + c;
          dist[7-r][c] = result.distance_mm[idx];
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

        float refl[8][8];
        for (int row = 0; row < 8; row++) {
          for (int col = 0; col < 8; col++) {
            int index = row*8 + col;
            refl[7-row][col] = result.reflectance[index];
          }
        }

        float flatRefl[6];
        int k = 0;
        for (int i = 0; i < 3; i++) {
          for (int j = 0; j < 2; j++) {
            flatRefl[k++] = refl[4+i][3+j];
          }
        }
        float meanReflectance = meanArray(flatRefl, 6);

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
