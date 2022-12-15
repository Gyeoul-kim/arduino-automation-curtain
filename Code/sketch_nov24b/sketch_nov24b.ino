#include <ESPAsyncTCP.h>
#include <Arduino.h>            //아두이노 명령셋
#include <ESP8266WiFi.h>        //와이파이 기능용
#include <ESPAsyncWebServer.h>  //웹서버 열기 위해
#include <ESP8266mDNS.h>        //mDNS 기능 사용 위해. 근데 이건 사과에서만 된다고 하네요. 윈도우와 안드로이드는 안되거나 반쪽임.
#include "LittleFS.h"           //파일시스템 사용 위해
#include <AccelStepper.h>       //스테퍼 라이브러리.
#include <GDBStub.h>            //for Debug.
//define Motor driver's pin
#define IN1 5
#define IN2 4
#define IN3 14
#define IN4 12

char *substr(int s, int e, char *str);        //배열용 substring for c 
char *message = "";                           //스트링 초기화.
const char *ssid = "2261030";              //상수로 SSID 지정.
const char *password = "team3131";                    //상수로 PASSWORD 기록(보안에는 안좋음)
AccelStepper stepper(8, IN2, IN4, IN1, IN3);  //스테퍼모터 지정.
AsyncWebServer server(80);                    //웹서버 오브젝트 생성.
AsyncWebSocket ws("/ws");                     //웹소켓 오브젝트 생성.
char *Direction = "STP";                      //기본 상태를 정지로 지정.
int steps = 4096;                             //스텝(회전)을 2048(한바퀴)로 설정.
char *Speed;                                  //Websocket 방식으로 받은 속도 저장 목적.
int R_speed;                                  //int 형 속도 변수.
bool notifyStop = false;                      //정지 알림 Default로 false 설정

char *SRO_Hour = "";
char *SRO_Min = "";
char *SRC_Hour = "";
char *SRC_Min = "";  //스트링 형태로 시간 저장.(R=Remote,앱에서 지정한 시간)
int RO_Hour;
int RO_Min;
int RC_Hour;
int RC_min;

char *isTrigger = "";
int isTriggerI;
int inRoutine;  //루틴 지정 여부 체크용
char *SC_Hour = "";
char *SC_Min = "";
char *SC_Sec;
char *C_Time = "";
String SC_Time = "";  //기기 시간 String형.
int C_Hour = 0;
int C_Min = 0;
int C_Sec = 0;

unsigned long currentMillis;
unsigned long previousMillis;
const long interval = 1000;

char *substr(int star, int en, char *str) {  //Substring 기능 구현

  char *niew = (char *)malloc(sizeof(char) * (en - star + 2));  //niew 차형 포인터 선언해서 en-star+2한 만큼 동적으로 지정
  strncpy(niew, str + star, en - star + 1);                     //입력받은 스트링(niew)에서 str+star부터 en-star+1만큼 복사
  niew[en - star + 1] = 0;                                      //niew 마지막에 널문자 추가.
  return niew;                                                  //niew 값 돌려보내기.
}

void Clocker() {  //시간 측정.

  //delay(1000);
  currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    C_Sec = C_Sec + 1;
    if (C_Sec == 60) {
      C_Sec = 0;
      C_Min = C_Min + 1;
    }
    if (C_Min == 60) {
      C_Min = 0;
      C_Hour = C_Hour + 1;
    }
    if (C_Hour == 24) {
      C_Hour = 0;
    }
    TimeRoutine();
    
    Serial.print("T:");
    Serial.println(C_Hour);
    Serial.print("M:");
    Serial.println(C_Min);
    Serial.print("S:");
    Serial.println(C_Sec);
  }
}

