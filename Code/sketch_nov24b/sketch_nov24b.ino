

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
#include <AccelStepper.h>
#include <MultiStepper.h>
#define IN1 5
#define IN2 4
#define IN3 14
#define IN4 12

AccelStepper stepper(AccelStepper::HALF4WIRE, IN1, IN3, IN2, IN4);

String message = "";

// Replace with your network credentials
/*
const char* ssid = "2261030";
const char* password = "luinesuki";
*/
const char* ssid = "SK_WiFiGIGA2BA4_2.4G";
const char* password = "1701000060";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create a WebSocket object
AsyncWebSocket ws("/ws");

//Variables to save values from HTML form
String direction ="STOP";
int steps=2048;
String Speed;
int speedy;

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
    Speed = message.substring(0, message.indexOf("&"));
    Speed.replace("'","");
    direction = message.substring(message.indexOf("&")+1, message.length());
    direction.replace("'","");
    Serial.print("value :");
    Serial.println(message);
    Serial.println(Speed);
    Serial.println(direction);
    notifyClients(direction);
    notifyStop = true;
    steps=2048;
    speedy=1000/Speed.toInt();
    if (direction == "OPN"){
        ESP.wdtDisable();
        stepper.move(steps);
        Serial.print("OPN");
        delay(5000);
        ESP.wdtEnable(5600);
    }
    else if(direction=="CLOSE"){
      ESP.wdtDisable();
      stepper.move(-steps);
      Serial.print("CLOSE");
      delay(5000);
      ESP.wdtEnable(5600);
    }else if(direction=="stop"){
      ESP.wdtDisable();
      stepper.stop();
      Serial.print("STOP");
      delay(5000);
      ESP.wdtEnable(5600);
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
  
  stepper.setAcceleration(1000);

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
  ws.cleanupClients();
  stepper.setMaxSpeed(speedy);
  stepper.run();
}
