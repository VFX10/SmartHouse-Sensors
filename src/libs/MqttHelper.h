#include <Ticker.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <libs\ConfigButtons.h>

ConfigButtons hardwareButtons(D1, D3);

void callback(char *topic, byte *payload, unsigned int length)
{
}

class MqttHelper
{
private:
    WiFiClient espClient;

    Ticker mqttTicker;
    std::string *domain = new std::string;
    IPAddress *server = new IPAddress;
    int *port;

public:
    MqttHelper();
    MqttHelper(std::string *&, int *&);
    MqttHelper(IPAddress *&, int *&);
    PubSubClient client = PubSubClient(espClient);
    ~MqttHelper()
    {
        Serial.println("destructor hit");
        client.disconnect();
    }

    void initMqtt()
    {
        //client = PubSubClient(espClient);

        Serial.print(*(this->server));
        Serial.println(*(this->port));
        client = PubSubClient(*(this->server), *(this->port), callback, espClient);
        if ((*(this->domain)).compare("") == 0)
        {
            Serial.println("am primit ip");
            client.setServer(*(this->server), *(this->port));
            //client = PubSubClient(*(this->server), *(this->port), callback, espClient);
        }
        else
        {
            Serial.println("am primit domeniu");
            client.setServer(*(this->domain)->c_str(), *(this->port));
            //client = PubSubClient(*(this->domain)->c_str(), *(this->port), callback, espClient);
        }

        client.setCallback([](char *topic, byte *payload, unsigned int length) {
            Serial.println("callback");
            char *msg = (char *)payload;
            StaticJsonBuffer<200> jsonBuffer;
            JsonObject &obj = jsonBuffer.parseObject(msg);
            obj.prettyPrintTo(Serial);
            Serial.println(WiFi.macAddress());
            Serial.println(obj.get<String>("macAddress"));
            // std::string x = obj.get<String>("macAddress");
            // String a = std::toupper(x);
            String deviceMacAddress = WiFi.macAddress();
            String receivedMacAddress = obj.get<String>("macAddress");
            deviceMacAddress.toUpperCase();
            receivedMacAddress.toUpperCase();
            if (receivedMacAddress.compareTo(deviceMacAddress)== 0)
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
    void connect(String name)
    {

        mqttTicker.detach();
        client.connect(name.c_str());
        if (client.connected())
        {
            if (client.subscribe("SensorsSetingsChannel"))
            {
                Serial.println("Succesfully subscribed to SensorsSetingsChannel");
            }
            mqttTicker.attach(1, [&]() {Serial.print("."); client.loop(); });
        }
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