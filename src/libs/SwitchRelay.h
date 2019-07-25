#include <Arduino.h>


#ifndef SwitchRelay_H
#define SwitchRelay_H
uint8_t *pin = new uint8_t(D8);
class SwitchRelay
{


public:
    SwitchRelay();
    SwitchRelay(uint8_t *&);
    int* changeState(uint8_t power)
    {
        Serial.println(*(pin));
        Serial.println(power);

        digitalWrite(*(pin), power);
        return new int(1);
    }
};
SwitchRelay::SwitchRelay()
{
}

SwitchRelay::SwitchRelay(uint8_t *&pn)
{
    pin = pn;
    pinMode(*(pin), OUTPUT);
    digitalWrite(*(pin), HIGH);
}
#endif