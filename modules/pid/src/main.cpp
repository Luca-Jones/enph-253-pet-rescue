#include <WebServer.h>
#include <WiFi.h>
#include <Arduino.h>

// WiFi credentials
const char *ssid = "hot_dawgs_esp";
const char *password = "team4_da_best_on_top";

// Web server
WebServer server(80);

// PID parameters (tunable via web or serial)
float Kp = 25.0, Ki = 0.0, Kd = 0.0;
float recoveryRatio = 0.6; // Ratio of base speed for recovery spin
float targetPosition = 0.0;
float currentPosition = 0.0;
float previousError = 0.0;
float integral = 0.0;
float lastPosition = 0.0;
float deadband = 0.1;
float integralDecay = 0.95;
float maxIntegral = 100.0;
bool targetChanged = false;

unsigned long lastPIDTime = 0;
bool lastPID[5];
unsigned long sampleTime = 10;

const int SENSOR_LEFT_LEFT = 12;
const int SENSOR_LEFT      = 14;
const int SENSOR_CENTER    = 27;
const int SENSOR_RIGHT     = 33;
const int SENSOR_RIGHT_RIGHT = 32;

const int MOTOR_LEFT_PWM  = 10;
const int MOTOR_LEFT_DIR  = 9;
const int MOTOR_RIGHT_PWM = 8;
const int MOTOR_RIGHT_DIR = 7;

const int PWM_FREQ = 1000;
const int PWM_RESOLUTION = 8;
const int PWM_CHANNEL_LEFT = 0;
const int PWM_CHANNEL_RIGHT = 1;

int baseSpeed = 100;
int maxSpeed = 200;
int minSpeed = 0;

// Tasks
TaskHandle_t wifiTaskHandle;
TaskHandle_t controlTaskHandle;

