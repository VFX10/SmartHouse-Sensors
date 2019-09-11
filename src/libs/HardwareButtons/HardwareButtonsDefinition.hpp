
#pragma once
#include <Arduino.h>
#include <WiFiManager.h>
#include <Ticker.h>
#include <libs/Relay/Relay.h>
#include <JC_Button.h>

#define REBOOT_BUTTON_DEFAULT_PIN D1
#define RESET_BUTTON_DEFAULT_PIN D2

#define STATUS_LED_DEFAULT_PIN D3
class HardwareButtons
{
private:
    WiFiManager mng;
    Relay relay;
    Ticker resetTicker;
    Ticker rebootTicker;
    Ticker swTicker;

    long buttonTimer = 0;
    long longPressTime = 250;

    boolean buttonActive = false;
    boolean longPressActive = false;
        Button *myBtn = new Button(REBOOT_BUTTON_DEFAULT_PIN);
        enum states_t {ONOFF, TO_BLINK, BLINK, TO_ONOFF};

public:
    HardwareButtons();
    void instantReset();
};