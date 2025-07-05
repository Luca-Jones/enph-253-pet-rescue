#include <Arduino.h>
#include "driver/ledc.h"
#include "driver/adc.h"

#define MAX_DUTY_CYCLE 4096
#define PWM_1_FREQUENCY 1000
#define PWM_1_CHANNEL 1
#define PWM_1_PIN 8
#define PWM_2_FREQUENCY 1000
#define PWM_2_CHANNEL 2
#define PWM_2_PIN 7

int pot_read = 0;

void setup() {
    adc1_config_width(ADC_WIDTH_12Bit);

    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_12); // ADC1_CHANNEL_0 = pin 36
    ledcSetup(PWM_1_CHANNEL, PWM_1_FREQUENCY, 12); // (pwmchannel to use, frequency in Hz, number of bits)
    ledcAttachPin(PWM_1_PIN, PWM_1_CHANNEL);

    adc1_config_channel_atten(ADC1_CHANNEL_1, ADC_ATTEN_DB_12);
    ledcSetup(PWM_2_CHANNEL, PWM_2_FREQUENCY, 12);
    ledcAttachPin(PWM_2_PIN, PWM_2_CHANNEL);

    ledcWrite(PWM_2_CHANNEL, MAX_DUTY_CYCLE / 2); // 50% duty cycle

}

void loop() {
    pot_read = adc1_get_raw(ADC1_CHANNEL_0); // reads the analog value of ADC1_CHANNEL_0 (pin 36)
    ledcWrite(PWM_1_CHANNEL, pot_read); // writes a dutycycle to the specified pwmchannel
    // ledcWrite(PWM_2_CHANNEL, MAX_DUTY_CYCLE / 2);
    delay(10); // brief delay of 10ms
}