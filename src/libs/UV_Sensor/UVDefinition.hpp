#include <Arduino.h>
#pragma once

#define UV_SENSOR_DEFAULT_PIN A0

class UVSensor
{
private:
    uint8_t pin;
    int getUVIndex();

public:
    UVSensor();
    UVSensor(uint8_t);
    String read();
};