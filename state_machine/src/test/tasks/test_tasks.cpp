#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

typedef enum {
    EVEN, ODD
} parity_e;

volatile int count;
volatile parity_e parity;
TaskHandle_t counting_up_task_handle;
TaskHandle_t counting_down_task_handle;

void counting_up_task(void *pvParameters) {
    for (;;) {
        count++;
        parity = count % 2 == 0 ? EVEN : ODD;
        delay(1);
    }
}

void counting_down_task(void *pvParameters) {
    for (;;) {
        count--;
        parity = count % 2 == 0 ? EVEN : ODD;
        delay(1);
    }
}

void setup() {
    Serial.begin(115200);
    count = 0;
    parity = EVEN;

    xTaskCreatePinnedToCore(
        counting_down_task,
        "Counting Task",
        4096,
        NULL,
        1,
        &counting_down_task_handle,
        0
    );

    vTaskSuspend(counting_down_task_handle);

    xTaskCreatePinnedToCore(
        counting_up_task,
        "Counting Task",
        4096,
        NULL,
        1,
        &counting_up_task_handle,
        0
    );
}

void loop() {
    Serial.printf("count = %d | ", count);
    Serial.println(parity == EVEN ? "even" : "odd");

    if (count > 5000) {
        
        if (eTaskGetState(counting_up_task_handle) != eSuspended) {
            Serial.println("counting up task stopping...");
            vTaskSuspend(counting_up_task_handle);
        } 

        if (eTaskGetState(counting_down_task_handle) == eSuspended) {
            Serial.println("counting down task starting...");
            vTaskResume(counting_down_task_handle);
        }


    } else if (count < -5000) {
        
        if (eTaskGetState(counting_up_task_handle) == eSuspended) {
            Serial.println("counting up task starting...");
            vTaskResume(counting_up_task_handle);
        } 

        if (eTaskGetState(counting_down_task_handle) != eSuspended) {
            Serial.println("counting down task stopping...");
            vTaskSuspend(counting_down_task_handle);
        }
    }

    delay(100);
}