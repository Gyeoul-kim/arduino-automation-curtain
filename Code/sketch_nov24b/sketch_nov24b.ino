#include <ESPAsyncTCP.h>
#include <Arduino.h>//아두이노 명령셋
#include <ESP8266WiFi.h>//와이파이 기능용
#include <ESPAsyncWebServer.h>//웹서버 열기 위해
#include <ESP8266mDNS.h>//mDNS 기능 사용 위해. 근데 이건 사과에서만 된다고 하네요. 윈도우와 안드로이드는 안되거나 반쪽임.
#include "LittleFS.h"//파일시스템 사용 위해
#include <AccelStepper.h>//스테퍼 라이브러리.

//define Motor driver's pin
#define IN1 5
#define IN2 4
#define IN3 14
#define IN4 12

AccelStepper stepper(AccelStepper::HALF4WIRE, IN1, IN3, IN2, IN4);//스테퍼모터 지정.
char *substr(int s, int e, char *str); //배열용 substring for c ref by codingdog.

char *message = "";//스트링 초기화.

// Wifi SSID & PASSWORD
/*
  const char* ssid = "2261030";
  const char* password = "luinesuki";
*/
const char* ssid = "SK_WiFiGIGA2BA4_2.4G";//상수로 SSID 지정.
const char* password = "1701000060";//상수로 PASSWORD 기록(보안에는 안좋음)

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);//웹서버 오브젝트 생성.

// Create a WebSocket object
AsyncWebSocket ws("/ws");//웹소켓 오브젝트 생성.

//Variables to save values from HTML form
char *Direction = "STP"; //기본 상태를 정지로 지정.

int steps = 2048; //스텝(회전)을 2048(한바퀴)로 설정.
char *Speed;//Websocket 방식으로 받은 속도 저장 목적.
int i_speed;//int 형 속도 변수.

bool notifyStop = false;//정지 알림 Default로 false 설정

//Variable for Routine Function.

char *SRO_Hour = ""; char *SRO_Min = "";  char *SRC_Hour = ""; char *SRC_Min = ""; //스트링 형태로 시간 저장.(R=Remote,앱에서 지정한 시간)
char *isRoutine = ""; int isRoutineI; //루틴 지정 여부 체크용

char *SC_Hour = ""; char *SC_Min = ""; char *C_Time = ""; String SC_Time = ""; //기기 시간 String형. 
int C_Hour; int C_Min;

//Get time from Timeserver


//LittleFS 파일 시스템 사용(ESP만의 기능. 내부 저장 공간 일부를 파일 저장하는데 사용 가능.)
void initFS() {
  if (!LittleFS.begin()) {//LittleFS가 시작되지 않는다면
    Serial.println("An error has occurred while mounting LittleFS");//메시지 출력
  }
  else {
    Serial.println("LittleFS mounted successfully");//메시지 출력
  }
}

// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);//와이파이 클라이언트로의 설정.
  WiFi.begin(ssid, password); //앞서 지정한 SSID,PASSWORD로 연결
  Serial.print("Connecting to WiFi ..");//시리얼에 남기기
  while (WiFi.status() != WL_CONNECTED) {//연결된 상태가 아니라면
    Serial.print('.');//출력하기
    delay(1000);//1초 대기
  }
  Serial.println(WiFi.localIP());//DHCP로부터 받아온 IP 시리얼로 출력
  if (!MDNS.begin("curtain")) { //MDNS가 curtain으로 시작하지 않는다면.
    Serial.println("Error while Turning on MDNS, Please USE IP.");//메세지 출력
  }
  Serial.println("mDNS Is Started.");//메세지 출력
  Serial.println("Server is on!");//모든 과정이 끝났다면 출력.
}

void notifyClients(char *state) {//클라이언트에 신호 전송
  ws.textAll(state);//받은거 그대로 웹소켓 통해 클라이언트로 전송.
}

