#pragma once
#include <libs/GasAndSmokeSensor/GasAndSmokeSensorDefinition.hpp>

GasAndSmokeSensor::GasAndSmokeSensor(int sensorPin = MQ2_DEFAULT_PIN, int speakerPin = MQ2_SPEAKER_DEFAULT_PIN)
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

DynamicJsonDocument GasAndSmokeSensor::read()
{

    long methane = this->mq2->readMethane(), smoke = this->mq2->readSmoke();
    // Serial.println(methane);
    // Serial.println(smoke);
    DynamicJsonDocument json(1024);
    json["methane"] = methane;
    json["smoke"] = smoke;

    // String jsonString;
    // serializeJson(json, jsonString);
    // json.prettyPrintTo(Serial);

    if (methane >= 20 || smoke >= 40)
    {
        tone(this->speakerPin, 2000, 1000);
        json["warning"] = true;
    }
    else
    {
        noTone(this->speakerPin);
        json["warning"] = false;
    }
    // serializeJson(json, jsonString);
    return json;
}