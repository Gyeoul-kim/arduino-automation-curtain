#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>
#include "LittleFS.h"
#include <AccelStepper.h>
#include <MultiStepper.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

//define Motor driver's pin
#define IN1 5
#define IN2 4
#define IN3 14
#define IN4 12

AccelStepper stepper(AccelStepper::HALF4WIRE, IN1, IN3, IN2, IN4);

String message = "";

// Wifi SSID & PASSWORD
/*
const char* ssid = "2261030";
const char* password = "luinesuki";
*/
const char* ssid = "SK_WiFiGIGA2BA4_2.4G";//상수로 SSID 지정.
const char* password = "1701000060";//상수로 PASSWORD 기록(보안에는 안좋음)

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create a WebSocket object
AsyncWebSocket ws("/ws");

//Variables to save values from HTML form
String direction ="STOP";//기본 상태를 정지로 지정.
int steps=2048;//스텝(회전)을 2048(한바퀴)로 설정.
String Speed;//Websocket 방식으로 받은 속도 저장 목적.
int i_speed;//int 형 속도 변수.

bool notifyStop = false;//정지 알림 Default로 false 설정

//Variable for Routine Function.
const int utcOffsetInSeconds = 32400;//for match Korea Standard Time zone.//UTC 0을 기준으로 하기에 국내 시간으로 조정 위해 9시간 오프셋 지정. (1시간=3600초,9시간=32400초)
String SRO_Hour; String SRO_Min;  String SRC_Hour; String SRC_Min;//스트링 형태로 시간 저장.(R=Remote,앱에서 지정한 시간)
String isRoutine; int isRoutineI; //루틴 지정 여부 체크용
int RO_Hour; int RO_Min;  int RC_Hour; int RC_Min;//int 형태의 시간 저장.

int C_Hour; int C_Min;//기기 로컬 시간 저장 용도(C=Client,Curtain)

//Get time from Timeserver
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "kr.pool.ntp.org",utcOffsetInSeconds);//각 지역의 풀에 맞게 사용하도록 권고하고 있음


//LittleFS 파일 시스템 사용(ESP만의 기능. 내부 저장 공간 일부를 파일 저장하는데 사용 가능.)
void initFS() {
  if (!LittleFS.begin()) {//LittleFS가 시작되지 않는다면
    Serial.println("An error has occurred while mounting LittleFS");//메시지 출력
  }
  else{
    Serial.println("LittleFS mounted successfully");//메시지 출력
  }
}

// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);//와이파이 클라이언트로의 설정.
  WiFi.begin(ssid, password);//앞서 지정한 SSID,PASSWORD로 연결
  Serial.print("Connecting to WiFi ..");//시리얼에 남기기
  while (WiFi.status() != WL_CONNECTED) {//연결된 상태가 아니라면
    Serial.print('.');//출력하기
    delay(1000);//1초 대기
  }
  Serial.println(WiFi.localIP());//DHCP로부터 받아온 IP 시리얼로 출력
  if(!MDNS.begin("curtain")){//MDNS가 curtain으로 시작하지 않는다면.
      Serial.println("Error while Turning on MDNS, USE IP.");//메세지 출력
  }
  Serial.println("mDNS Is Started.");//메세지 출력
  timeClient.begin();//네트워크에서 시간 받아오기
  Serial.println("Server is on!");
}

void notifyClients(String state) {//클라이언트에 신호 전송
  ws.textAll(state);//받은거 그대로 웹소켓 통해 클라이언트로 전송.
}



void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {//웹소켓 메시지 해독
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    message = (char*)data;//웹소켓 메시지를 캐릭터 포인터형으로 변환하여 message 문자열에 넣기

    isRoutine = message.substring(message.indexOf("&")+17,message.length());
    isRoutineI=isRoutine.toInt();
    Serial.print("value :");
    Serial.println(isRoutine);
    if(!isRoutineI==1){
      Speed = message.substring(0, message.indexOf("&"));//message에서 & 앞의 문자 추출
      direction = message.substring(message.indexOf("&")+1,5);
      Serial.println(message);
      Serial.println(Speed);
      Serial.println(direction);
      steps=2048;
      i_speed=1000/Speed.toInt();

      if (direction == "OPN"){
        ESP.wdtDisable();
        stepper.move(steps);
        Serial.print("Opened");
        delay(5000);
        ESP.wdtEnable(5600);
        }
        else if(direction=="CLS"){
          ESP.wdtDisable();
          stepper.move(-steps);
          Serial.print("CLOSE");
          delay(5000);
          ESP.wdtEnable(5600);
          }else if(direction=="STP"){
            ESP.wdtDisable();
        stepper.stop();
        Serial.print("STOP");
        delay(5000);
        ESP.wdtEnable(5600);
          }
    }else{
      SRO_Hour = message.substring(message.indexOf("&")+5,8);
      SRO_Min = message.substring(message.indexOf("&")+8,11);
      SRC_Hour = message.substring(message.indexOf("&")+11,14);
      SRC_Min = message.substring(message.indexOf("&")+14,17);
      Serial.println(SRO_Hour);
      Serial.println(SRO_Min);
      Serial.println(SRC_Hour);
      Serial.println(SRC_Min);
      TimeRoutine();
    }
    notifyClients(direction);
    notifyStop = true;
  }
}
void TimeRoutine(){
  RC_Hour=SRC_Hour.toInt(); RC_Min=SRC_Min.toInt();
  RO_Hour=SRO_Hour.toInt(); RO_Min=SRO_Min.toInt();
  if((RO_Hour==C_Hour)&&(RO_Min==C_Min)){
    direction = "OPN";
  }else if((RC_Hour==C_Hour)&&(RC_Min==C_Min)){
    direction = "CLS";
  }else{
    Serial.println("error while Routine.");
  }
}

void SetCurtainTime(){
  C_Hour=timeClient.getHours();
  C_Min=timeClient.getMinutes();
  Serial.println(C_Hour);
  Serial.println(C_Min);
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
    direction = "STP";
    notifyClients(direction);
    notifyStop = false;
  }
  ws.cleanupClients();
  stepper.setMaxSpeed(i_speed);
  stepper.run(); 
  timeClient.update();
  SetCurtainTime();
  }
  
