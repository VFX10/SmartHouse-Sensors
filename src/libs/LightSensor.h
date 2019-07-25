#include <Arduino.h>
#include <Wire.h>
#include <BH1750.h>
#include <ArduinoJson.h>

class LightSensor
{
    BH1750 lightMeter;

public:
    LightSensor();
    LightSensor(int, int, BH1750::Mode);
    String ReadLight()
    {
        DynamicJsonBuffer jsonBuffer;
        JsonObject &json = jsonBuffer.createObject();
        json["light"] = lightMeter.readLightLevel();
        String data;
        json.prettyPrintTo(data);
        json.prettyPrintTo(Serial);
        return data;
    }
};
LightSensor::LightSensor()
{
}
LightSensor::LightSensor(int sda, int scl, BH1750::Mode readingMode)
{
    Wire.begin(sda, scl);
    if (lightMeter.begin(readingMode))
    {
        Serial.println(F("BH1750 begin"));
    }
    else
    {
        Serial.println(F("Error initialising BH1750"));
    }
}
