#include <Arduino.h>
#include <ArduinoJson.h>
#include <libs/GasAndSmokeSensor/GasAndSmokeSensorDefinition.hpp>

#ifndef GAS_SENSOR_H
#define GAS_SENSOR_H

GasAndSmokeSensor::GasAndSmokeSensor()
{
    pinMode(MQ2_SPEAKER_DEFAULT_PIN, OUTPUT);
    this->mq2 = new MQ2(MQ2_DEFAULT_PIN);
    this->calibrateSensor();
}

GasAndSmokeSensor::GasAndSmokeSensor(uint8_t sensorPin, uint8_t speakerPin)
{
    this->speakerPin = speakerPin;
    pinMode(this->speakerPin, OUTPUT);
    this->mq2 = new MQ2(sensorPin);
    this->calibrateSensor();
}

void GasAndSmokeSensor::calibrateSensor()
{
    this->mq2->calibrate();
    Serial.print("Ro = ");
    Serial.println(this->mq2->getRo());
}

String GasAndSmokeSensor::read()
{

    long methane = this->mq2->readMethane(), smoke = this->mq2->readSmoke();
    Serial.println(methane);
    Serial.println(smoke);
    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();
    json["methane"] = methane;
    json["smoke"] = smoke;

    String jsonString;
    json.printTo(jsonString);
    json.prettyPrintTo(Serial);

    if (methane >= 10 || smoke >= 10)
        tone(this->speakerPin, 2000, 1000);
    else
    {
        noTone(this->speakerPin);
    }

    return jsonString;
}
#endif