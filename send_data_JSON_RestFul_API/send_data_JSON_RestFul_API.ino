#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2c

const char* ssid = "ekmekci";
const char* password = "1234567890";

unsigned long lm;
unsigned long delayTime;
bool status;
String JsonData = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(500);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the Wifi network");
  status = bme.begin(0x76);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (WiFi.status() == WL_CONNECTED) {
    String id_alat = "ALAT01";
    float TEMPERATURE = bme.readTemperature();
    float HUMIDITY = bme.readHumidity();
    float PRESSURE = bme.readPressure() / 100.0F;

    StaticJsonDocument<200> doc;
    doc["ID_ALAT"] = id_alat;
    doc["TEMPERATURE"] = TEMPERATURE;
    doc["HUMIDITY"] = HUMIDITY;
    doc["PRESSURE"] = PRESSURE;

    serializeJson(doc, JsonData);
    Serial.print(JsonData);
    Serial.println();

    // String url = "http://192.168.1.35:3000/api/postData";
    String url = "http://192.168.1.35:3000/api/sensor";
    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type", "application/json"); // corrected header
    int httpresponseCode = http.POST(JsonData);
    if (httpresponseCode > 0) {
      Serial.println(httpresponseCode);
      String payload = http.getString();
      Serial.println(payload);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpresponseCode);
    }
    http.end();
  } else {
    Serial.println("Error in WiFi connection"); // corrected Serial
  }
  delay(1000);
  JsonData = "";
}
