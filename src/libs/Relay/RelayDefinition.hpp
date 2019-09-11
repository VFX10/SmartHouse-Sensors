#pragma once

#include <Arduino.h>
#include <Ticker.h>
#define RELAY_DEFAULT_PIN D8
#define RELAY_BUTTON_DEFAULT_PIN D5

class Relay
{
private:

    uint8_t pin;
    Ticker swTicker;

    uint8_t stateLED = HIGH;
    uint8_t stateButton;
    uint8_t previous = LOW;
    long time = 0;
    long debounce = 200;

public:
    Relay(uint8_t);
    void changeState(uint8_t);
    uint8_t read();
};