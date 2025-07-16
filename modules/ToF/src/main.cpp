#include <Arduino.h>
#include <Wire.h>
#include "arm_movements.h"

#define PIN_UART_RX 14
#define PIN_UART_TX 12

enum tof_reading_e {
    TOF_READING_NONE            = 0x00,
    TOF_READING_PET             = 0x01,
    TOF_READING_PILLAR          = 0x02,
    TOF_READING_PET_OFFCENTER   = 0x03,
};

struct tof_data_packet {
    enum tof_reading_e reading;
};

void setup() {

    /* Computer Serial */
    Serial.begin(115200);

    /* GPIO Serial */
    Serial2.begin(115200, SERIAL_8N1, PIN_UART_RX, PIN_UART_TX);
    Serial.println("UART2 initialized");

}

void loop() {

    struct tof_data_packet tof_data;
    
    if (Serial2.available()) {

        Serial2.readBytes((char *) &tof_data, 1);
        Serial2.flush();
        Serial.printf("reading = %d\n", tof_data.reading);

        switch (tof_data.reading){
        case TOF_READING_NONE:
            Serial.println("No reading...");
            break;
        case TOF_READING_PET:
            Serial.println("Pet detected!");
            break;
        case TOF_READING_PILLAR:
            Serial.println("Pillar detected!");
            break;
        case TOF_READING_PET_OFFCENTER:
            Serial.println("Pet offcenter!");
            break;
        default:
            Serial.println("Incorrect Data Format!");
            break;
        }
    }

    delay(100);

}