#pragma once
#include <Arduino.h>

#define INLINE_CONTACT_DEFAULT_PIN A0
class InlineContact
{

private:
    uint8_t pin;

public:
    InlineContact();
    InlineContact(uint8_t);
    String read();
};
