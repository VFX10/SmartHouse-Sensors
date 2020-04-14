#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Sensor.h>
#define SCL_DEFAULT D6
#define SDL_DEFAULT D5
class PowerConsumptionSensor : public Sensor
{

private:
Adafruit_INA219 ina219;
public:
    PowerConsumptionSensor(int, int);
    DynamicJsonDocument read();
};
