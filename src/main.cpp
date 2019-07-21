#include <Arduino.h>
#include <libs/ConfigButtons.h>
#include <libs/TempSensor.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <libs/WebRequests.h>

// Update these with values suitable for your network.
void mqttConnect();

void callback(char *topic, byte *payload, unsigned int length)
{
  // handle message arrived
  Serial.println(topic);
}
WiFiClient espClient;
PubSubClient client(espClient);
WebRequests webRequests;
void setup()
{
  Serial.begin(9600);
  ConfigButtons(D1, D3);
  setupWifiManager();
  tempSensorInit(D4, atoi(freqMinutes.c_str()));
  client.setServer(server.c_str(), 1883);
  client.setCallback(callback);
  webRequests = WebRequests(&server, &port);
  Serial.println(server);
  mqttConnect();
  webRequests.Post("/api/registerSensor", "{\"sensorName\": \""+ sensorName + "\",\"macAddress\": \""+ WiFi.macAddress() + "\",\"ipAddress\": \"" + WiFi.localIP().toString() + "\" }");
  //Serial.println("Going into deep sleep for " + String(deepSleepFreq) + " minutes");
  //ESP.(deepSleepFreq * (60e6));
  //node-red
}

void mqttConnect()
{
    client.connect(sensorName.c_str());
}
void loop()
{
  if (!client.connected())
  {
    Serial.println("Nu is conectat la Mqtt. Ce sa fac?");
    mqttConnect();
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
    client.publish(sensorName.c_str(), data.c_str());
  }

  client.loop();
  delay(10000);
}