void TimeRoutine() {                                               //루틴 함수.
  if ((RO_Hour == C_Hour) && (RO_Min == C_Min) && (C_Sec <= 1) && !(C_Sec==0||C_Min==0)) {  //만약 여는시간과 시스템 시간이 일치한다면
    Direction = "OPN";
    R_speed = 1000;
    OpenCurtain();                                                        //열기
  } else if ((RC_Hour == C_Hour) && (RC_min == C_Min) && (C_Sec <= 1)&& !(C_Sec==0||C_Min==0)) {  //아니라면 닫는 시간과 시스템 시간이 일치한다면
    Direction = "CLS";
    R_speed = 1000;
    CloseCurtain();  //닫기
  } else {           //그것도 아니라면.

    Serial.println("");
    Serial.println("여는시간 :");
    Serial.print("T:");
    Serial.print(RO_Hour);  //출력.
    Serial.println("");
    Serial.print("M:");
    Serial.print(RO_Min);
    Serial.println("");
    Serial.println("닫는시간 :");
    Serial.print("T:");
    Serial.print(RC_Hour);  //출력.
    Serial.println("");
    Serial.print("M:");
    Serial.print(RC_min);
    Serial.println("");
  }
}
void initFS() {                                                       //LittleFS 파일 시스템 사용(ESP만의 기능. 내부 저장 공간 일부를 파일 저장하는데 사용 가능.)
  if (!LittleFS.begin()) {                                            //LittleFS가 시작되지 않는다면
    Serial.println("An error has occurred while mounting LittleFS");  //메시지 출력
  } else {
    Serial.println("LittleFS mounted successfully");  //메시지 출력
  }
}

void initWiFi() {                          // Initialize WiFi
  WiFi.mode(WIFI_STA);                     //와이파이 클라이언트로의 설정.
  WiFi.begin(ssid, password);              //앞서 지정한 SSID,PASSWORD로 연결
  Serial.print("Connecting to WiFi ..");   //시리얼에 남기기
  while (WiFi.status() != WL_CONNECTED) {  //연결된 상태가 아니라면
    Serial.print('.');                     //출력하기
    delay(1000);                           //1초 대기
  }
  Serial.println(WiFi.localIP());                                   //DHCP로부터 받아온 IP 시리얼로 출력
  if (!MDNS.begin("curtain")) {                                     //MDNS가 curtain으로 시작하지 않는다면.
    Serial.println("Error while Turning on MDNS, Please USE IP.");  //메세지 출력
  }
  Serial.println("mDNS Is Started.");  //메세지 출력
  Serial.println("Server is on!");     //모든 과정이 끝났다면 출력.
}

void initWebSocket() {     //웹소켓 세팅
  ws.onEvent(onEvent);     //온이벤트 호출~
  server.addHandler(&ws);  //헨들러 추가~
}

