#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <libs/TempSensor/TempSensorDefinition.hpp>

TempSensor::TempSensor()
{
    this->dht->begin();
}
TempSensor::TempSensor(uint8_t pin)
{
    this->dht = new DHT(pin, DHT11);
    this->dht->begin();
}

float TempSensor::readTemperature()
{
    return (float)this->dht->readTemperature();
}
float TempSensor::readHumidity()
{
    return (float)this->dht->readHumidity();
}

String TempSensor::read()
{
    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();
    json["temperature"] = readTemperature();
    json["humidity"] = readHumidity();

    String jsonString;
    json.printTo(jsonString);
    json.prettyPrintTo(Serial);
    return jsonString;
}