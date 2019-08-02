#include <Arduino.h>
#include <libs/TempSensor/TempSensor.h>
#include <libs/MqttHelper/MqttHelper.h>
#include <libs/Relay/Relay.h>
#include <libs/LightSensor/LightSensor.h>
#include <libs/InlineContact/InlineContact.h>
#include <libs/GasAndSmokeSensor/GasAndSmokeSensor.h>
#include <libs/UV_Sensor/UVSensor.h>
#include <libs/WiFiConfig/Wifihelper.h>

IPAddress *addr = new IPAddress();
int *mqttPort = new int(1883);
MqttHelper *mqttClient = new MqttHelper(addr, mqttPort);
LightSensor lightSensor;
Relay relay;
InlineContact contact;
GasAndSmokeSensor mq;
UVSensor UV;
TempSensor tmp;
WiFiHelper wifi("Door Sensor");
void setup()
{
  Serial.begin(9600);
  wifi.begin();
  addr->fromString(wifi.server);
  mqttClient->initMqtt();
  mqttClient->connect(WiFi.macAddress());
}

void loop()
{
  if (!mqttClient->isConnected())
  {
    Serial.println("Nu is conectat la Mqtt. Ce sa fac?");
    mqttClient->connect(WiFi.macAddress());
  }


  if (mqttClient->isConnected())
  {

    Serial.println("Sunt conectat. Trimit date!");
    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();
    json["macAddress"] = WiFi.macAddress();
    json["data"] = relay.read();
    String data;
    json.prettyPrintTo(data);
    json.prettyPrintTo(Serial);
    //mqttClient->publish("SensorsDataChannel", data.c_str());
  }
  delay(1000);
}