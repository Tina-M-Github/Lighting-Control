#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

// Define pins for sensors and actuators
#define PIR_PIN D1         // PIR Motion Sensor
#define IR_PIN D2          // IR Sensor
#define DHT_PIN D3         // DHT11 Sensor
#define LDR_PIN A0         // LDR Sensor (Analog pin)
#define LED_PIR D5         // LED controlled by PIR
#define LED_IR D6          // LED controlled by IR
#define LED_LDR D7         // LED controlled by LDR
#define FAN_PIN D8         // Fan controlled by temperature/humidity

// DHT11 settings
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

// Temperature and humidity threshold
float tempThreshold = 30.0; // Celsius
float humThreshold = 70.0;  // Percentage

// Blynk auth token
char auth[] = "YourAuthToken"; // Replace with your Blynk auth token

// WiFi credentials
char ssid[] = "YourNetworkName"; // Replace with your WiFi SSID
char pass[] = "YourPassword";    // Replace with your WiFi password

BlynkTimer timer;

bool manualMode = false;
bool manualPIRLEDState = false;
bool manualIRLEDState = false;
bool manualLDRLEDState = false;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize the DHT sensor
  dht.begin();

  // Initialize pins
  pinMode(PIR_PIN, INPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(LED_PIR, OUTPUT);
  pinMode(LED_IR, OUTPUT);
  pinMode(LED_LDR, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);

  // Ensure all LEDs and fan are off at the start
  digitalWrite(LED_PIR, LOW);
  digitalWrite(LED_IR, LOW);
  digitalWrite(LED_LDR, LOW);
  digitalWrite(FAN_PIN, LOW);

  // Initialize Blynk
  Blynk.begin(auth, ssid, pass);

  // Setup a timer to run every 2 seconds
  timer.setInterval(2000L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();

  if (!manualMode) {
    // Automatic mode: Read sensor values and control devices
    bool pirState = digitalRead(PIR_PIN);
    bool irState = digitalRead(IR_PIN);
    int ldrValue = analogRead(LDR_PIN);
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Control LED with PIR sensor
    if (pirState) {
      digitalWrite(LED_PIR, HIGH);
    } else {
      digitalWrite(LED_PIR, LOW);
    }

    // Control LED with IR sensor
    if (irState) {
      digitalWrite(LED_IR, HIGH);
    } else {
      digitalWrite(LED_IR, LOW);
    }

    // Control LED with LDR sensor
    if (ldrValue < 500) {  // Adjust the threshold value as needed
      digitalWrite(LED_LDR, HIGH);
    } else {
      digitalWrite(LED_LDR, LOW);
    }

    // Control fan with temperature and humidity
    if (temperature > tempThreshold || humidity > humThreshold) {
      digitalWrite(FAN_PIN, HIGH);
    } else {
      digitalWrite(FAN_PIN, LOW);
    }
  } else {
    // Manual mode: Control LEDs based on Blynk app input
    digitalWrite(LED_PIR, manualPIRLEDState ? HIGH : LOW);
    digitalWrite(LED_IR, manualIRLEDState ? HIGH : LOW);
    digitalWrite(LED_LDR, manualLDRLEDState ? HIGH : LOW);
  }
}

void sendSensorData() {
  // Read sensor values
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int ldrValue = analogRead(LDR_PIN);

  // Send sensor values to Blynk app
  Blynk.virtualWrite(V5, temperature);
  Blynk.virtualWrite(V6, humidity);
}

BLYNK_WRITE(V0) {
  // Mode control (manual or automatic)
  manualMode = param.asInt();
}

BLYNK_WRITE(V1) {
  // Manual control of PIR LED
  manualPIRLEDState = param.asInt();
}

BLYNK_WRITE(V2) {
  // Manual control of IR LED
  manualIRLEDState = param.asInt();
}

BLYNK_WRITE(V3) {
  // Manual control of LDR LED
  manualLDRLEDState = param.asInt();
}
