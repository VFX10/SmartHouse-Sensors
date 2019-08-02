#pragma once
#include <FS.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

#include <libs/WiFiConfig/WiFiHelperDefinition.hpp>
#include <libs/HardwareButtons/HardwareButtons.h>

Ticker WiFiHelper::pairingModeTicker;
bool WiFiHelper::shouldSaveConfig = false;

WiFiHelper::WiFiHelper()
{
    this->getSavedConfig();
}
WiFiHelper::WiFiHelper(String ssidName, String password = "")
{
    Serial.println("SSid si Pas Salvat");
    this->ssidName = ssidName;
    this->password = password;
}
void WiFiHelper::begin()
{
    Serial.println("Am ajuns aici. Doamne ajuta");
    pinMode(STATUS_LED_DEFAULT_PIN, OUTPUT);
    SPIFFS.begin();
    this->pairingModeTicker.attach(0.6, []() {
        digitalWrite(STATUS_LED_DEFAULT_PIN, !digitalRead(STATUS_LED_DEFAULT_PIN));
    });
    WiFiManagerParameter customIp("ip", "ip Hub", server.c_str(), 40);
    WiFiManagerParameter customPort("port", "port", port.c_str(), 6);
    WiFiManagerParameter customFreq("Freq", "Frecventa citire", freqMinutes.c_str(), 2);
    WiFiManagerParameter customSensorName("sensorName", "Nume senzor", sensorName.c_str(), 50);
    WiFiManagerParameter customSensorType("sensortype", "Tip senzor", sensorType.c_str(), 50);
    wifiManager.setSaveConfigCallback([]() {
        Serial.println("Should save config");
        shouldSaveConfig = true;
    });
    wifiManager.addParameter(&customIp);
    wifiManager.addParameter(&customPort);
    wifiManager.addParameter(&customFreq);
    wifiManager.addParameter(&customSensorName);
    wifiManager.addParameter(&customSensorType);
    wifiManager.setAPCallback([](WiFiManager *) {
        Serial.println("Entered config mode");
        pairingModeTicker.attach(0.2, []() {
            digitalWrite(STATUS_LED_DEFAULT_PIN, !digitalRead(STATUS_LED_DEFAULT_PIN));
        });
    });
    bool status = false;
    if (password == "")
    {
        // config without password
        status = wifiManager.autoConnect(ssidName.c_str());
    }
    else
    {
        // config with password
        status = wifiManager.autoConnect(ssidName.c_str(), password.c_str());
    }
    if (!status)
    {
        Serial.println("Failed to connect and hit timeout. Sensor will reset in 3 seconds!");
        delay(3000);
        //reset and try again, or maybe put it to deep sleep
        wifiManager.resetSettings();
        Serial.println("Sensor succesfuly reset. Module will reboot in 5 seconds.");
        delay(5000);
        ESP.reset();
    }
    server = customIp.getValue();
    port = customPort.getValue();
    freqMinutes = customFreq.getValue();
    sensorName = customSensorName.getValue();
    sensorType = customSensorType.getValue();

    Serial.println("Dupa Configurare " + sensorName);

    if (shouldSaveConfig)
    {
        Serial.println("Saving configuration file");
        DynamicJsonBuffer jsonBuffer;
        JsonObject &json = jsonBuffer.createObject();
        json["server"] = server;
        json["port"] = port;
        json["freqMinutes"] = freqMinutes;
        json["sensorName"] = sensorName;
        json["sensorType"] = sensorType;

        File configFile = SPIFFS.open("/config.json", "w");

        json.printTo(Serial);
        json.printTo(configFile);
        configFile.close();
        //end save
    }
    this->pairingModeTicker.detach();
    digitalWrite(D3, HIGH);
    getSavedConfig();
    Serial.println("I have connection to wifi.");
}
void WiFiHelper::getSavedConfig()
{
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json"))
    {
        //file exists, reading and loading
        Serial.println("reading config file");
        File configFile = SPIFFS.open("/config.json", "r");
        if (configFile)
        {
            Serial.println("opened config file");
            size_t size = configFile.size();
            // Allocate a buffer to store contents of the file.
            std::unique_ptr<char[]> buf(new char[size]);

            configFile.readBytes(buf.get(), size);
            DynamicJsonBuffer jsonBuffer;
            JsonObject &json = jsonBuffer.parseObject(buf.get());
            json.prettyPrintTo(Serial);
            if (json.success())
            {
                Serial.println("\nparsed json");
                server = json.get<String>("server");
                port = json.get<String>("port");
                freqMinutes = json.get<String>("freqMinutes");
                sensorName = json.get<String>("sensorName");
                sensorType = json.get<String>("sensorType");
            }
            else
            {
                Serial.println("failed to load json config");
            }
            configFile.close();
        }
    }
}