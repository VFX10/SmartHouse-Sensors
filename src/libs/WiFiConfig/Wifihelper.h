#pragma once
#include <libs/WiFiConfig/WiFiHelperDefinition.hpp>

WiFiHelper::WiFiHelper(Sensor *relay = nullptr)
{
    this->relay = relay;
    webServer = new AsyncWebServer(80);
    WiFi.mode(WIFI_STA);
}
DynamicJsonDocument handleTest(AsyncWebServerRequest *request, uint8_t *datas)
{
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, (char *)datas);
    return doc;
}
void WiFiHelper::end()
{
    WiFi.stopSmartConfig();
}
void WiFiHelper::begin()
{
    pinMode(STATUS_LED_DEFAULT_PIN, OUTPUT);
    this->pairingModeTicker.attach(0.6, []() {
        digitalWrite(STATUS_LED_DEFAULT_PIN, !digitalRead(STATUS_LED_DEFAULT_PIN));
    });
    bool configDone = false;
    if (config->hasWiFiCredentials())
    {

        Serial.println("Sensor allready configured");
        DynamicJsonDocument doc = config->getWifiCredentials();

        String ssid = doc["ssid"], password = doc["password"];
        WiFi.begin(ssid.c_str(), password.c_str());
        configDone = true;
    }
    else
    {
        pairingModeTicker.attach(0.2, []() {
            digitalWrite(STATUS_LED_DEFAULT_PIN, !digitalRead(STATUS_LED_DEFAULT_PIN));
        });
        WiFi.beginSmartConfig();
    }

    while (WiFi.status() != WL_CONNECTED && !WiFi.smartConfigDone())
    {
        delay(500);
        Serial.print(".");
    }

    apIP = new IPAddress(WiFi.localIP());
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    dnsServer.start(DNS_PORT, "*", *apIP);
    webServer->begin();
    webServer->on("/api/getConfig", HTTP_GET, [&](AsyncWebServerRequest *request) {
        if (sensorType == SENSOR_UNDEFINED)
        {
            request->send(200, "application/json", "{\"error\": \"Sensor undefined\"}");
        }
        else
        {
            DynamicJsonDocument json(1024);
            auto error = deserializeJson(json, config->readConfig());
            if (!error)
            {
                DynamicJsonDocument payloadJson(1024);
                payloadJson["sensorName"] = json["sensorName"];
                payloadJson["freqMinutes"] = json["freqMinutes"];
                payloadJson["sensorType"] = json["sensorType"];
                payloadJson["server"] = json["server"];
                payloadJson["port"] = json["port"];
                payloadJson["macAddress"] = WiFi.macAddress();
                String data;
                serializeJson(payloadJson, data);
                request->send(200, "application/json", data);
            }
            else
            {
                request->send(200, "application/json", "{\"error\": \"Error getting device config\"}");
            }
        }
    });

    webServer->on("/api/events", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [&](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
            auto jsonData = handleTest(request, data);
            DynamicJsonDocument jsonResponse(1024);
        String event;
        Serial.println("Received event");

        serializeJsonPretty(jsonData, Serial);
        auto ticker = new Ticker();
        if (jsonData["event"] == "reboot")
        {

            ticker->once(5, [&]() {
                Serial.println("I'm rebooting.");
                ESP.restart();
            });

            request->send(200, "application/json", "{\"response\": \"Sensor will reboot in 5 seconds\"}");
        }
        else if (jsonData["event"] == "reset")
        {
            ticker->once(5, [&]() {
                Serial.println("I'm reseting.");
                hardwareButtons.instantReset();
            });
            request->send(200, "application/json", "{\"response\": \"Sensor will reset in 5 seconds\"}");
        }
        else if (jsonData["event"] == "on")
        {
            if (relay == nullptr)
            {
                request->send(200, "application/json", "{\"response\": \"Event not permitted\"}");
            }
            else
            {
                relay->changeState(HIGH);
                DynamicJsonDocument json(1024);
                json["macAddress"] = WiFi.macAddress();
                json["state"] = 1;
                String data;
                serializeJson(json, data);
                request->send(200, "application/json", data);
            }
        }
        else if (jsonData["event"] == "off")
        {
            if (relay == nullptr)
            {
                request->send(200, "application/json", "{\"response\": \"Event not permitted\"}");
            }
            else
            {
                relay->changeState(LOW);
                DynamicJsonDocument json(1024);
                json["macAddress"] = WiFi.macAddress();
                json["state"] = 0;
                String data;
                serializeJson(json, data);
                request->send(200, "application/json", data);
            }
        }
        request->send(500, "application/json", "{\"error\": \"Wrong command\"}"); });
    webServer->on("/api/config", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [&](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
            auto jsonData = handleTest(request, data);
            DynamicJsonDocument jsonResponse(1024);
            String response;
            int statusCode = 200;
            if (!jsonData.isNull())
            {
                DynamicJsonDocument currentConfig(1024);
                DynamicJsonDocument json(1024);
                deserializeJson(currentConfig, config->readConfig());

                json["sensorName"] = currentConfig["sensorName"];
                json["freqMinutes"] = currentConfig["freqMinutes"];
                if (jsonData.containsKey("sensorName"))
                {
                    json["sensorName"] = jsonData["sensorName"];
                }
                if (jsonData.containsKey("freqMinutes"))
                {
                    json["freqMinutes"] = jsonData["freqMinutes"];
                }
                json["ssid"] = jsonData["ssid"];
                json["password"] = jsonData["password"];
                json["server"] = jsonData["server"];
                json["port"] = jsonData["port"];

                jsonResponse["macAddress"] = WiFi.macAddress();
                jsonResponse["message"] = "Sensor will reboot in 5 secconds!";
                serializeJson(jsonResponse, response);
                configDone = config->writeConfig(json);
            }
            else
            {
                response = "{\"error\": \"invalid configuration\"}";
                statusCode = 500;
                Serial.println("failed to load json config");
                configDone = false;
            }
            auto b = new Ticker();
            b->once(5, [&]() {
                Serial.println("I'm rebooting.");
                ESP.restart();
            });
            request->send(statusCode, "application/json", response); });

    webServer->onNotFound([&](AsyncWebServerRequest *request) {
        Serial.println("am intrat in 404");
        request->send(404, "application/json", "{\"error\": \"page not found\"}");
    });

    this->webServerConfigTicker.attach(0.1, [&]() {
        dnsServer.processNextRequest();
    });
    if (!configDone)
        Serial.println("Waiting for configuration file");
    while (!configDone)
    {
        Serial.print(".");
        delay(500);
    }
    this->pairingModeTicker.detach();
    digitalWrite(D3, HIGH);
}