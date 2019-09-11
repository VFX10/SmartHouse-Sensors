#pragma once
#include <FS.h>
#include <Arduino.h>
#include <libs/HardwareButtons/HardwareButtonsDefinition.hpp>

HardwareButtons::HardwareButtons()
{
    //pinMode(RESET_BUTTON_DEFAULT_PIN, INPUT_PULLUP);
    //pinMode(REBOOT_BUTTON_DEFAULT_PIN, INPUT);
    myBtn->begin();

    rebootTicker.attach(0.1, [&]() {
        myBtn->read();
        if (myBtn->wasReleased()){
            {
            Serial.println("Sensor will reboot now.");
            ESP.restart();
        }
        }
        else if (myBtn->pressedFor(5000))
        {
            digitalWrite(STATUS_LED_DEFAULT_PIN, LOW);
            //while(!digitalRead(RESET_BUTTON_DEFAULT_PIN));
            Serial.println("Sensor will reset now.");
            // SPIFFS.remove("/config.json");
            this->mng.resetSettings();
            delay(5000);
            digitalWrite(STATUS_LED_DEFAULT_PIN, HIGH);
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