char *substr(int star, int en, char *str) { //Substring 기능 구현

  char *niew = (char *)malloc(sizeof(char) * (en - star + 2)); //niew 차형 포인터 선언해서 en-star+2한 만큼 동적으로 지정
  strncpy(niew, str + star, en - star + 1); //입력받은 스트링(niew)에서 str+star부터 en-star+1만큼 복사
  niew[en - star + 1] = 0; //niew 마지막에 널문자 추가.
  return niew;//niew 값 돌려보내기.
}
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {//웹소켓 메시지 해독
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;//형식은 /DOCS내 이미지 참고.
    message = (char*)data;//웹소켓 메시지를 캐릭터 포인터형으로 변환하여 message 문자열에 넣기
    isRoutine = substr(18, 18, message); //맨 마지막 숫자를 isRoutine 에 저장.
    isRoutineI = atoi(isRoutine); //if문 통한 비교를 위해 int형으로 변환
    Serial.print("value :");
    Serial.println(isRoutine);
    if (!isRoutineI == 1) { //isRoutineI가 1이 아니라면.
      Speed = substr(0, 0, message); //message에서 & 앞의 문자 추출
      Direction = substr(2, 4, message); //& 뒤의 문자를 추출.
      Serial.println(message);//표시
      Serial.println(Speed);
      Serial.println(Direction);
      steps = 8192;
      i_speed = 1000 / atoi(Speed);
      Serial.println(i_speed);
      if (strcmp(Direction, "OPN") == 0) { //입력받은 string 이 OPN이면
        ESP.wdtDisable();//딜레이 사용시 와치독 켜져 뻗어버리는 문제 발생.
        stepper.move(steps);//스텝수만큼 회전.
        Serial.print("Opened");//열었다고 시리얼에 표시
        delay(5000);//5초 대기
        ESP.wdtEnable(5600);//작동 후 다시 켜줌.
      } else if (strcmp(Direction, "CLS") == 0) { //입력받은 string이 CLS라면
        ESP.wdtDisable();//와치독 임시 비활성화.
        stepper.move(-steps);//역방향으로 한바퀴 회전.
        Serial.print("CLOSE");//닫았다고 표시.
        delay(5000);//5초 대기.
        ESP.wdtEnable(5600);//다시 와치독 활성화.
      } else { //입력받은 string이 STP라면
        ESP.wdtDisable();//와치독 비활성화
        stepper.stop();//모터 정지(거의 바로 정지함.)
        Serial.print("STOP");//시리얼에 정지했다고 표시.
        delay(5000);//5초 대기
        ESP.wdtEnable(5600);//다시 와치독 킴.
      }
    } else { //아니라면(isRoutineI가 1이라면)
      //SRO_Hour = message.substring(message.indexOf("&")+5,8);
      SRO_Hour = substr(6, 7, message); //여는시각의 시를 추출함.
      SRO_Min = substr(9, 10, message); //여는시각의 분을 추출함.
      SRC_Hour = substr(12, 13, message); //닫는시간의 시를 추출함.
      SRC_Min = substr(15, 16, message); //닫는시간의 분을 추출함.
      Serial.println(SRO_Hour);//표시
      Serial.println(SRO_Min);
      Serial.println(SRC_Hour);
      Serial.println(SRC_Min);
      //TimeRoutine();//루틴 함수 시작.
    }
    notifyClients(Direction);//브라우저(클라이언트)에 방향 표시.
    notifyStop = true;//함수 트루로
  }
}
void TimeRoutine() { //루틴 함수.
  Serial.println("");
  Serial.print("T:");
  Serial.print(SC_Hour);//출력.
  Serial.println("");
  Serial.print("M:");
  Serial.print(SC_Min);
  Serial.println("");
  if ((strcmp(SRO_Hour, SC_Hour) == 0) && (strcmp(SRO_Min, SC_Min) == 0)) { //만약 여는시간과 시스템 시간이 일치한다면
    Direction = "OPN";//열기
  } else if ((strcmp(SRC_Hour, SC_Hour) == 0) && (strcmp(SRC_Min, SC_Min) == 0)) { //아니라면 닫는 시간과 시스템 시간이 일치한다면
    Direction = "CLS";//닫기
  } else { //그것도 아니라면.
    //TimeRoutine();//다시 루틴 돌리기.
  }
}


