#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "driver/gpio.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 // shared with the esp32 reset pin

#define PIN_EXT_LED 25          // pin 33 to power the LED
#define PIN_LIMIT_SWITCH 26     // pin to attach the interrupt to

/* creates a global instance of the display object */
Adafruit_SSD1306 display_handler(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); 
volatile bool switchPressed;
volatile unsigned int counter;

// IRAM safe code
void IRAM_ATTR switchPressedISR() {
    switchPressed = gpio_get_level((gpio_num_t) PIN_LIMIT_SWITCH);
    gpio_set_level((gpio_num_t) PIN_EXT_LED, switchPressed);
    if(!switchPressed) {
        counter++;
    }
}

void OledSetup() {
    display_handler.begin(SSD1306_SWITCHCAPVCC, 0x3C); // 3.3V at the default i2c addr
    display_handler.setTextSize(1);
    display_handler.setTextColor(SSD1306_WHITE);
    display_handler.clearDisplay();
    display_handler.setCursor(0, 0);
}

void setup() {

    OledSetup();
    display_handler.println("Hello, World!");
    display_handler.display();

    pinMode(PIN_EXT_LED, OUTPUT);
    pinMode(PIN_LIMIT_SWITCH, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_LIMIT_SWITCH), switchPressedISR, CHANGE);
    digitalWrite(PIN_EXT_LED, LOW); 
    switchPressed = false;
    counter = 0;
}

void loop() {
    display_handler.clearDisplay();
    display_handler.setCursor(0, 0);
    display_handler.println(counter);
    display_handler.display();
}

