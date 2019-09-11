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
    pinMode(RELAY_BUTTON_DEFAULT_PIN, INPUT_PULLUP);
    this->swTicker.attach(0.1, [&]() {
        stateButton = digitalRead(RELAY_BUTTON_DEFAULT_PIN);

        if (stateButton == HIGH && previous == LOW && millis() - time > 200)
        {
            if (!this->read() == HIGH)
            {
                //stateLED = LOW;
                digitalWrite(this->pin, LOW);
            }
            else
            {
                digitalWrite(this->pin, HIGH);
            }
            time = millis();

        }
        previous = stateButton;
        //digitalWrite(LED, stateLED);
    });
}
void Relay::changeState(uint8_t power)
{
    digitalWrite(this->pin, power);
}
uint8_t Relay::read()
{
    return !digitalRead(this->pin);
}
