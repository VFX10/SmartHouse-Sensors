#pragma once

class Sensor
{
public:
    virtual String read();
    virtual void changeState(int) {}
};
