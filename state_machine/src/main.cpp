#include <Arduino.h>
#include "state_machine/state_machine.h"

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
    state_machine_run();
}

void loop() {}