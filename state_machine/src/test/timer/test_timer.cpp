#include <Arduino.h>
#include "driver/timer.h"

#define TIMEOUT 10

volatile bool timer_triggered = false;

bool IRAM_ATTR timer_isr_callback(void *args) {
    timer_triggered = true;
    return true;
}

void setup() {
    Serial.begin(115200);
    timer_config_t timer_config = {
        .alarm_en = TIMER_ALARM_EN,
        .counter_en = TIMER_PAUSE,
        .counter_dir = TIMER_COUNT_UP,
        .auto_reload = TIMER_AUTORELOAD_EN,
        .divider = 16,
    };
    timer_init(TIMER_GROUP_0, TIMER_0, &timer_config);
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0x0ULL);
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, TIMEOUT * TIMER_BASE_CLK / 16);
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_isr_callback_add(TIMER_GROUP_0, TIMER_0, timer_isr_callback, NULL, ESP_INTR_FLAG_IRAM);

    timer_start(TIMER_GROUP_0, TIMER_0);
    Serial.println("Timer started!");

}

void loop() {
    if (timer_triggered) {
        timer_triggered = false;
        Serial.println("timer triggered");
    }
}
