#pragma once

#include <Arduino.h>
#include <Ticker.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <libs/WiFiConfig/Wifihelper.h>
#include <libs/Relay/Relay.h>
#include <libs/HardwareButtons/HardwareButtons.h>

class MqttHelper
{
private:
    Relay relay;
    HardwareButtons hardwareButtons;
    WiFiClient espClient;
    WiFiHelper wifi;
    Ticker mqttTicker;
    std::string *domain = new std::string;
    IPAddress *server = new IPAddress;
    int *port;
    PubSubClient client = PubSubClient(espClient);

public:
    MqttHelper();
    MqttHelper(std::string *&, int *&);
    MqttHelper(IPAddress *&, int *&);

    ~MqttHelper();

    void initMqtt();
    void connect(String);
    bool isConnected();
    bool registerModule();
    void publish(String, String);
};