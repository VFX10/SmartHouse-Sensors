#include <Arduino.h>
#include <libs/TempSensor.h>
#include <libs/MqttHelper.h>
#include <libs/WebRequests.h>


WebRequests webRequests;
MqttHelper mqttClient;
//ConfigButtons hardwareButtons(D1, D3);

void setup()
{
  Serial.begin(9600);
  setupWifiManager();
  tempSensorInit(D4, atoi(freqMinutes.c_str()));
  webRequests = WebRequests(&server, &port);
  mqttClient = MqttHelper(server, 1883);
  mqttClient.connect();
  webRequests.Post("/api/registerSensor", "{\"sensorName\":\"" + sensorName + "\",\"macAddress\":\"" + WiFi.macAddress() + "\",\"sensorType\":\"temperature and humidity\"}");
  //Serial.println("Going into deep sleep for " + String(deepSleepFreq) + " minutes");
  //ESP.(deepSleepFreq * (60e6));
  //node-red
}


void loop()
{
  if (!mqttClient.isConnected())
  {
    Serial.println("Nu is conectat la Mqtt. Ce sa fac?");
    mqttClient.connect();
  }
  else
  {
    //"Temperaturte and humidity", readDataFromSensor()
    Serial.println("Sunt conectat. Trimit date!");
    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();
    json["macAddress"] = WiFi.macAddress();
    json["data"] = readDataFromSensor();
    String data;
    json.prettyPrintTo(data);
    mqttClient.publish("SensorsDataChannel", data.c_str());
  }

  //client.loop();
  delay(10000);
}