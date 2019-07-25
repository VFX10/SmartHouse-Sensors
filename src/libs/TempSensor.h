#include <dht11.h>
#include <DHT.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#define DHT11PIN A0
DHT dht(DHT11PIN, DHT11);
long freqRead;
float readTemperature();
float readHumidity();


void tempSensorInit(const uint8_t port, const int freq)
{
    dht = DHT(port, DHT11);
    dht.begin();
    freqRead = (1000 * 60) * freq;
}

String readDataFromSensor()
{
    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();
    json["temperature"] = readTemperature();
    json["humidity"] = readHumidity();

    String jsonString;
    json.printTo(jsonString);
    return jsonString;
    //delay(freqRead);
}

float readTemperature()
{
    return (float)dht.readTemperature();
}
float readHumidity()
{
    return (float)dht.readHumidity();
}