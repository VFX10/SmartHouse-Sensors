#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Ticker.h>
#include <WiFiManager.h>

class WiFiHelper
{
private:
    static Ticker pairingModeTicker;
    int httpCode = 0;

    static bool shouldSaveConfig;
    WiFiManager wifiManager;
    String ssidName, password;
    void tick();
    void getSavedConfig();

public:
    WiFiHelper();
    WiFiHelper(String, String);
    void begin();
    String server, port, freqMinutes, sensorName = "", sensorType;
};