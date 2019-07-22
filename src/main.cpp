#include <Arduino.h>
#include <libs/TempSensor.h>
#include <libs/MqttHelper.h>
#include <libs/WebRequests.h>

WebRequests webRequests;
MqttHelper mqttClient(IPAddress(10,3,141,1), 1883);
// 192.168.101.184
void setup()
{
  Serial.begin(9600);

Serial.println("Saving configuration file");
        DynamicJsonBuffer jsonBuffer;
        JsonObject &json = jsonBuffer.createObject();
        json["server"] = "10.3.141.1";
        json["port"] = "8000";
        json["freqMinutes"] = "5";
        json["sensorName"] = WiFi.macAddress();
        json["sensorType"] = "temperature and humidity";

        File configFile = SPIFFS.open("/config.json", "w");

        json.printTo(Serial);
        json.printTo(configFile);
        configFile.close();

  setupWifiManager();
  webRequests = WebRequests(&server, &port);

  tempSensorInit(D4, atoi(freqMinutes.c_str()));
  //mqttClient.client.connect(sensorName.c_str());
   mqttClient.connect(sensorName);
 webRequests.Post("/api/registerSensor", "{\"sensorName\":\"" + sensorName + "\",\"macAddress\":\"" + WiFi.macAddress() + "\",\"sensorType\":\"temperature and humidity\",\"readingFrequency\":" + freqMinutes + "}");
  //Serial.println("Going into deep sleep for " + String(deepSleepFreq) + " minutes");
  //ESP.(deepSleepFreq * (60e6));
  //node-red
}

void loop()
{
  if (!mqttClient.isConnected())
  {
    Serial.println("Nu is conectat la Mqtt. Ce sa fac?");
    mqttClient.connect(sensorName);
  }
  if (mqttClient.isConnected())
  {

    Serial.println("Sunt conectat. Trimit date!");
    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();
    json["macAddress"] = WiFi.macAddress();
    json["data"] = readDataFromSensor();
    String data;
    json.prettyPrintTo(data);
    mqttClient.publish("SensorsDataChannel", data.c_str());
  }
  delay(10000);
}