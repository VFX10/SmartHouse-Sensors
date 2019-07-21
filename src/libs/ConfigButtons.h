#include <Arduino.h>
#include <WiFiManager.h>
#include <libs/WifiConfigurations.h>

class ConfigButtons
{
    Ticker resetTicker;
    Ticker rebootTicker;

public:
    ConfigButtons(uint8_t, uint8_t);
    void instantReset()
    {
        pairingModeTicker.detach();
        digitalWrite(D2, LOW);
        Serial.println("Sensor will reset now.");
        wifiManager.resetSettings();
        delay(5000);
        digitalWrite(D2, HIGH);
    }
};
ConfigButtons::ConfigButtons(uint8_t resetSwitchPin, uint8_t rebootSwitchPin)
{
    Serial.println("Buttons const");
    pinMode(resetSwitchPin, INPUT_PULLUP);
    pinMode(rebootSwitchPin, INPUT_PULLUP);
    resetTicker.attach(1, []() {
        static int count;
        count = !digitalRead(D3) ? count + 1 : 0;
        if (count == 10)
        {
            pairingModeTicker.detach();
            digitalWrite(D2, LOW);
            Serial.println("Sensor will reset now.");
            wifiManager.resetSettings();
            delay(5000);
            digitalWrite(D2, HIGH);

            //delay(5000);
            // ESP.reset();
        }
    });
    rebootTicker.attach(0.1, []() {
        if (!digitalRead(D1))
        {
            Serial.println("Sensor will reboot now.");
            ESP.restart();
        }
    });
    Serial.println("Sensor initialized");
}
