#include <Arduino.h>
#include <config/pin_out.h>
#include <config/pwm_config.h>

#define CASCADE_LIFT_UP     LOW
#define CASCADE_LIFT_DOWN   HIGH
#define CASCADE_LIFT_TIME   5500
#define CASCADE_LOWER_TIME  5200

// Motor cascade_motor(PIN_CASCADE_PWM, PWM_CHANNEL_CASCADE, PWM_FRQ_HZ_CASCADE, PWM_RESOLUTION_CASCADE, PIN_CASCADE_DIR);

void setup() {

    Serial.begin(115200);

    // set up cascade motor
    pinMode(PIN_CASCADE_DIR, OUTPUT);
    ledcSetup(PWM_CHANNEL_CASCADE, PWM_FRQ_HZ_CASCADE, PWM_RESOLUTION_CASCADE);
    ledcAttachPin(PIN_CASCADE_PWM, PWM_CHANNEL_CASCADE);


    
}

void loop() {

    Serial.println("Lifting cascade!");

    // raise the cascade
    digitalWrite(PIN_CASCADE_DIR, CASCADE_LIFT_UP);
    ledcWrite(PWM_CHANNEL_CASCADE, PWM_MAX_DUTY_CASCADE);
    delay(CASCADE_LIFT_TIME);
    ledcWrite(PWM_CHANNEL_CASCADE, 0);
    
    delay(3000);

    Serial.println("Lowering cascade!");
    
    // lower the cascade
    digitalWrite(PIN_CASCADE_DIR, CASCADE_LIFT_DOWN);
    ledcWrite(PWM_CHANNEL_CASCADE, PWM_MAX_DUTY_CASCADE);
    delay(CASCADE_LOWER_TIME);
    ledcWrite(PWM_CHANNEL_CASCADE, 0);
    
    delay(3000);
}