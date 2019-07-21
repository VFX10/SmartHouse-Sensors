#include <Arduino.h>
#include <WiFiManager.h>
#include <libs/WifiConfigurations.h>

Ticker resetTicker;
Ticker rebootTicker;

void btnResetTicker();
void btnRebootTicker();
void ConfigButtons(const uint8_t rebootPin, const uint8_t resetPin)
{
    Serial.println("Buttons const");
    pinMode(resetPin, INPUT_PULLUP);
    pinMode(rebootPin, INPUT_PULLUP);
    resetTicker.attach(1, btnResetTicker);
    rebootTicker.attach(0.1, btnRebootTicker);
    Serial.println("Sensor initialized");
}


void btnResetTicker()
{
    static int count;
    count = !digitalRead(D3) ? count + 1 : 0;
    if (count == 3)
    {
        pairingModeTicker.detach();
        digitalWrite(D2, LOW);
        Serial.println("Sensor will reset now.");
        wifiManager.resetSettings();
        delay(5000);
        digitalWrite(D2, HIGH);
        //delay(5000);
        //ESP.reset();
    }
}
void btnRebootTicker()
{
    if (!digitalRead(D1))
    {
        Serial.println("Sensor will reboot now.");
        ESP.restart();
    }
}


