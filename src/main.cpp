#include <Arduino.h>
#include <libs/TempSensor.h>
#include <libs/MqttHelper.h>
#include <libs/WebRequests.h>

IPAddress *addr = new IPAddress();
WebRequests webRequests;
int *mqttPort = new int(1883);
MqttHelper *mqttClient = new MqttHelper(addr, mqttPort);
void setup()
{
  pinMode(D8, OUTPUT);
  Serial.begin(9600);
  digitalWrite(D8, HIGH);
  setupWifiManager();
  webRequests = WebRequests(&server, &port);
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
    //mqttClient->publish("SensorsDataChannel", data.c_str());
  }
  mqttClient->client.loop();


  delay(10000);

}