#pragma once
#include <FS.h>
#include <Arduino.h>
#include <libs/HardwareButtons/HardwareButtonsDefinition.hpp>

HardwareButtons::HardwareButtons()
{
    pinMode(RESET_BUTTON_DEFAULT_PIN, INPUT_PULLUP);
    pinMode(REBOOT_BUTTON_DEFAULT_PIN, INPUT_PULLUP);
    resetTicker.attach(1, [&]() {
        static int count;
        count = !digitalRead(RESET_BUTTON_DEFAULT_PIN) ? count + 1 : 0;
        if (count == 10)
        {
            digitalWrite(STATUS_LED_DEFAULT_PIN, LOW);
            Serial.println("Sensor will reset now.");
            SPIFFS.remove("/config.json");
            this->mng.resetSettings();
            delay(5000);
            digitalWrite(STATUS_LED_DEFAULT_PIN, HIGH);
        }
    });
    rebootTicker.attach(0.1, []() {
        if (!digitalRead(REBOOT_BUTTON_DEFAULT_PIN))
        {
            Serial.println("Sensor will reboot now.");
            ESP.restart();
        }
    });
    Serial.println("Sensor initialized");
}
void HardwareButtons::instantReset()
{
    // pairingModeTicker.detach();
    digitalWrite(STATUS_LED_DEFAULT_PIN, LOW);
    Serial.println("Sensor will reset now.");
    this->mng.resetSettings();
    delay(5000);
    digitalWrite(STATUS_LED_DEFAULT_PIN, HIGH);
}