void SetCurrentTimeNetwork() { //현재시간 지정

  SC_Time.toCharArray(C_Time, SC_Time.length() + 2);
  SC_Hour = substr(17, 18, C_Time); //여는시각의 시를 추출함.
  SC_Min = substr(20, 21, C_Time); //여는시각의 분을 추출함.
  //Serial.println(SC_Hour);
  C_Hour = atoi(SC_Hour);
  C_Min = atoi(SC_Min);
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {//여기는 진짜 모르겠다 ㅎㅎㅎ
  switch (type) {//웹소켓 이벤트 타입에 따라 스위치 돌리기.
    case WS_EVT_CONNECT://웹소켓 연결되면
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());//어..시리얼에 출력(id,아이피,원격 아이피)
      notifyClients(Direction);//방향 클라이언트에 전송
      break;//탈출
    case WS_EVT_DISCONNECT://연결 끊기면
      Serial.printf("WebSocket client #%u disconnected\n", client->id());//끊겼다고 시리얼ㅊ에 출력
      break;
    case WS_EVT_DATA://데이터 들어오면
      handleWebSocketMessage(arg, data, len);//함수에 던지기
      break;//탈출!
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {//웹소켓 세팅
  ws.onEvent(onEvent);//온이벤트 호출~
  server.addHandler(&ws);//헨들러 추가~
}

void setup() {
  // Serial port for debugging purposes

  Serial.begin(9600);//시리얼 활성화
  initWiFi();//와이파이온
  initWebSocket();//웹소켓 온
  initFS();//FS 온
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) { //루트로 요청하면
    request->send(LittleFS, "/index.html", "text/html");//여기 파일 보여주기
  });
  server.serveStatic("/", LittleFS, "/");//고정!
  server.begin();//서버 완전히 온
  stepper.setAcceleration(1000);//가감속력 설정, 1000이면 거~의 즉각적 정지.
  getTime();
  SetCurrentTimeNetwork();//변수에 지정.
}
void UpdateCurrentHour(){
  delay(60000*60);
  C_Hour++;
  
 
}
void UpdateCurrentMin(){
  delay(60000);
  C_Min++;
  
 
}
void getTime() {
  WiFiClient client;
  while (!!!client.connect("google.com", 80)) { //  google.com 연결 안되면, !!! 또는 ! 는 부울
    Serial.println("connection failed, retrying...");
  }

  client.print("HEAD / HTTP/1.1\r\n\r\n"); // google.com 에 접속해서 헤더파일 받아오는 명령어?

  while (!!!client.available()) {} // !!! 또는 ! 는 부울

  while (client.available()) {
    if (client.read() == '\n') {
      if (client.read() == 'D') {
        if (client.read() == 'a') {
          if (client.read() == 't') {
            if (client.read() == 'e') {
              if (client.read() == ':') {
                client.read();
                SC_Time = client.readStringUntil('\r');
                client.stop();
              }
            }
          }
        }
      }
    }
  }
}
void loop() {
  if (notifyStop == true) { //만약 스탑이 트루면
    Direction = "STP";//방향 스탑
    notifyClients(Direction);//클라에 방향 전달
    notifyStop = false;//스탑 폴스로
  }
  //ws.cleanupClients();//클린업!
  stepper.setMaxSpeed(i_speed);//속도 맥시멈 지정. 위에서 변경한대로.
  stepper.run(); //굴릴려면 루프에서 불러야
  UpdateCurrentHour();
  UpdateCurrentMin();
  Serial.println(C_Hour);
  Serial.println(C_Min);
  
  
  //Serial.println(SC_Min);
}
