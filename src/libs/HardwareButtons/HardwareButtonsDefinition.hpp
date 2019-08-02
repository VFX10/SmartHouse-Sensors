#include <Arduino.h>
#include <WiFiManager.h>
#include <Ticker.h>
#pragma once

#define REBOOT_BUTTON_DEFAULT_PIN D1
#define RESET_BUTTON_DEFAULT_PIN D2
#define STATUS_LED_DEFAULT_PIN D3
class HardwareButtons
{
private:
    WiFiManager mng;
    Ticker resetTicker;
    Ticker rebootTicker;

public:
    HardwareButtons();
    void instantReset();
};