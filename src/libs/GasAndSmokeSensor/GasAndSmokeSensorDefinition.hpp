#pragma once

#include <TroykaMQ.h>

#define MQ2_DEFAULT_PIN A0
#define MQ2_SPEAKER_DEFAULT_PIN D6

class GasAndSmokeSensor
{

private:
    uint8_t speakerPin = MQ2_SPEAKER_DEFAULT_PIN;
    MQ2 *mq2 = new MQ2(MQ2_DEFAULT_PIN);
    void calibrateSensor();

public:
    GasAndSmokeSensor();
    GasAndSmokeSensor(uint8_t, uint8_t);
    String read();
};