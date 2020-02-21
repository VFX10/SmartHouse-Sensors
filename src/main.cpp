#include <Arduino.h>
#include <libs/MqttHelper/MqttHelper.h>
#include <libs/WiFiConfig/Wifihelper.h>
#include <libs/OTAHelper/OTAHelper.h>
#include <config.h>
#include <Sensor.h>
WiFiHelper *wifi;
MqttHelper *mqttClient;
DynamicJsonDocument doc(1024);
DynamicJsonDocument prejson(1024);
auto buttons = new HardwareButtons();
Sensor *sensor;

void setup()
{
  Serial.begin(9600);
  Config *config = new Config;

  config->initPinsToGetConfig();

  switch (sensorType)
  {
  case SENSOR_UV:
    sensor = new UVSensor();
    wifi = new WiFiHelper();
    break;
  case SENSOR_DOOR:
    sensor = new InlineContact();
    wifi = new WiFiHelper();
    break;
  case SENSOR_GAS_AND_SMOKE:
    sensor = new GasAndSmokeSensor();
    wifi = new WiFiHelper();
    break;
  case SENSOR_LIGHT:
    sensor = new LightSensor();
    break;
  case SENSOR_SWITCH:
    sensor = new Relay();
    wifi = new WiFiHelper(sensor);
    break;
  case SENSOR_TEMP_AND_HUMIDITY:
    sensor = new TempSensor();
    wifi = new WiFiHelper();
    break;
  default:
    sensor = nullptr;
    break;
  }
  wifi->begin();
  mqttClient = new MqttHelper();
  mqttClient->initMqtt();
  int timeout = 0;
  while (timeout++ < 10)
  {

    if (mqttClient->isConnected())
    {
      DynamicJsonDocument json(1024);
      deserializeJson(json, config->readConfig());
      String data;
      serializeJson(json, data);
      mqttClient->publish("SensorsConfigChannel", data);
      break;
    }
    else if (timeout >= 10)
    {
      Serial.println("Module can't be registered, please reset it. Abort All!");
      // buttons->instantReset();
    }
    else
    {
      mqttClient->connect(WiFi.macAddress());
    }
    Serial.print('.');
    delay(1000);
  }
  auto conf = config->readConfig();
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, conf);
  String server = doc["sensorName"];
  Serial.println(server);
  initOTA(doc["sensorName"]);
}
bool checkData(String origin, String comp)
{
  for (size_t i = 0; i < origin.length(); i++)
  {
    if (origin[i] != comp[i])
    {
      return false;
    }
  }
  return true;
}

void loop()
{
  ArduinoOTA.handle();
  if (mqttClient->isConnected())
  {
    DynamicJsonDocument json(1024);

    json["macAddress"] = WiFi.macAddress();
    json["data"] = sensor->read();
    String data, prevData;
    serializeJson(json, data);
    if (json["data"] != prejson["data"])
    {
      serializeJsonPretty(json, Serial);
      prejson["data"] = json["data"];
      Serial.println("Data changed. Send it to server");
      mqttClient->publish("SensorsDataChannel", data.c_str());
    }
  }
  else
  {
    mqttClient->connect(WiFi.macAddress());
    Serial.println("Lost Connection to MQTT Server! Reconnect!");
  }
  //delay(atoi(wifi.freqMinutes.c_str()) * 60 * 1000);
  delay(1000);
}