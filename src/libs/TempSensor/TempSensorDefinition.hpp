#pragma once
#include <Arduino.h>
#include <dht11.h>
#include <DHT.h>

#define DHT11_DEFAULT_PIN D5
class TempSensor
{

private:
    DHT *dht = new DHT(DHT11_DEFAULT_PIN, DHT11);
    float readTemperature();
    float readHumidity();

public:
    TempSensor();
    TempSensor(uint8_t);
    String read();
};
