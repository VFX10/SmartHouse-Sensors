#pragma once

#include <Arduino.h>
#include <libs/PowerConsumptiom/PowerConssumptionDefinitions.h>
#include <math.h>
#include <ArduinoJson.h>

PowerConsumptionSensor::PowerConsumptionSensor(int scl = SCL_DEFAULT, int sda = SDL_DEFAULT)
{
    TwoWire *i2c = new TwoWire();
    i2c->begin(sda, scl);
    ina219.begin();
    // To use a slightly lower 32V, 1A range (higher precision on amps):
    // ina219.setCalibration_32V_1A();
    // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
    // ina219.setCalibration_16V_400mA();
    ina219.setCalibration_32V_2A();
}

DynamicJsonDocument PowerConsumptionSensor::read()
{
    DynamicJsonDocument json(1024);

    // int shuntvoltage = ina219.getShuntVoltage_mV();
    // int busvoltage = ina219.getBusVoltage_V();
    float power = ina219.getPower_mW() / 1000.0f;
    // json["busvoltage"] = ina219.getBusVoltage_V();
    // json["shuntvoltage"] = shuntvoltage;
    // json["loadvoltage"] = busvoltage + (shuntvoltage / 1000);
    // json["current"] = ina219.getCurrent_mA();
    json["power"] = roundf(power * 100) / 100 ;
    // serializeJsonPretty(json, Serial);
    return json;
}