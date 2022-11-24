/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/stepper-motor-esp8266-websocket/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <Stepper.h>

#define IN1 D5
#define IN2 D6
#define IN3 D7
#define IN4 D8
Stepper stepper(2048, IN4, IN2, IN3, IN1);

String message = "";

// Replace with your network credentials
const char* ssid = "2261030";
const char* password = "luinesuki";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create a WebSocket object
AsyncWebSocket ws("/ws");

//Variables to save values from HTML form
String direction ="STOP";
int steps=2048;

bool notifyStop = false;

// Initialize LittleFS
void initFS() {
  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
  }
  else{
    Serial.println("LittleFS mounted successfully");
  }
}

// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void notifyClients(String state) {
  ws.textAll(state);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    message = (char*)data;
    direction = message.substring(message.indexOf("&")+1, message.length());
    Serial.print("direction");
    Serial.println(direction);
    notifyClients(direction);
    notifyStop = true;
    
    if (direction == "OPN"){
        ESP.wdtDisable();
        stepper.setSpeed(20);
        stepper.step(steps);
        if(direction=="stop"){
          stepper.stop();
        }
        Serial.print("OPN");
        delay(5000);
        ESP.wdtEnable(5600);
    }
    else if(direction=="CLOSE"){
      ESP.wdtDisable();
      stepper.setSpeed(20);
      stepper.step(-steps);
      Serial.print("CLOSE");
      delay(5000);
      ESP.wdtEnable(5600);
    }else if(direction=="stop"){
      stepper.stop();
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      //Notify client of motor current state when it first connects
      notifyClients(direction);
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len);
        break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
     break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void setup() {
  // Serial port for debugging purposes

  Serial.begin(115200);
  initWiFi();
  initWebSocket();
  initFS();

  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html");
  });
  
  server.serveStatic("/", LittleFS, "/");

  server.begin();
}

void loop() {
  if (notifyStop == true){  
    direction = "stop";
    notifyClients(direction);
    notifyStop = false;
  }
  if(direction=="stop"){
        stepper.stop();
      }
  ws.cleanupClients();
}
