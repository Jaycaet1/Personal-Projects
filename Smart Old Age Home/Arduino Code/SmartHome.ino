#include <WiFi.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"

// Wi-Fi Credentials
const char* ssid = "group115";
const char* password = "group115";

// Flask server endpoint
const char* serverUrl = "http://192.168.43.164:5000/data";

// DS18B20 Temperature sensor setup
#define ONE_WIRE_BUS 14
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds18b20(&oneWire);

// DHT22 sensor setup
#define DHTPIN 23
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// PIR motion sensor and motion LED
#define PIR_PIN 12
#define MOTION_LED 19

// LDR sensors
#define LDR_INSIDE_PIN 34
#define LDR_OUTSIDE_PIN 39

// Grow light LED pin
#define GROW_LIGHT_PIN 23

unsigned long motionTimer = 0;
bool motionActive = false;

void connectToWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    Serial.print(".");
    delay(500);
    retries++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println(" Failed to connect to WiFi.");
  }
}

void setup() {
  Serial.begin(115200);

  // Pin modes
  pinMode(PIR_PIN, INPUT);
  pinMode(MOTION_LED, OUTPUT);
  pinMode(GROW_LIGHT_PIN, OUTPUT);

  dht.begin();
  ds18b20.begin();

  connectToWiFi();
}

void loop() {
  ds18b20.requestTemperatures();
  float temperature = ds18b20.getTempCByIndex(0);
  float humidity = dht.readHumidity();
  int motion = digitalRead(PIR_PIN);
  int ldr_inside = analogRead(LDR_INSIDE_PIN);
  int ldr_outside = analogRead(LDR_OUTSIDE_PIN);

  // Grow light control logic
  if (temperature < 25 && ldr_inside < 1500) {
    digitalWrite(GROW_LIGHT_PIN, HIGH);
  } else if (temperature > 30 || ldr_inside > 2500) {
    digitalWrite(GROW_LIGHT_PIN, LOW);
  }

  // PIR motion logic
  if (motion == HIGH) {
    digitalWrite(MOTION_LED, HIGH);
    motionTimer = millis();
    motionActive = true;
    Serial.println("Motion Detected");
  }
  if (motionActive && millis() - motionTimer > 7000) {
    digitalWrite(MOTION_LED, LOW);
    motionActive = false;
  }

  // Create JSON payload
  String jsonData = "{";
  jsonData += "\"temperature\":" + String(temperature, 2) + ",";
  jsonData += "\"humidity\":" + String(humidity, 2) + ",";
  jsonData += "\"motion\":" + String(motion) + ",";
  jsonData += "\"ldr_inside\":" + String(ldr_inside) + ",";
  jsonData += "\"ldr_outside\":" + String(ldr_outside);
  jsonData += "}";

  Serial.println(jsonData);

  // Send data to Flask server
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");
    int responseCode = http.POST(jsonData);

    if (responseCode > 0) {
      Serial.print("Server response: ");
      Serial.println(http.getString());
    } else {
      Serial.println("Failed to send data. HTTP error: " + String(responseCode));
    }
    http.end();
  } else {
    Serial.println("WiFi not connected, retrying...");
    connectToWiFi(); // Attempt reconnection
  }

  delay(2500);
}
