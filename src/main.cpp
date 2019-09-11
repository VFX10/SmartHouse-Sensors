#include <Arduino.h>
#include <libs/TempSensor/TempSensor.h>
#include <libs/MqttHelper/MqttHelper.h>
#include <libs/Relay/Relay.h>
#include <libs/LightSensor/LightSensor.h>
#include <libs/InlineContact/InlineContact.h>
#include <libs/GasAndSmokeSensor/GasAndSmokeSensor.h>
#include <libs/UV_Sensor/UVSensor.h>
#include <libs/WiFiConfig/Wifihelper.h>
#include <LiquidCrystal_I2C.h>
// TODO: AP Password will be the Mac Address of the ESP8266
WiFiHelper wifi("Door Sensor");

IPAddress *addr = new IPAddress();
int *mqttPort = new int(1883);
MqttHelper *mqttClient = new MqttHelper(addr, mqttPort);
//LightSensor lightSensor;
Relay relay;

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display
//InlineContact contact;
//GasAndSmokeSensor mq;
//UVSensor UV;
//TempSensor tmp;



void setup()
{
  Serial.begin(9600);
  lcd.init(D6, D7); // initialize the lcd
  lcd.backlight();
  //if(digitalRead(RESET_BUTTON_DEFAULT_PIN)) {

  wifi.begin();
  addr->fromString(wifi.server);
  mqttClient->initMqtt();
  if (mqttClient->isConnected())
  {
    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();
    json["macAddress"] = WiFi.macAddress();
    json["sensorName"] = wifi.sensorName;
    json["sensorType"] = wifi.sensorType;
    json["readingFrequency"] = wifi.freqMinutes;
    String data;
    json.prettyPrintTo(data);
    json.prettyPrintTo(Serial);

    mqttClient->publish("SensorsConfigChannel", data.c_str());
  }
  else
  {
    Serial.println("Module can't be registered, please reset it. Abort All!");
  }
  //}
}
void loop()
{
  if (mqttClient->isConnected())
  {
    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();
    json["macAddress"] = WiFi.macAddress();
    json["data"] = relay.read();
    String data;
    json.prettyPrintTo(data);
    //json.prettyPrintTo(Serial);
    lcd.clear();
    lcd.print("Switch Module:");
    lcd.setCursor(0, 1);
    lcd.print(relay.read());
    //mqttClient->publish("SensorsDataChannel", data.c_str());
  }
  else
  {
    mqttClient->connect(WiFi.macAddress());
    Serial.println("Lost Connection to MQTT Server! Reconnect!");
  }



  // delay(atoi(wifi.freqMinutes.c_str()) * 60 * 1000);
  delay(1000);
}