void notifyClients(char *state) {  //클라이언트에 신호 전송
  ws.textAll(state);               //받은거 그대로 웹소켓 통해 클라이언트로 전송.
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {  //웹소켓 패킷 받아오면 호출되는
  switch (type) {                                                                                                              //웹소켓 이벤트 타입에 따라 스위치 돌리기.
    case WS_EVT_CONNECT:                                                                                                       //웹소켓 연결되면
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());          //시리얼에 출력(id,아이피,원격 아이피)
      notifyClients(Direction);                                                                                                //방향 클라이언트에 전송
      break;                                                                                                                   //탈출
    case WS_EVT_DISCONNECT:                                                                                                    //연결 끊기면
      Serial.printf("WebSocket client #%u disconnected\n", client->id());                                                      //끊겼다고 시리얼ㅊ에 출력
      break;
    case WS_EVT_DATA:                          //데이터 들어오면
      handleWebSocketMessage(arg, data, len);  //함수에 던지기
      break;                                   //탈출!
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {  //웹소켓 메시지 해독
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;                        //형식은 /DOCS내 이미지 참고.
    message = (char *)data;               //웹소켓 메시지를 캐릭터 포인터형으로 변환하여 message 문자열에 넣기
    isTrigger = substr(18, 18, message);  //맨 마지막 숫자를 isTrigger 에 저장.
    isTriggerI = atoi(isTrigger);         //if문 통한 비교를 위해 int형으로 변환
    Serial.print("value :");
    Serial.println(isTriggerI);
    if (isTriggerI == 0) {
      Speed = substr(0, 0, message);  //message에서 & 앞의 문자 추출
      int Speed_i = atoi(Speed);
      Direction = substr(2, 4, message);  //& 뒤의 문자를 추출.
      Serial.println(message);            //표시
      Serial.println(Speed);
      Serial.println(Direction);
      switch (Speed_i) {
        case (1):
          R_speed = 800;
          break;
        case (2):
          R_speed = 900;
          break;
        case (3):
          R_speed = 1000;
          break;
      }
      Serial.println(R_speed);
      if (strcmp(Direction, "OPN") == 0) {  //입력받은 string 이 OPN이면
        OpenCurtain();
      } else if (strcmp(Direction, "CLS") == 0) {  //입력받은 string이 CLS라면
        CloseCurtain();
      } else {  //입력받은 string이 STP라면
        StopCurtain();
      }

    } else if (isTriggerI == 2) {
      SC_Hour = substr(6, 7, message);
      SC_Min = substr(9, 10, message);
      SC_Sec = "0";
      C_Hour = atoi(SC_Hour);
      C_Min = atoi(SC_Min);
      C_Sec = atoi(SC_Sec);
      Serial.print(SC_Hour);
      Serial.print(SC_Min);
      Serial.print(C_Sec);
    }
    if (isTriggerI == 1) {                 //루틴 돌리는 코드.
      SRO_Hour = substr(6, 7, message);    //여는시각의 시를 추출함.
      SRO_Min = substr(9, 10, message);    //여는시각의 분을 추출함.
      SRC_Hour = substr(12, 13, message);  //닫는시간의 시를 추출함.
      SRC_Min = substr(15, 16, message);   //닫는시간의 분을 추출함.
      RO_Hour = atoi(SRO_Hour);
      RO_Min = atoi(SRO_Min);
      RC_Hour = atoi(SRC_Hour);
      RC_min = atoi(SRC_Min);

      Serial.println(RO_Hour);  //표시
      Serial.println(RO_Min);
      Serial.println(RC_Hour);
      Serial.println(RC_min);
      TimeRoutine();  //루틴 함수 시작.
    }
  }
}

void OpenCurtain() {
  notifyClients(Direction);
  ESP.wdtDisable();         //딜레이 사용시 와치독 켜져 뻗어버리는 문제 발생.
  stepper.move(steps * 3);  //스텝수만큼 회전.
  Serial.print("Opened");   //열었다고 시리얼에 표시
  //delay(5000);//5초 대기
  ESP.wdtEnable(5600);  //작동 후 다시 켜줌.
  //Direction="STP";
  notifyStop = true;  //스탑 폴스로
  notifyClients(Direction);
}

void CloseCurtain() {
  notifyClients(Direction);
  ESP.wdtDisable();          //와치독 임시 비활성화.
  stepper.move(-steps * 3);  //역방향으로 한바퀴 회전.
  Serial.print("CLOSE");     //닫았다고 표시.
  //delay(5000);//5초 대기.
  ESP.wdtEnable(5600);  //다시 와치독 활성화.
  //Direction="STP";
  notifyStop = true;  //스탑 폴스로
  notifyClients(Direction);
}

void StopCurtain() {
  notifyClients(Direction);
  ESP.wdtDisable();      //와치독 비활성화
  stepper.stop();        //모터 정지(거의 바로 정지함.)
  Serial.print("STOP");  //시리얼에 정지했다고 표시.
  //delay(5000);//5초 대기
  ESP.wdtEnable(5600);  //다시 와치독 킴.
  Direction = "STP";
  notifyClients(Direction);
}

void setup() {
  // Serial port for debugging purposes

  Serial.begin(9600);                                          //시리얼 활성화
  initWiFi();                                                    //와이파이온
  initWebSocket();                                               //웹소켓 온
  initFS();                                                      //FS 온
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {  //루트로 요청하면
    request->send(LittleFS, "/index.html", "text/html");         //여기 파일 보여주기
  });
  server.serveStatic("/", LittleFS, "/");  //고정!
  server.begin();                          //서버 완전히 온
  stepper.setAcceleration(1000);           //가감속력 설정, 1000이면 거~의 즉각적 정지.
  //gdbstub_init();for debug purpose.
}

void loop() {
  if (stepper.distanceToGo() == 0 && notifyStop == true) {  //만약 스탑이 트루면
    Direction = "STP";                                      //방향 스탑
    notifyClients(Direction);                               //클라에 방향 전달
    notifyStop = false;                                     //스탑 폴스로
  }
  Clocker();
  //TimeRoutine();
  //ws.cleanupClients();//클린업!
  stepper.setMaxSpeed(R_speed);  //속도 맥시멈 지정. 위에서 변경한대로.
  stepper.run();                 //굴릴려면 루프에서 불러야
}

/*

*/
