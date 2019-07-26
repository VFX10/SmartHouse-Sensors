#include <Arduino.h>

class InlineContact
{

private:
    uint8_t *pin = new uint8_t(D8);
public:
    InlineContact(uint8_t *&);
    uint8_t getState(){
        return digitalRead(*(this->pin));
    }
};
InlineContact::InlineContact(uint8_t *&pin){
    this->pin = pin;
    pinMode(*(this->pin), INPUT);
}