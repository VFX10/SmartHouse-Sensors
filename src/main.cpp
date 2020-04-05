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
  DynamicJsonDocument configJson(1024);

String name;
// auto buttons = new HardwareButtons();
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
    mqttClient = new MqttHelper();
    break;
  case SENSOR_DOOR:
    sensor = new InlineContact();
    wifi = new WiFiHelper();
    mqttClient = new MqttHelper();
    break;
  case SENSOR_GAS_AND_SMOKE:
    sensor = new GasAndSmokeSensor();
    wifi = new WiFiHelper();
    mqttClient = new MqttHelper();
    break;
  case SENSOR_LIGHT:
    sensor = new LightSensor();
    mqttClient = new MqttHelper();
    wifi = new WiFiHelper();

    break;
  case SENSOR_SWITCH:
    sensor = new Relay();
    wifi = new WiFiHelper(sensor);
    mqttClient = new MqttHelper(config, sensor);
    break;
  case SENSOR_TEMP_AND_HUMIDITY:
    sensor = new TempSensor();
    wifi = new WiFiHelper();
    mqttClient = new MqttHelper();
    break;
  default:
    sensor = nullptr;
    mqttClient = new MqttHelper();
    wifi = new WiFiHelper();

    break;
  }
  Serial.println("begin wifi");
  wifi->begin();
  Serial.println("end wifi");
  deserializeJson(configJson, config->readConfig());
  DynamicJsonDocument sensorName(1024);
  sensorName["name"] = WiFi.macAddress();
  sensorName["account"] = configJson["account"];

  serializeJson(sensorName, name);
  Serial.println(name);
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
  Serial.println(name);

      mqttClient->connect(name);
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
String readData()
{
  DynamicJsonDocument json(1024);
  if (sensorType == SENSOR_TEMP_AND_HUMIDITY)
  {

    DynamicJsonDocument sensorData = sensor->read();

    json["macAddress"] = WiFi.macAddress();
    json["data"] = sensorData;
    json["account"] = configJson["account"];
    String data, prevData;
    serializeJson(json, data);
    int tempDiference = (int)json["data"]["temperature"] - (int)prejson["data"]["temperature"];
    int humidityDiference = (int)json["data"]["humidity"] - (int)prejson["data"]["humidity"];
    Serial.println(sqrt(pow(tempDiference, 2)));
    Serial.println(sqrt(pow(humidityDiference, 2)));

    if ((int)json["data"]["temperature"] <= 100 && (int)json["data"]["humidity"] <= 100 && (sqrt(pow(tempDiference, 2)) >= 2 || sqrt(pow(humidityDiference, 2)) >= 2))
    {
      serializeJsonPretty(json, Serial);

      prejson["data"] = json["data"];
      Serial.println("Data changed. Send it to server");
      return data;
    }
  }
  else
  {
    DynamicJsonDocument sensorData = sensor->read();

    json["macAddress"] = WiFi.macAddress();
    json["data"] = sensorData;
    json["account"] = configJson["account"];
    String data, prevData;
    serializeJson(json, data);
    if (json["data"] != prejson["data"])
    {
      serializeJsonPretty(json, Serial);
      prejson["data"] = json["data"];
      Serial.println("Data changed. Send it to server");
      return data;
    }
  }
  return "";
}

void loop()
{
  ArduinoOTA.handle();
  if (mqttClient->isConnected())
  {
    String data = readData();
    if (data != "")
    {
      mqttClient->publish("SensorsDataChannel", data.c_str());
    }
  }
  else
  {
    mqttClient->connect(name);
    Serial.println("Lost Connection to MQTT Server! Reconnect!");
  }
  //delay(atoi(wifi.freqMinutes.c_str()) * 60 * 1000);
  delay(1000);
}