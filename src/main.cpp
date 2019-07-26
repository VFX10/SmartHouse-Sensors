#include <Arduino.h>
#include <libs/TempSensor.h>
#include <libs/MqttHelper.h>
#include <libs/WebRequests.h>
#include <libs/SwitchRelay.h>
#include <libs/LightSensor.h>
#include <libs/InlineContact.h>

uint8_t *RELAY_PIN = new uint8_t(D7);
uint8_t *CONTACT_PIN = new uint8_t(D8);

IPAddress *addr = new IPAddress();
int *mqttPort = new int(1883);
MqttHelper *mqttClient = new MqttHelper(addr, mqttPort);
LightSensor lightSensor(D5, D6, BH1750::CONTINUOUS_HIGH_RES_MODE_2);
// SwitchRelay relay(RELAY_PIN);
InlineContact contact(CONTACT_PIN);

void setup()
{
pinMode(D7, OUTPUT);
  Serial.begin(9600);

  setupWifiManager();
  addr->fromString(server);
  mqttClient->initMqtt();

  tempSensorInit(D4, atoi(freqMinutes.c_str()));
  //mqttClient.client.connect(sensorName.c_str());
  mqttClient->connect(WiFi.macAddress());
  //mqttClient->client.setCallback(callback);

  DynamicJsonBuffer jsonBuffer;
  JsonObject &json = jsonBuffer.createObject();
  json["macAddress"] = WiFi.macAddress();
  json["sensorName"] = sensorName;
  json["sensorType"] = sensorType;
  json["readingFrequency"] = freqMinutes;
  String data;
  json.prettyPrintTo(data);

  mqttClient->publish("SensorsConfigChannel", data.c_str());
  //Serial.println("Going into deep sleep for " + String(deepSleepFreq) + " minutes");
  //ESP.(deepSleepFreq * (60e6));
  //node-red
}

void loop()
{
  if (!mqttClient->isConnected())
  {
    Serial.println("Nu is conectat la Mqtt. Ce sa fac?");
    mqttClient->connect(sensorName);
  }
  if (mqttClient->isConnected())
  {

    Serial.println("Sunt conectat. Trimit date!");
    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();
    json["macAddress"] = WiFi.macAddress();
    json["data"] = readDataFromSensor();
    String data;
    json.prettyPrintTo(data);
    json.prettyPrintTo(Serial);
    //mqttClient->publish("SensorsDataChannel", data.c_str());
  }
  float sensorVoltage;
  float sensorValue;

  sensorValue = analogRead(A0);
  sensorVoltage = sensorValue/1024*3.3;
  Serial.print("sensor reading = ");
  Serial.print(sensorValue);
  Serial.println("");
  Serial.print("sensor voltage = ");
  Serial.print(sensorVoltage);
  Serial.println(" V");
  // delay(1000);
  Serial.print("Contact status: ");
  Serial.println(contact.getState());
  lightSensor.ReadLight();
  tone(D7, 2000, 1000);
  delay(1000);
  //noTone(D7);
}