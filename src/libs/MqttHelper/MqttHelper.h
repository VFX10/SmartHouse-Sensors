#pragma once
#include <libs/MqttHelper/MqttHelperDefinition.hpp>
#include <ArduinoJson.h>

MqttHelper::MqttHelper() {}
MqttHelper::MqttHelper(std::string *&server, int *&port)
{
    // client = PubSubClient(espClient);
    this->domain = server;
    this->port = port;
}
MqttHelper::MqttHelper(IPAddress *&srv, int *&port)
{
    // client = PubSubClient(espClient);
    this->server = srv;
    this->port = port;
}
MqttHelper::~MqttHelper()
{
    Serial.println("destructor hit");
    client.disconnect();
}

void MqttHelper::initMqtt()
{
    //client = PubSubClient(espClient);

    Serial.print(*(this->server));
    Serial.println(*(this->port));
    if ((*(this->domain)).compare("") == 0)
    {
        Serial.println("am primit ip");
        this->client.setServer(*(this->server), *(this->port));
    }
    else
    {
        Serial.println("am primit domeniu");
        this->client.setServer(*(this->domain)->c_str(), *(this->port));
    }

    client.setCallback([&](char *topic, byte *payload, unsigned int length) {
        Serial.println("callback");
        char *msg = (char *)payload;
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject &obj = jsonBuffer.parseObject(msg);
        obj.prettyPrintTo(Serial);
        String deviceMacAddress = WiFi.macAddress();
        String receivedMacAddress = obj.get<String>("macAddress");
        deviceMacAddress.toUpperCase();
        receivedMacAddress.toUpperCase();
        if (receivedMacAddress.compareTo(deviceMacAddress) == 0)
        {
            Serial.println("Eu is");
            if (obj["event"] == "reboot")
            {
                ESP.restart();
            }
            else if (obj["event"] == "reset")
            {
                hardwareButtons.instantReset();
            }
            else if (obj["event"] == "on")
            {
                // digitalWrite(D8, LOW);
                relay.changeState(LOW);
                DynamicJsonBuffer jsonBuffer;
                JsonObject &json = jsonBuffer.createObject();
                json["macAddress"] = WiFi.macAddress();
                json["sensorName"] = wifi.sensorName;
                json["state"] = 1;
                String data;
                json.prettyPrintTo(data);
                publish("response", data.c_str());
            }
            else if (obj["event"] == "off")
            {
                // digitalWrite(D8, HIGH);
                relay.changeState(HIGH);
                DynamicJsonBuffer jsonBuffer;
                JsonObject &json = jsonBuffer.createObject();
                json["macAddress"] = WiFi.macAddress();
                json["sensorName"] = wifi.sensorName;
                json["state"] = 0;
                String data;
                json.prettyPrintTo(data);
                publish("response", data.c_str());
            }
            else if (obj["event"] == "config")
            {
                Serial.println("Saving configuration file");
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
                    JsonObject &currentSettingsJson = jsonBuffer.parseObject(buf.get());
                    JsonObject &json = jsonBuffer.createObject();
                    json["server"] = currentSettingsJson.get<String>("server");
                    json["port"] = currentSettingsJson.get<String>("port");
                    json["freqMinutes"] = obj["config"]["freq"];
                    json["sensorName"] = obj["config"]["name"];
                    json["sensorType"] = obj["config"]["sensorType"];

                    File configFile = SPIFFS.open("/config.json", "w");

                    json.printTo(Serial);
                    json.printTo(configFile);
                    configFile.close();
                    ESP.restart();
                    //end save
                }
            }
        }
        else
        {
            Serial.println("Nu sunt eu");
        }
    });
}
void MqttHelper::connect(String name)
{

    this->mqttTicker.detach();
    this->client.connect(name.c_str());
    if (this->client.connected())
    {
        if (this->client.subscribe("SensorsSetingsChannel"))
        {
            Serial.println("Succesfully subscribed to SensorsSetingsChannel");
        }
        this->mqttTicker.attach(1, [&]() {Serial.print("."); this->client.loop(); });
    }
    if (!this->registerModule())
    {
        delay(5000);
        this->hardwareButtons.instantReset();
        return;
    }
}
bool MqttHelper::isConnected()
{
    return this->client.connected();
}
void MqttHelper::publish(String topic, String message)
{
    this->client.publish(topic.c_str(), message.c_str());
}
bool MqttHelper::registerModule()
{
    if (this->client.connected())
    {
        DynamicJsonBuffer jsonBuffer;
        JsonObject &json = jsonBuffer.createObject();
        json["macAddress"] = WiFi.macAddress();
        json["sensorName"] = wifi.sensorName;
        json["sensorType"] = wifi.sensorType;
        json["readingFrequency"] = wifi.freqMinutes;
        String data;
        json.prettyPrintTo(data);

        this->publish("SensorsConfigChannel", data.c_str());
        return true;
    }
    else
    {
        Serial.println("Module can't be registered, please reset it. Abort All!");
        return false;
    }
}