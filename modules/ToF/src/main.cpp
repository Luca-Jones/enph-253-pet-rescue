#include <Arduino.h>
#include <Wire.h>

#define PIN_UART_RX 14
#define PIN_UART_TX 12

typedef enum tof_reading_e {
    TOF_READING_NONE            = 0,
    TOF_READING_PET             = 1,
    TOF_READING_PILLAR          = 2,
    TOF_READING_PET_OFFCENTER   = 3,
};

struct tof_data_packet {
    enum tof_reading_e reading;
};

void setup() {

    delay(1000);

    /* Computer Serial */
    Serial.begin(115200);

    /* GPIO Serial */
    Serial2.begin(115200, SERIAL_8N1, PIN_UART_RX, PIN_UART_TX);
    Serial.println("UART2 initialized");

}

void loop() {

    struct tof_data_packet tof_data;
    
    
    if (Serial2.available()) {

        Serial2.readBytes((char *) &tof_data, sizeof(tof_data_packet));

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
        default:
            break;
        }
    }

}