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
  Serial.begin(9600);

  setupWifiManager();
  webRequests = WebRequests(&server, &port);
  addr->fromString(server);
  mqttClient->initMqtt();

  tempSensorInit(D4, atoi(freqMinutes.c_str()));
  //mqttClient.client.connect(sensorName.c_str());
  mqttClient->connect(WiFi.macAddress());
  //mqttClient->client.setCallback(callback);
  webRequests.Post("/api/registerSensor", "{\"sensorName\":\"" + sensorName + "\",\"macAddress\":\"" + WiFi.macAddress() + "\",\"sensorType\":\"temperature and humidity\",\"readingFrequency\":" + freqMinutes + "}");
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
    mqttClient->publish("SensorsDataChannel", data.c_str());
  }
  mqttClient->client.loop();
  delay(10000);
}