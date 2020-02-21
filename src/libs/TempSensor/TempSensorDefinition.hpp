#pragma once
#include <Arduino.h>
#include <dht11.h>
#include <DHT.h>
#include <Sensor.h>
#define DHT11_DEFAULT_PIN D5
class TempSensor : public Sensor
{

private:
    DHT *dht;
    float readTemperature();
    float readHumidity();

public:
    TempSensor(int);
    String read();
};
