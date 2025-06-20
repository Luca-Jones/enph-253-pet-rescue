#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 // shared with the esp32 reset pin

#define PIN_UART_RX 14
#define PIN_UART_TX 12

/* creates a global instance of the display object */
Adafruit_SSD1306 display_handler(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); 

void OledSetup() {
    display_handler.begin(SSD1306_SWITCHCAPVCC, 0x3C); // 3.3V at the default i2c addr
    display_handler.setTextSize(1);
    display_handler.setTextColor(SSD1306_WHITE);
    display_handler.clearDisplay();
    display_handler.setCursor(0, 0);
}

void setup() {

    sleep(1);
    OledSetup();
    display_handler.println("Hello, World!");
    display_handler.display();


    /* Computer Serial */
    Serial.begin(115200);

    /* GPIO Serial */
    Serial2.begin(115200, SERIAL_8N1, PIN_UART_RX, PIN_UART_TX);
    Serial.println("UART2 initialized");

}

void loop() {
    
    Serial2.println("Hello from ESP32!");

    if (Serial2.available()) {
        String received = Serial2.readStringUntil('\n');
        Serial.print("Received on UART2: ");
        Serial.println(received);
    }

    sleep(1);
}