#include <libs/InlineContact/InlineContactDefinition.hpp>
#include <ArduinoJson.h>
InlineContact::InlineContact()
{
    this->pin = INLINE_CONTACT_DEFAULT_PIN;
}
InlineContact::InlineContact(uint8_t pin)
{
    this->pin = pin;
}
String InlineContact::read()
{
    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();
    json["state"] = analogRead(this->pin) == 1024;
    String data;
    json.printTo(data);
    json.prettyPrintTo(Serial);
    return data;
}