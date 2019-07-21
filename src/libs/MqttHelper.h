#include <Ticker.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <libs\ConfigButtons.h>

ConfigButtons hardwareButtons(D1, D3);

class MqttHelper
{
    WiFiClient espClient;
    
    Ticker mqttTicker;
    String server;
    int port;

public:
    MqttHelper();
    MqttHelper(String , int);
    MqttHelper(IPAddress, int);
    PubSubClient client;
    void connect(String name)
    {

            //mqttTicker.detach();
            client.connect(name.c_str());
            //client.subscribe("SensorsSetingsChannel");
            //mqttTicker.attach(1, [&]() { client.loop(); });
    }
    boolean isConnected()
    {
        return client.connected();
    }
    void publish(String topic, String message)
    {
        client.publish(topic.c_str(), message.c_str());
    }
};

MqttHelper::MqttHelper() {}
MqttHelper::MqttHelper(String server, int port)
{
    client = PubSubClient(espClient);
    client.setServer(server.c_str(), port);
    client.setCallback([&](char *topic, byte *payload, unsigned int length) {
        char *msg = (char *)payload;
        Serial.println(msg);
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject &obj = jsonBuffer.parseObject(msg);
        if (obj["sensorName"] == sensorName)
        {
            if (obj["event"] == "reboot")
            {
                ESP.restart();
            }
            else if (obj["event"] == "reset")
            {
                hardwareButtons.instantReset();
            }
            // else
            // {
            //     Serial.println("Saving configuration file");
            //     Serial.println("reading config file");
            //     File configFile = SPIFFS.open("/config.json", "r");
            //     if (configFile)
            //     {
            //         Serial.println("opened config file");
            //         size_t size = configFile.size();
            //         // Allocate a buffer to store contents of the file.
            //         std::unique_ptr<char[]> buf(new char[size]);

            //         configFile.readBytes(buf.get(), size);
            //         DynamicJsonBuffer jsonBuffer;
            //         JsonObject &currentSettingsJson = jsonBuffer.parseObject(buf.get());
            //         JsonObject &json = jsonBuffer.createObject();
            //         json["server"] = currentSettingsJson.get<String>("server");
            //         json["port"] = currentSettingsJson.get<String>("port");
            //         json["freqMinutes"] = obj["event"]["freq"];
            //         json["sensorName"] = obj["event"]["name"];

            //         File configFile = SPIFFS.open("/config.json", "w");

            //         json.printTo(Serial);
            //         json.printTo(configFile);
            //         configFile.close();
            //         //end save
            //     }
            // }
        }
    });
    //client.connect(sensorName.c_str());
}
MqttHelper::MqttHelper(IPAddress server, int port)
{
    client = PubSubClient(espClient);
    client.setServer(server, port);
    client.setCallback([&](char *topic, byte *payload, unsigned int length) {
        char *msg = (char *)payload;
        Serial.println(msg);
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject &obj = jsonBuffer.parseObject(msg);
        if (obj["sensorName"] == sensorName)
        {
            if (obj["event"] == "reboot")
            {
                ESP.restart();
            }
            else if (obj["event"] == "reset")
            {
                hardwareButtons.instantReset();
            }
            // else
            // {
            //     Serial.println("Saving configuration file");
            //     Serial.println("reading config file");
            //     File configFile = SPIFFS.open("/config.json", "r");
            //     if (configFile)
            //     {
            //         Serial.println("opened config file");
            //         size_t size = configFile.size();
            //         // Allocate a buffer to store contents of the file.
            //         std::unique_ptr<char[]> buf(new char[size]);

            //         configFile.readBytes(buf.get(), size);
            //         DynamicJsonBuffer jsonBuffer;
            //         JsonObject &currentSettingsJson = jsonBuffer.parseObject(buf.get());
            //         JsonObject &json = jsonBuffer.createObject();
            //         json["server"] = currentSettingsJson.get<String>("server");
            //         json["port"] = currentSettingsJson.get<String>("port");
            //         json["freqMinutes"] = obj["event"]["freq"];
            //         json["sensorName"] = obj["event"]["name"];

            //         File configFile = SPIFFS.open("/config.json", "w");

            //         json.printTo(Serial);
            //         json.printTo(configFile);
            //         configFile.close();
            //         //end save
            //     }
            // }
        }
    });
    //client.connect(sensorName.c_str());
}