#include <Arduino.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <libs/LightSensor/LightSensorDefinition.hpp>

String LightSensor::ReadLight()
{
    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();
    json["light"] = this->lightMeter.readLightLevel();
    String data;
    json.prettyPrintTo(data);
    json.prettyPrintTo(Serial);
    return data;
}
LightSensor::LightSensor()
{
    Wire.begin(SDA_DEFAULT_PIN, SCL_DEFAULT_PIN);
    if (lightMeter.begin(READING_MODE_DEFAULT_PIN))
    {
        Serial.println(F("BH1750 begin with default values"));
    }
    else
    {
        Serial.println(F("Error initialising BH1750 with default values"));
    }
}
LightSensor::LightSensor(int sda, int scl, BH1750::Mode readingMode)
{
    Wire.begin(sda, scl);
    if (lightMeter.begin(readingMode))
    {
        Serial.println(F("BH1750 begin with custom values"));
    }
    else
    {
        Serial.println(F("Error initialising BH1750 with custom values"));
    }
}
