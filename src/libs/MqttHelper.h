#include <Ticker.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <libs\ConfigButtons.h>

ConfigButtons hardwareButtons(D1, D3);

class MqttHelper
{
    WiFiClient espClient;
    PubSubClient client;
    Ticker mqttTicker;
    String server;
    int port;

public:
    MqttHelper();
    MqttHelper(String, int);
    void connect()
    {
        mqttTicker.detach();
        client.connect(sensorName.c_str());
        client.subscribe("SensorsSetingsChannel");
        mqttTicker.attach(1, [&]() { client.loop(); });
    }
    bool isConnected()
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
    client.setServer(server.c_str(), 1883);
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
        }
    });
}