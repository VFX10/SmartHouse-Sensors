#pragma once

#include <Arduino.h>
#define RELAY_DEFAULT_PIN D8

class Relay
{
private:
    uint8_t pin;

public:
    Relay(uint8_t);
    void changeState(uint8_t);
    uint8_t read();
};