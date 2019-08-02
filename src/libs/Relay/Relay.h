#pragma once

#include <Arduino.h>
#include <libs/Relay/RelayDefinition.hpp>

Relay::Relay(uint8_t pin = 0)
{
    Serial.println(pin);
    Serial.println(this->pin);
    if (pin == 0)
    {
        this->pin = RELAY_DEFAULT_PIN;
        pinMode(this->pin, OUTPUT);
        digitalWrite(this->pin, HIGH);
    }
    else
    {
        this->pin = pin;
        pinMode(this->pin, OUTPUT);
        digitalWrite(this->pin, HIGH);
    }
}
void Relay::changeState(uint8_t power)
{
    Serial.println(this->pin);
    Serial.println(power);
    digitalWrite(this->pin, power);
}
uint8_t Relay::read()
{
    return digitalRead(this->pin);
}
