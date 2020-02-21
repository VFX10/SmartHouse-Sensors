#pragma once

#include <Arduino.h>
#pragma once

#include <ArduinoJson.h>
#include <libs/TempSensor/TempSensorDefinition.hpp>

TempSensor::TempSensor(int pin = DHT11_DEFAULT_PIN)
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
    DynamicJsonDocument json(1024);
    json["temperature"] = readTemperature();
    json["humidity"] = readHumidity();

    String jsonString;
    serializeJson(json, jsonString);
    // json.prettyPrintTo(Serial);
    return jsonString;
}