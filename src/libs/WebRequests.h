#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

class WebRequests
{
     String server, port;

public:
    WebRequests();
    WebRequests(String *, String *);
    void Post(String route, String payload)
    {
        Serial.println("http://" + server + ":" + port + route);
        HTTPClient http;
        http.begin("http://" + server + ":" + port + route);
        http.addHeader("Content-Type", "application/json"); //Specify content-type header
        // http.addHeader("jwt", token);
        int httpCode1 = http.POST(payload);
        String response = http.getString(); //Get the response payload

        Serial.println(httpCode1); //Print HTTP return code
        Serial.println(response);   //Print request response payload
        http.end();
    }
    void Post(String route, String payload, String token){
        Serial.println("http://" + server + ":" + port + route);
        HTTPClient http;
        http.begin("http://" + server + ":" + port + route);
        http.addHeader("Content-Type", "application/json"); //Specify content-type header
        http.addHeader("Authorization", token);
        int httpCode1 = http.POST(payload);
        String response = http.getString(); //Get the response payload

        Serial.println(httpCode1); //Print HTTP return code
        Serial.println(response);   //Print request response payload
        http.end();
    }
    void Get(String route);
    void Get(String route, String payload, String token);
};

WebRequests::WebRequests(String *srv, String *prt)
{
    server = *srv;
    port = *prt;
}
WebRequests::WebRequests(){ }