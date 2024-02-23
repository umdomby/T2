// const char* ssid = "Robolab2";
// const char* password = "wifi123123123";
// const char* idSocket = "9955122222222222222222";

// #include <Arduino.h>
// #include <ArduinoWebsockets.h>
// #include <ESP8266WiFi.h>
// #include <ArduinoJson.h>

#include <ArduinoWebsockets.h>
#include <ESP8266WiFi.h>

const char* ssid = "Robolab22"; //Enter SSID
const char* password = "wifi123123123"; //Enter Password
const char* websockets_server_host = "217.21.54.2"; // Enter server adress
const uint16_t websockets_server_port = 8081; // Enter server port
using namespace websockets;
const char* idSocket = "995511";


#include <ArduinoJson.h>
StaticJsonDocument<512> doc2;
String output;
WebsocketsClient client;

const char* method = "";
boolean messageOnOff = true;
boolean connectByte = false;
unsigned long lastUpdate = millis();


void onMessageCallback(WebsocketsMessage messageSocket) {

    DeserializationError error = deserializeJson(doc2, messageSocket.data());
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        //lastUpdate = millis() + 15000;
        //return;
    };

    method = doc2["method"];

    // if(String(method) == "arduino"){
    //     messageOnOff = doc2["messageOnOff"];
    //     Serial.printf("test = %s\n", String(messageOnOff));
    //     doc2["method"] = "test";
    //     doc2["id"] = idSocket;
    //     doc2["txt"] = "From arduino ESP8266";
    //     doc2["messageOnOff"] = messageOnOff; 
    //     output = doc2.as<String>();
    //     client.send(output);
    // };


    if(String(method) == "test"){
        const char* id = doc2["id"];
        messageOnOff = doc2["messageOnOff"];
        Serial.printf(" id = %s", String(id));
        Serial.printf(" OnOff = %s\n", String(messageOnOff));


        //to Server
        doc2["method"] = "testFromArduino";
        doc2["id"] = idSocket;
        doc2["messageOnOff"] = messageOnOff; 
        output = doc2.as<String>();
        client.send(output);
    };

    if(String(method) == "connectByte"){
        connectByte = doc2["connectByte"];
        lastUpdate = millis();
        //connectByte = false;
        //Serial.printf("connectByte = %s\n", String(connectByte));
    };

    // Serial.print("Got Message: ");
    // Serial.println(message.data());
}

void onEventsCallback(WebsocketsEvent event, String data) {
    if(event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connnection Opened");
    } else if(event == WebsocketsEvent::ConnectionClosed) {
        Serial.println("Connnection Closed");
    } else if(event == WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
    } else if(event == WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
    }
}

void setup() {
    Serial.begin(115200);
    // Connect to wifi
    WiFi.begin(ssid, password);

    //Wait some time to connect to wifi
    for(int i = 0; i < 50 && WiFi.status() != WL_CONNECTED; i++) {
        Serial.print(".");
        delay(1000);
    }

    // run callback when messages are received
    client.onMessage(onMessageCallback);
    
    // run callback when events are occuring
    client.onEvent(onEventsCallback);

    // Connect to server
    client.connect(websockets_server_host, websockets_server_port, "/");

    doc2["method"] = "connection";
    doc2["id"] = idSocket;
    doc2["connected"] = "arduino";
    output = doc2.as<String>();
    client.send(output);

    // doc2["method"] = "test";
    // doc2["id"] = idSocket;
    // doc2["txt"] = "11111111111111111111111111111111111";
    // output = doc2.as<String>();
    // client.send(output);

    // Send a message
    //client.send("Hello Server");

    // Send a ping
    client.ping();
}

void loop() {
    client.poll();

    if (lastUpdate + 35000 < millis()){ 
        Serial.println("lastUpdate + `35000, ESP.restart()");
        ESP.restart();
        //WiFi.disconnect();
        //delay(2000);
        //WiFi.reconnect();
        lastUpdate = millis();
    };
}