// HTML page
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 PID Tuning</title>
    <style>
      body { font-family: sans-serif; margin: 20px; }
      .controls { margin-bottom: 20px; }
      .control-group { margin: 10px 0; }
      label { margin-right: 10px; }
      input[type="number"] { width: 80px; padding: 5px; }
      button { padding: 8px 16px; background: #007bff; color: white; border: none; cursor: pointer; }
      button:hover { background: #0056b3; }
      .sensor-values {
        margin: 20px 0;
        padding: 10px;
        background: #f8f9fa;
        border-radius: 5px;
      }
      .sensor {
        display: inline-block;
        margin: 0 10px;
        text-align: center;
      }
      .sensor-indicator {
        width: 20px;
        height: 20px;
        border-radius: 50%;
        margin: 5px auto;
        background-color: #ddd;
      }
      .sensor-active {
        background-color: #28a745;
      }
      .status {
        margin: 20px 0;
        padding: 10px;
        background: #e9ecef;
        border-radius: 5px;
      }
    </style>
</head>
<body>
    <h2>ESP32 PID Tuning</h2>

    <div class="sensor-values">
      <h3>Sensor Values</h3>
      <div class="sensor">
        <div id="sensor-ll" class="sensor-indicator"></div>
        <div>LL</div>
      </div>
      <div class="sensor">
        <div id="sensor-l" class="sensor-indicator"></div>
        <div>L</div>
      </div>
      <div class="sensor">
        <div id="sensor-c" class="sensor-indicator"></div>
        <div>C</div>
      </div>
      <div class="sensor">
        <div id="sensor-r" class="sensor-indicator"></div>
        <div>R</div>
      </div>
      <div class="sensor">
        <div id="sensor-rr" class="sensor-indicator"></div>
        <div>RR</div>
      </div>
    </div>

    <div class="status">
      <h3>Status</h3>
      <p>Current Position: <span id="current-pos">0.0</span></p>
      <p>Error: <span id="error">0.0</span></p>
      <p>PID Output: <span id="pid-output">0.0</span></p>
    </div>

    <div class="controls">
      <div class="control-group">
        <label>Kp: <input id="kp" type="number" value="100.0" step="0.1"></label>
        <label>Ki: <input id="ki" type="number" value="0.5" step="0.1"></label>
        <label>Kd: <input id="kd" type="number" value="20.0" step="0.1"></label>
        <button onclick="setPID()">Set PID</button>
      </div>
      
      <div class="control-group">
        <label>Recovery Ratio: <input id="recovery-ratio" type="number" value="0.6" step="0.1" min="0.1" max="1.0"></label>
        <button onclick="setRecoveryRatio()">Set Recovery Ratio</button>
      </div>
      
      <div class="control-group">
        <label>Base Speed: <input id="base-speed" type="number" value="100" min="0" max="255"></label>
        <button onclick="setSpeed()">Set Speed</button>
      </div>
    </div>
  
    <script>
      function setPID() {
        const kp = document.getElementById('kp').value;
        const ki = document.getElementById('ki').value;
        const kd = document.getElementById('kd').value;
        fetch(`/setPID?kp=${kp}&ki=${ki}&kd=${kd}`)
          .then(response => {
            if (!response.ok) {
              console.error('Failed to set PID parameters');
            }
          })
          .catch(error => console.error('Error:', error));
      }
  
      function setRecoveryRatio() {
        const ratio = document.getElementById('recovery-ratio').value;
        fetch(`/setRecoveryRatio?value=${ratio}`)
          .then(response => {
            if (!response.ok) {
              console.error('Failed to set recovery ratio');
            }
          })
          .catch(error => console.error('Error:', error));
      }
      
      function setSpeed() {
        const speed = document.getElementById('base-speed').value;
        fetch(`/setSpeed?value=${speed}`)
          .then(response => {
            if (!response.ok) {
              console.error('Failed to set speed');
            }
          })
          .catch(error => console.error('Error:', error));
      }
  
      function updateSensors(sensors) {
        document.getElementById('sensor-ll').className = 'sensor-indicator' + (sensors.ll ? ' sensor-active' : '');
        document.getElementById('sensor-l').className = 'sensor-indicator' + (sensors.l ? ' sensor-active' : '');
        document.getElementById('sensor-c').className = 'sensor-indicator' + (sensors.c ? ' sensor-active' : '');
        document.getElementById('sensor-r').className = 'sensor-indicator' + (sensors.r ? ' sensor-active' : '');
        document.getElementById('sensor-rr').className = 'sensor-indicator' + (sensors.rr ? ' sensor-active' : '');
      }
      
      function updateStatus(data) {
        document.getElementById('current-pos').textContent = data.current.toFixed(2);
        document.getElementById('error').textContent = data.error.toFixed(2);
        document.getElementById('pid-output').textContent = data.output.toFixed(2);
      }

      function update() {
        fetch('/status')
          .then(res => res.json())
          .then(data => {
            updateSensors(data.sensors);
            updateStatus(data);
          })
          .catch(error => {
            console.error('Error fetching status:', error);
          });
      }
  
      window.addEventListener('load', function() {
        setInterval(update, 200);
      });
    </script>
</body>
</html>
)rawliteral";

// ==== Helper Functions ====

void controlMotors(float pidOutput) {
  int leftSpeed = baseSpeed - pidOutput;
  int rightSpeed = baseSpeed + pidOutput;

  leftSpeed = constrain(leftSpeed, -maxSpeed, maxSpeed);
  rightSpeed = constrain(rightSpeed, -maxSpeed, maxSpeed);

  // Stop motors first
  ledcWrite(PWM_CHANNEL_LEFT, 0);
  ledcWrite(PWM_CHANNEL_RIGHT, 0);
  delayMicroseconds(100);

  // Set directions
  digitalWrite(MOTOR_LEFT_DIR, leftSpeed >= 0 ? HIGH : LOW);
  digitalWrite(MOTOR_RIGHT_DIR, rightSpeed >= 0 ? HIGH : LOW);
  delayMicroseconds(100);

  // Apply PWM values
  ledcWrite(PWM_CHANNEL_LEFT, abs(leftSpeed));
  ledcWrite(PWM_CHANNEL_RIGHT, abs(rightSpeed));
}

float calculateError(bool ll, bool l, bool c, bool r, bool rr) {
  float totalWeight = 0;
  float weightedSum = 0;

  if (ll) { totalWeight += 0.5; weightedSum += -4; }
  if (l)  { totalWeight += 1.0; weightedSum += -2; }
  if (c)  { totalWeight += 1.5; weightedSum += 0; }
  if (r)  { totalWeight += 1.0; weightedSum += 2; }
  if (rr) { totalWeight += 0.5; weightedSum += 4; }

  if (totalWeight == 0) return currentPosition; // No line detected, maintain last position
  return weightedSum / totalWeight;
}

void recoverySpin() {
  // Spin right to find line
  digitalWrite(MOTOR_LEFT_DIR, HIGH);
  digitalWrite(MOTOR_RIGHT_DIR, LOW);
  ledcWrite(PWM_CHANNEL_LEFT, baseSpeed * recoveryRatio);
  ledcWrite(PWM_CHANNEL_RIGHT, baseSpeed * recoveryRatio);
}

void offTapeCorrection(bool lastLeft){
  if (lastLeft) {
    digitalWrite(MOTOR_LEFT_DIR, LOW);
    digitalWrite(MOTOR_RIGHT_DIR, HIGH);
    ledcWrite(PWM_CHANNEL_LEFT, baseSpeed * recoveryRatio);
    ledcWrite(PWM_CHANNEL_RIGHT, baseSpeed * recoveryRatio);
  } else {
    digitalWrite(MOTOR_LEFT_DIR, HIGH);
    digitalWrite(MOTOR_RIGHT_DIR, LOW);
    ledcWrite(PWM_CHANNEL_LEFT, baseSpeed * recoveryRatio);
    ledcWrite(PWM_CHANNEL_RIGHT, baseSpeed * recoveryRatio);
  }
}

// ==== Tasks ====

void wifiTask(void *pvParameters) {
  server.on("/", []() { 
    server.send_P(200, "text/html", htmlPage); 
  });

  server.on("/setPID", []() {
    if (server.hasArg("kp")) Kp = server.arg("kp").toFloat();
    if (server.hasArg("ki")) Ki = server.arg("ki").toFloat();
    if (server.hasArg("kd")) Kd = server.arg("kd").toFloat();
    integral = 0; // Reset integral when PID changes
    server.send(200, "text/plain", "OK");
  });

  server.on("/setRecoveryRatio", []() {
    if (server.hasArg("value")) {
      recoveryRatio = server.arg("value").toFloat();
      recoveryRatio = constrain(recoveryRatio, 0.1, 1.0);
    }
    server.send(200, "text/plain", "OK");
  });
  
  server.on("/setSpeed", []() {
    if (server.hasArg("value")) {
      baseSpeed = server.arg("value").toInt();
      baseSpeed = constrain(baseSpeed, 0, 255);
    }
    server.send(200, "text/plain", "OK");
  });

  server.on("/status", []() {
    float error = targetPosition - currentPosition;
    float pTerm = Kp * error;
    float iTerm = Ki * integral;
    float dTerm = Kd * (lastPosition - currentPosition);
    float output = pTerm + iTerm + dTerm;
    
    String json = "{\"current\":" + String(currentPosition, 2) +
                  ",\"error\":" + String(error, 2) +
                  ",\"output\":" + String(output, 2) +
                  ",\"sensors\":{" +
                  "\"ll\":" + (digitalRead(SENSOR_LEFT_LEFT) ? "true" : "false") +
                  ",\"l\":" + (digitalRead(SENSOR_LEFT) ? "true" : "false") +
                  ",\"c\":" + (digitalRead(SENSOR_CENTER) ? "true" : "false") +
                  ",\"r\":" + (digitalRead(SENSOR_RIGHT) ? "true" : "false") +
                  ",\"rr\":" + (digitalRead(SENSOR_RIGHT_RIGHT) ? "true" : "false") +
                  "}}";
    server.send(200, "application/json", json);
  });

  server.begin();

  while (true) {
    server.handleClient();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void controlTask(void *pvParameters) {
  // Setup pins
  pinMode(SENSOR_LEFT_LEFT, INPUT);
  pinMode(SENSOR_LEFT, INPUT);
  pinMode(SENSOR_CENTER, INPUT);
  pinMode(SENSOR_RIGHT, INPUT);
  pinMode(SENSOR_RIGHT_RIGHT, INPUT);
  
  pinMode(MOTOR_LEFT_DIR, OUTPUT);
  pinMode(MOTOR_RIGHT_DIR, OUTPUT);
  
  // Setup PWM
  ledcSetup(PWM_CHANNEL_LEFT, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(PWM_CHANNEL_RIGHT, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(MOTOR_LEFT_PWM, PWM_CHANNEL_LEFT);
  ledcAttachPin(MOTOR_RIGHT_PWM, PWM_CHANNEL_RIGHT);

  unsigned long lastPrint = 0;
  lastPIDTime = millis();

  while (true) {
    unsigned long now = millis();
    
    // PID control runs at specified sample time
    if (now - lastPIDTime >= sampleTime) {
      // Read sensors
      bool sLL = digitalRead(SENSOR_LEFT_LEFT);
      bool sL  = digitalRead(SENSOR_LEFT);
      bool sC  = digitalRead(SENSOR_CENTER);
      bool sR  = digitalRead(SENSOR_RIGHT);
      bool sRR = digitalRead(SENSOR_RIGHT_RIGHT);

      float output, error, pTerm, iTerm, dTerm;
      if (!sLL && !sL && !sC && !sR && !sRR) {
          float lastLL = lastPID[1];
          float lastRR = lastPID[3];

          if (lastLL){
            controlMotors(baseSpeed * recoveryRatio);
          }
          if (lastRR){
            controlMotors(baseSpeed * recoveryRatio * -1);
          }
      } else{
        currentPosition = calculateError(sLL, sL, sC, sR, sRR);
      
        // Calculate error
        float error = targetPosition - currentPosition;
  
        // PID calculations
        float deltaTime = (now - lastPIDTime) / 1000.0; // Convert to seconds
        
        // Proportional term
        float pTerm = Kp * error;
        
        // Integral term with decay and limits
        integral = integral * integralDecay + (error * deltaTime);
        integral = constrain(integral, -maxIntegral, maxIntegral);
        float iTerm = Ki * integral;
        
        // Derivative term (on measurement to avoid derivative kick)
        float dTerm = Kd * (lastPosition - currentPosition) / deltaTime;
        lastPosition = currentPosition;
        
        // Calculate final output
        output = pTerm + iTerm + dTerm;

        controlMotors(output);

        if (now - lastPrint > 500) {
          Serial.printf("LL:%d L:%d C:%d R:%d RR:%d | Pos:%.2f | Err:%.2f | P:%.2f I:%.2f D:%.2f | Out:%.2f\n",
                        sLL, sL, sC, sR, sRR, 
                        currentPosition, error, pTerm, iTerm, dTerm, output);
          lastPrint = now;
        }
      }
      
      lastPIDTime = now;

      lastPID[0] = sLL;
      lastPID[1] = sL;
      lastPID[2] = sC;
      lastPID[3] = sR;
      lastPID[4] = sRR;

    }

    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

// ==== Main Setup ====

void setup() {
  Serial.begin(115200);
  
  // Set up WiFi Access Point
  WiFi.softAP(ssid, password);
  IPAddress apIP = WiFi.softAPIP();

  Serial.println("Access Point Started");
  Serial.print("Connect to WiFi: ");
  Serial.println(ssid);
  Serial.print("Then open browser to: http://");
  Serial.println(apIP);

  // Create tasks on different cores
  xTaskCreatePinnedToCore(wifiTask, "WiFiTask", 8192, NULL, 1, &wifiTaskHandle, 0);
  xTaskCreatePinnedToCore(controlTask, "ControlTask", 4096, NULL, 2, &controlTaskHandle, 1);
}

void loop() {
  // Nothing needed in loop - tasks handle everything
  delay(1000);
}