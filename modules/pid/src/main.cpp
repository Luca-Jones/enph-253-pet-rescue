#include <Arduino.h>

const int SENSOR_LEFT = 14;
const int SENSOR_CENTER = 27;
const int SENSOR_RIGHT = 33;

const int MOTOR_LEFT_PWM = 8;      
const int MOTOR_LEFT_DIR = 19;      
const int MOTOR_RIGHT_PWM =7;     
const int MOTOR_RIGHT_DIR = 5;     

const int PWM_FREQ = 1000;
const int PWM_RESOLUTION = 8;
const int PWM_CHANNEL_LEFT = 0;
const int PWM_CHANNEL_RIGHT = 1;

float Kp = 100.0;    
float Kd = 0;  
float error = 0;
float lastError = 0;
float derivative = 0;
float pidOutput = 0;

int baseSpeed = 150;
int maxSpeed = 255;
int minSpeed = 0;

unsigned long lastTime = 0;
const unsigned long sampleTime = 10; 
unsigned long lastPrintTime = 0;
const unsigned long printInterval = 500; 

float calculateError(bool left, bool center, bool right);
float calculatePID(float currentError, unsigned long deltaTime);
void controlMotors(float pidOutput);
void stopMotors();

void setup() {
  Serial.begin(115200);
  
  // Initialize sensor pins
  pinMode(SENSOR_LEFT, INPUT);
  pinMode(SENSOR_CENTER, INPUT);
  pinMode(SENSOR_RIGHT, INPUT);
  
  // Initialize motor pins
  pinMode(MOTOR_LEFT_DIR, OUTPUT);
  pinMode(MOTOR_RIGHT_DIR, OUTPUT);
  
  // Setup PWM channels for ESP32
  ledcSetup(PWM_CHANNEL_LEFT, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(PWM_CHANNEL_RIGHT, PWM_FREQ, PWM_RESOLUTION);
  
  // Attach PWM channels to pins
  ledcAttachPin(MOTOR_LEFT_PWM, PWM_CHANNEL_LEFT);
  ledcAttachPin(MOTOR_RIGHT_PWM, PWM_CHANNEL_RIGHT);
}

void loop() {
  unsigned long currentTime = millis();
  
  // Run PID loop at specified interval
  if (currentTime - lastTime >= sampleTime) {
    // Read sensors
    bool leftSensor = digitalRead(SENSOR_LEFT);
    bool centerSensor = digitalRead(SENSOR_CENTER);
    bool rightSensor = digitalRead(SENSOR_RIGHT);
    
    // Calculate position error
    error = calculateError(leftSensor, centerSensor, rightSensor);
    
    // Calculate PID output
    pidOutput = calculatePID(error, currentTime - lastTime);
    
    // Apply motor control
    controlMotors(pidOutput);
    
    // Debug printing at a slower rate
    if (currentTime - lastPrintTime >= printInterval) {
      Serial.println("------ Sensor Values ------");
      Serial.printf("Left: %d | Center: %d | Right: %d\n", 
                    leftSensor, centerSensor, rightSensor);
      
      Serial.println("------ Motor Control ------");
      int leftSpeed = baseSpeed - pidOutput;
      int rightSpeed = baseSpeed + pidOutput;
      leftSpeed = constrain(leftSpeed, -maxSpeed, maxSpeed);
      rightSpeed = constrain(rightSpeed, -maxSpeed, maxSpeed);
      
      Serial.printf("Left Motor:  DIR=%s, PWM=%d\n", 
                    (leftSpeed >= 0 ? "FWD" : "REV"), abs(leftSpeed));
      Serial.printf("Right Motor: DIR=%s, PWM=%d\n", 
                    (rightSpeed >= 0 ? "FWD" : "REV"), abs(rightSpeed));
      Serial.printf("Error: %.2f | PID Output: %.2f\n", error, pidOutput);
      Serial.println("------------------------\n");
      
      lastPrintTime = currentTime;
    }
    
    lastTime = currentTime;
  }
}


float calculatePID(float currentError, unsigned long deltaTime) {
  float lastError = currentError;
  
  float derivative = (currentError - lastError) / deltaTime;
  
  float output = (Kp * currentError) + (Kd * derivative);
  
  return output;
}

void controlMotors(float pidOutput) {
  int leftSpeed = baseSpeed - pidOutput;
  int rightSpeed = baseSpeed + pidOutput;
  
  // Constrain speeds
  leftSpeed = constrain(leftSpeed, -maxSpeed, maxSpeed);
  rightSpeed = constrain(rightSpeed, -maxSpeed, maxSpeed);
  
  // left motor 
  digitalWrite(MOTOR_LEFT_DIR, leftSpeed >= 0 ? HIGH : LOW);
  ledcWrite(PWM_CHANNEL_LEFT, abs(leftSpeed));
  
  // right motor 
  digitalWrite(MOTOR_RIGHT_DIR, rightSpeed >= 0 ? HIGH : LOW);
  ledcWrite(PWM_CHANNEL_RIGHT, abs(rightSpeed));
}

void stopMotors() {
  ledcWrite(PWM_CHANNEL_LEFT, 0);
  ledcWrite(PWM_CHANNEL_RIGHT, 0);
  digitalWrite(MOTOR_LEFT_DIR, LOW);
  digitalWrite(MOTOR_RIGHT_DIR, LOW);
}

float calculateError(bool left, bool center, bool right) {
// Negative = line is to the left, Positive = line is to the right

if (!left && !center && !right) {
  // No line detected - continue with last error
    return error;
  }
  
  if (!left && center && !right) {
    // Line is centered
    return 0;
  }
  
  if (left && !center && !right) {
    // Line is to the left
    return -2;
  }
  
  if (!left && !center && right) {
    // Line is to the right
    return 2;
  }
  
  if (left && center && !right) {
    // Line is slightly to the left
    return -1;
  }
  
  if (!left && center && right) {
    // Line is slightly to the right
    return 1;
  }
  
  if (left && !center && right) {
    // Line is wide or at intersection - go straight
    return 0;
  }
  
  if (left && center && right) {
    // All sensors on line - intersection or wide line
    return 0;
  }
  
  return 0;
}