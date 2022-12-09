# arduino IoT curtain

## KR   
배재대학교 2022학번 AI소프트웨어공학부 컴퓨터공학전공   
창의공학설계 08분반 팀 31 프로젝트   
2261030 김현우 2261031 신동원   
   
## EN   
Team Project of Pai Chai University, Major of Computer Engineering   
Creative Engineering Design 08 Placement ,Team 31 project   

## 프로잭트 소개
Wi-Fi통신으로 커튼을 조작 할 수 있는 장치   
  >조작 명령을 위한 앱 개발     
  >Wi-Fi통신 코드 개발   
  >동작 속도 단계 조절 코드 개발   
  >동작 정지 기능 코드 개발
  >루틴 기능 아두이노 코드 개발 

## Project Introduction
A device that can manipulate curtains with Wi-Fi communication   
  >Develop apps for manipulation commands   
  >Development of routine function to Arduino code
  >Development of Wi-Fi Communication Code   
  >Development of a speed control code   
  >Development of Stop Function Code

## 핵심재료 / Main Parts List
**ESP 8266 NODE V3**   
**28BYJ-48 스텝 모터** / **28BYJ-48 Stepper Motor**   
**ULN2003 모터 드라이버 모듈** / **ULN2003 Motor Driver Module**   

## 연구배경 및 필요성
 인터넷이 널리 보급되면서 사물에도 인터넷 기술을 접목시킬 수 있게 되었습니다.    
이 사물인터넷(이하IoT)은 기존에 사용자가 직접 기계를 작동시키던 방식에서    
사용자의 원하는 루틴에 따라 작동할 수 있는 자동화를 가능하게 만들었습니다.    
현제 IoT기기들은 다양하게 존재 하지만 새로운 기기를 구매하는 것이 아니라면    
기존에 사용하고 있는 인터넷이 지원되지 않는 기기는 IoT시스템을 사용할 수가 없습니다.    
이는 비용적인 측면에서 상당한 부담이 됩니다. 현제 시중에 판매되는 대형가전이 아니라면    
같은 품목에서 IoT를 지원하지 않는 물건과 IoT를 지원하는 물건은 가격에 적어도 1.5배에서    
2배이상 차이가 납니다. 때문에 저희 팀은 IoT를 지원하지 않는 가구 혹은 기기에 물리적으로    
IoT기능을 지원할 수 있는 스위치 봇을 개발하고자 하였고 다양한 방법 중 커튼을 물리적으로    
열고 닫을수 있는 기계를 만들기로 하였습니다. 이 기계는 휴대폰에 APP이나 Web으로 접속하여   
조작 할 수 있으며 개발속도에 따라 개인 루틴을 추가하거나 조도센서와 같은 외부 센서를    
통하여 상황에 적응하는 방식을 고려하고 있습니다.

## 목표
### 원격으로 기기를 조작하고 커튼 조작을 정확히 수행할 수 있는 장치 개발
- ESP8266을 사용하여 아두이노가 원격으로 신호를 받을 수 있는 코드 개발
- 모든 동작 명령을 내릴수 있는 앱 개발
- 명령을 받아 28BYJ-48모터를 작동시킬 수 있는 코드 개발
- ULN2003모듈을 사용하여 동작 속도를 조절할 수 있는 코드 개발
- 모터의 힘으로 커튼을 움직일 수 있는 하드웨어 개발
- ~서버의 시간을 받아와~ 현재시간을 설정하고 특정시간에 기기가 동작하는 코드 개발(루틴기능) (개발성공)  
    
## 동작 예시 사진(우리가 만들고 싶은것) / working pictures(what we want to do)   
![GIF](./Docs/curtains-opening.gif)   
> 출처 image by Geniusness / https://github.com/Geniusness/Genius-AutoCurtains    
   
## 아두이노 코드 개발
-설명이 굉장히 많으니 기능 단위별로 끈어서 보시는게 편합니다.   
-코드 바로 하단에 있는 글이 코드를 설명하는 글입니다.

### 코드 설명   

    #include <ESPAsyncTCP.h>
    #include <Arduino.h>  //아두이노 명령셋
    #include <ESP8266WiFi.h>  //와이파이 기능용
    #include <ESPAsyncWebServer.h>  //웹서버 열기 위해
    #include <ESP8266mDNS.h>
    #include "LittleFS.h"  //파일시스템 사용 위해
    #include <AccelStepper.h>  //스테퍼 라이브러리.
    #include <GDBStub.h>  //for Debug.
    
아래 설명할 코드를 위해 필요한 Library 종류입니다.  

    //define Motor driver's pin
    #define IN1 5 
    #define IN2 4
    #define IN3 14
    #define IN4 12

모터 드라이버의 핀을 정의하여줍니다.
	

    char *substr(int s, int e, char *str);  //배열용 substring for c ref by codingdog.
    char *message = "";  //앱으로부터 받아오는 정보를 저장할 문자열입니다.
    const char* ssid = "각자_쓰는_와이파이_이름";  //상수로 SSID 지정.
    const char* password = "비밀번호";//상수로 PASSWORD 기록(보안에는 안좋음),공개형 와이파이인 경우 공란으로 둡니다.
    AccelStepper stepper(AccelStepper::HALF4WIRE, IN2, IN4, IN1, IN3);  //스테퍼모터의 타입, 와이어를 지정합니다.
    AsyncWebServer server(80);  //웹서버 오브젝트를 80 포트를 통해 받도록 지정합니다. 80포트는 http 프로토콜의 기본 포트입니다.
    AsyncWebSocket ws("/ws");  //웹소켓 오브젝트 생성하여 웹소켓 패킷은 해당 주소로만 받도록 지정합니다.
    char *Direction = "STP";  //기본 상태를 STP 즉, 정지 상태로 지정해둡니다.
    int steps = 0;  //스텝(회전)을 0으로 초기화 합니다.
    char *Speed;  //Websocket 방식으로 받은 속도 저장하기 위해 생성합니다.
    int R_speed;  //int 형 속도 변수.
    bool notifyStop = false;//정지 알림 Default로 false 설정
    /*
    char *SRO_Hour = ""; char *SRO_Min = "";  char *SRC_Hour = ""; char *SRC_Min = "";  //스트링 형태로 열고 닫는 시간을 저장합니다.(R=Remote,앱에서 지정한 시간)
    int RO_Hour; int RO_Min; int RC_Hour; int RC_min;  //위의 스트링 형태로 받은 시간을 atoi 함수를 통해 정수형으로 변환한 뒤 저장하기 위해 쓰이는 함수입니다.
    */
    char *isTrigger = ""; int isTriggerI; //int inRoutine; 루틴 상태인지,어떤 모드를 요청하였는지 구분하기 위해 쓰이는 변수입니다.
    char *SC_Hour = ""; char *SC_Min = ""; char *SC_Sec; char *C_Time = ""; String SC_Time = "";  //기기 시간 String형태로 받아오기 위해 쓰이는 변수입니다. 
    int C_Hour=0; int C_Min=0; int C_Sec=0;  //받아온 시간을 int 형태로 변환하여 저장하기 위해 쓰이는 변수입니다.
    
   코드 내에서 사용할 변수들을 선언해 줍니다.
   
    unsigned long currentMillis;
    unsigned long previousMillis;
    const long interval = 1000;
    
   delay()를 사용할 때 생기는 문제점을 해결하기 위해, millis()를 이용하기 위해 변수를 선언해 줍니다.
    
    char *substr(int star, int en, char *str) {  //Substring 기능 구현
    
      char *niew = (char *)malloc(sizeof(char) * (en - star + 2));  //niew 차형 포인터 선언해서 en-star+2한 만큼 동적으로 지정
      strncpy(niew, str + star, en - star + 1);  //입력받은 스트링(niew)에서 str+star부터 en-star+1만큼 복사
      niew[en - star + 1] = 0;  //strncpy는 \0을 가져오지 않음으로 마지막에 추가해 줍니다. niew 마지막에 널문자 추가.
      return niew;  //niew 값 돌려보내기.
    }
아두이노 기본에 들어있는 substring 함수는 string 형으로 선언된 변수에만 사용이 가능하여, char 포인터 형에도 사용이 가능한 substring 함수를 구현하였습니다.

    void Clocker(){  //시간 측정.
    

      currentMillis=millis();  //값을 millis로 지정.
    
      if(currentMillis-previousMillis >=interval){  //만약 현재 밀리초값-이전 밀리초값이 1000보다 크다면
        previousMillis=currentMillis;  //이전 밀리초값을 현재 밀리초값으로 지정
        C_Sec=C_Sec+1;//초에 초+1
        if(C_Sec==60){  //만약 초가 60이라면
          C_Sec=0;  //초를 0으로
          C_Min=C_Min+1;  //분을 분+1로
        }
        if(C_Min==60){  //만약 분이 60이라면
          C_Min=0;  //분을 0으로
          C_Hour=C_Hour+1;  //시를 시+1로
        }if(C_Hour==24){  /만약 시가 24라면
          C_Hour=0;  //시를 0으로
        }
      }
    }
 

   안에서 구동하는 시계 함수입니다. 지금의 밀리초와 과거의 밀리초 차가 1000ms가 되면 함수는 작동합니다.   
   delay를 사용하는 경우 해당 함수를 실행하기 위해 모든 프로세스가 일시정지를 함으로 작동하지 않은 문제가 있어 millis 함수를 이용하여 제작하였습니다.   
   

     void initFS() {  //LittleFS 파일 시스템 사용(ESP만의 기능. 내부 저장 공간 일부를 파일 저장하는데 사용 가능.)
          if (!LittleFS.begin()) {  //LittleFS가 시작되지 않는다면
            Serial.println("An error has occurred while mounting LittleFS");  //메시지 출력
          }
          else {
            Serial.println("LittleFS mounted successfully");  //메시지 출력
          }
        }
  

ESP8266의 기능 중 하나인 FS 시스템을 사용하기 위한 함수입니다. 내부 저장공간의 일부를 파일 저장을 위한 공간으로 사용이 가능하여 여러가지 시스템을 구현할 수 있습니다.   
   

     void initWiFi() {  // Initialize WiFi
          WiFi.mode(WIFI_STA);  //와이파이 클라이언트로의 설정.
          WiFi.begin(ssid, password);   //앞서 지정한 SSID,PASSWORD로 연결
          Serial.print("Connecting to WiFi ..");  //시리얼에 남기기
          while (WiFi.status() != WL_CONNECTED) {  //연결된 상태가 아니라면
            Serial.print('.');  //출력하기
            delay(1000);  //1초 대기
          }
          Serial.println(WiFi.localIP());  //DHCP로부터 받아온 IP 시리얼로 출력
          if (!MDNS.begin("curtain")) {  //MDNS가 curtain으로 시작하지 않는다면.
            Serial.println("Error while Turning on MDNS, Please USE IP.");  //메세지 출력
          }
          Serial.println("mDNS Is Started.");  //메세지 출력
          Serial.println("Server is on!");  //모든 과정이 끝났다면 출력.
        }
    

와이파이 연결 하는 함수입니다. 와이파이를 연결한 뒤 MDNS 서비스를 시작합니다.    
MDNS 서비스를 사용하면 IP를 몰라도 접속이 가능하지만 현재는 애플사의 기기만 온전히 지원하며, 타사의 기기에서는 지원이 안되거나 반쪽으로 지원하는 단점이 있습니다.   

 
        void notifyClients(char *state) {  //클라이언트에 신호 전송
          ws.textAll(state);  //받은거 그대로 웹소켓 통해 클라이언트로 전송.
        }
  

클라이언트(앱)에 웹소켓 프로토콜을 통해 현재 상태를 전송하는 코드입니다.
   

     void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {  //웹소켓 메시지 해독
          AwsFrameInfo *info = (AwsFrameInfo*)arg;
          if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
            data[len] = 0;//형식은  /DOCS내 이미지 참고.
            message = (char*)data;  //웹소켓 메시지를 캐릭터 포인터형으로 변환하여 message 문자열에 넣기
            isTrigger = substr(18, 18, message);  //맨 마지막 숫자를 isTrigger 에 저장.
            isTriggerI = atoi(isTrigger);  //if문 통한 비교를 위해 int형으로 변환
            Serial.print("value :");
            Serial.println(isTriggerI);  //모드 값 출력합니다.
            if(isTriggerI==0){  //만약 모드가 0이면
              Speed = substr(0, 0, message);  //message에서 & 앞의 문자 추출
              int Speed_i=atoi(Speed);  //Speed 값을 정수형으로 변환하여 Speed_i에 저장합니다.
              Direction = substr(2, 4, message);  //& 뒤의 문자를 추출.
              Serial.println(message);  //표시
              Serial.println(Speed);  //표시
              Serial.println(Direction);  //표시
              steps = 20480;  //스텝을 20480으로 지정합니다.
              switch (Speed_i) {  //받아온 속도값을 스위치에 넣습니다.
                case(1):  //1이라면
                R_speed = 500;  //회전 속도를 500으로 지정합니다.
                break;  //나가기
                case(2):  //2라면
                R_speed = 750;  //회전 속도를 750으로 설정합니다.
                break;  //나가기
                case(3):  //3이라면
                R_speed = 1000;  //회전 속도를 1000으로 설정합니다.
                break;  //나가기
              }
              Serial.println(R_speed);  //R_Speed를 출력합니다.
              if (strcmp(Direction, "OPN") == 0) {  //입력받은 string 이 OPN이면
                notifyClients(Direction);
                ESP.wdtDisable();  //와치독을 잠시 꺼줍니다.
                stepper.move(-steps);  //스텝수만큼 회전.
                Serial.print("Opened");  //열었다고 시리얼에 표시
                //delay(5000);  //5초 대기
                ESP.wdtEnable(5600);  //작동 후 다시 켜줌.
                //Direction="STP";
                notifyClients(Direction);
              } else if (strcmp(Direction, "CLS") == 0) {  //입력받은 string이 CLS라면
                notifyClients(Direction);
                ESP.wdtDisable();  //와치독 임시 비활성화.
                stepper.move(10240);  //10240스탭만큼 회전.
                Serial.print("CLOSE");  //닫았다고 표시.
                //delay(5000);  //5초 대기.
                ESP.wdtEnable(5600);  //다시 와치독 활성화.
                //Direction="STP";
                notifyClients(Direction);
              } else {  //입력받은 string이 STP라면
                notifyClients(Direction);
                ESP.wdtDisable();  //와치독 비활성화
                stepper.stop();  //모터 정지(거의 바로 정지함.)
                Serial.print("STOP");  //시리얼에 정지했다고 표시.
                //delay(5000);  //5초 대기
                ESP.wdtEnable(5600);  //다시 와치독 킴.
                Direction="STP";
                notifyClients(Direction);
              }
        
            }else if(isTriggerI==1){
              SC_Hour=substr(6,7,message);  //메세지의 6번째부터 7번째까지의 패킷을 추출합니다.
              SC_Min=substr(9,10,message);  //메세지의 9번째부터 10번째까지의 패킷을 추출합니다.
              SC_Sec="0";  //초를 0으로 지정합니다.
              C_Hour=atoi(SC_Hour);  //받아온 시를 정수형으로 바꿔 저장합니다.
              C_Min=atoi(SC_Min);  //받아온 분을 정수형으로 바꿔 저장합니다.
              C_Sec=atoi(SC_Sec);  //받아온 초를 정수형으로 바꿔 저장합니다.
              Serial.print(SC_Hour);  //시리얼에 표시합니다.
              Serial.print(SC_Min);  //시리얼에 표시합니다.
              Serial.print(C_Sec);  //시리얼에 표시합니다.
            }else{
            }
          }
        }
    

웹소켓 패킷을 받아 분석하여 그에 맞는 동작을 하는 코드입니다.

    void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {  //웹소켓 패킷을 처리하기 위한 코드입니다.
      switch (type) {  //받아온 타입에 따라 처리하는 코드입니다.
        case WS_EVT_CONNECT://웹소켓 연결 상태라면
          Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());  //시리얼에 출력(id,아이피,원격 아이피)
          notifyClients(Direction);  //방향 클라이언트에 전송
          break;  //나가기
        case WS_EVT_DISCONNECT:  //연결 끊기면
          Serial.printf("WebSocket client #%u disconnected\n", client->id());//끊겼다고 시리얼에 출력
          break;  //나가기
        case WS_EVT_DATA:  //데이터 들어오면
          handleWebSocketMessage(arg, data, len);  //함수에 던지기
          break;  //나가기
        case WS_EVT_PONG:  //PING 입력을 받은 상태일때 실행할 케이스이지만, 핑 입력을 보낼 일이 없어 제외했습니다.
        case WS_EVT_ERROR:
          break;
      }
    }
클라이언트로부터 받아온 웹소켓 패킷을 분석하고 처리하기 위한 코드입니다.

    void initWebSocket() {  //웹소켓 세팅
      ws.onEvent(onEvent);  //온이벤트 함수를 호출합니다.
      server.addHandler(&ws);  //헨들러를 추가합니다.
    }
웹소켓을 시작하는 코드입니다.

    void setup() {
      // Serial port for debugging purposes
    
      Serial.begin(115200);  //시리얼 활성화
      initWiFi();  //와이파이를 활성화합니다.
      initWebSocket();  //웹소켓을 활성화합니다.
      initFS();  //FS를 활성화합니다.
      /*
      server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {  //루트로 요청하면
        request->send(LittleFS, "/index.html", "text/html");  //여기 파일 보여주기
      });  //웹 클라이언트 개발의 흔적입니다,
      */
      server.serveStatic("/", LittleFS, "/");  //고정 상태로 지정합니다.
      server.begin();  //서버를 시작합니다.
      stepper.setAcceleration(1000);  //가감속력 설정합니다. 1000의 경우 즉각적인 정지에 가깝습니다.
      //gdbstub_init();gdb 디버깅 용도입니다.
    }
ESP8266이 시작할때 작동하는 코드입니다.

    void loop() {
      if (notifyStop == true) {  //만약 스탑이 트루면
        Direction = "STP";  //방향 스탑
        notifyClients(Direction);  //클라이언트에 방향 전달
        notifyStop = false;  //스탑 폴스로
      }
      //Clocker(); 루틴 기능 개발의 흔적입니다.
      //TimeRoutine(); 루틴 기능 개발의 흔적입니다.
      //ws.cleanupClients();  //클라이언트를 주기적으로 클린업합니다.
      stepper.setMaxSpeed(R_speed);  //속도 맥시멈 지정. 위에서 변경한대로.
      stepper.run();  //계속해서 호출해야 모터가 굴러갑니다.
    
    }
    
전원이 인가해있는 동안 반복적으로 작동하는 코드입니다.
   

     /*
        if(isTriggerI==1){//루틴 돌리는 코드.
              //inRoutine=1;
              SRO_Hour = substr(6, 7, message); //여는시각의 시를 추출함.
              SRO_Min = substr(9, 10, message); //여는시각의 분을 추출함.
              SRC_Hour = substr(12, 13, message); //닫는시간의 시를 추출함.
              SRC_Min = substr(15, 16, message); //닫는시간의 분을 추출함.
              RO_Hour=atoi(SRO_Hour);
              RO_Min=atoi(SRO_Min);
              RC_Hour=atoi(SRC_Hour);
              RC_min=atoi(SRC_Min);
    
          Serial.println(RO_Hour);//표시
          Serial.println(RO_Min);
          Serial.println(RC_Hour);
          Serial.println(RC_min);
          inRoutine=1;
          TimeRoutine();//루틴 함수 시작.
          
        }else
    
 

   루틴 기능 개발의 흔적입니다. 개발 능력 한계/기기의 한계로 인해 완성하지 못했습니다.
   
    void TimeRoutine() {  //루틴 함수.
      if ((RO_Hour==C_Hour) && (RO_Min == C_Min)) {  //만약 여는시간과 시스템 시간이 일치한다면
        Direction = "OPN";  //열기
        inRoutine=0;
      } else if ((RC_Hour == C_Hour) && (RC_min, C_Min)) {  //아니라면 닫는 시간과 시스템 시간이 일치한다면
        Direction = "CLS";  //닫기
        inRoutine=0;
      } else {  //그것도 아니라면.
        if(currentMillis-previousMillis >=interval){
        previousMillis=currentMillis;
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
    }
    */
루틴 함수입니다.

## 서버와 앱 소통 방식 / 텍스트 규격
![WebSocket Packet](https://user-images.githubusercontent.com/117341089/206406701-058912a0-887f-4dbd-bd59-982a8c334e98.png)    
저희 팀이 만든 앱에서 서버로 메시지를 전송할 때 사용하는 메시지 규격입니다.   
해당 규격을 통해서 앱에서 데이터를 전송하며 배열번호 18번 자리가 식별자로써 서버가 배열의 데이터를 어디까지만 읽고 저장할지 구분시켜줍니다.
주의할점은 배열번호가 칸으로 나누어져 있기 때문에 자리수가 틀어지면 안됩니다.   
2-4번은 무조껀 텍스트 글자가 3자여야 하며, 6-7번은 반드시 2자리 숫자가 들어와야 합니다. 다른 번호 또한 마찬가지입니다.   
   
## 앱 개발
-설명 순서는 앱 사용시 사용순서와 일치하도록 배치하였습니다.   
-사진의 바로 밑에있는 글이 코드를 설명하는 글입니다.   
   
![전역번수](https://user-images.githubusercontent.com/117341089/206365355-f7b39e60-556f-4e3b-bf84-d80a402e4c0f.PNG)   
전역변수 초기값   
    
![메인화면](https://user-images.githubusercontent.com/117341089/206363257-6f0aabe3-1efc-4b48-8e74-2c02c139a65b.PNG)   
앱 메인 화면  
   
![연결버튼](https://user-images.githubusercontent.com/117341089/206363602-5c3c2375-76f7-437b-80c0-e82fe0c3738b.PNG)   
주소창에 주소 입력이후 연결 버튼을 누르면 전역변수에 IP주소값이 URL형식으로 값이 들어가게 되고 앱소켓 방식으로 서버와 연결을 시도합니다.   
이떄 전역변수 PORT값은 아두이노 코드에서 주소에 접근하려는 시도가 있을때 접근의도를 파악하기위해 포트값으로 80(=PORT값)이 있는지 확인합니다.    
단순하게 PORT값은 웹소켓 방식으로 연결하는데 필요한 값입니다.   

![웹 소켓 연결 상태 표기](https://user-images.githubusercontent.com/117341089/206368728-54a1ab19-2b9f-4bf1-b720-ba5d971b9cf2.PNG)   
연결 버튼을 누른이후 연결 상태를 메인화면 상태창에 연결 상태를 표기합니다.   
   
![현재시간 설정](https://user-images.githubusercontent.com/117341089/206370277-9c3dda89-f172-4689-93fe-01296840a754.PNG)   
연결 성공이후 현재시간을 설정해줘야 합니다.   
웹소켓 방식으로 텍스트를 보내며 서버에서는 택스트 끝에 2값을 먼져 받아들여 CTI와 전역변수 CURtimeH(현재:시), CURtimeM(현재:분) 값만 읽고 저장합니다.   
   
![반환 데이터 진행 표기](https://user-images.githubusercontent.com/117341089/206371215-4ea7866d-eafa-4905-ba25-92b7092c14fa.PNG)     
서버는 동작 명령을 받고 텍스트를 읽어 해당동작에 해당하는 텍스트를 앱으로 반환합니다.    
OPN,CLS,TIM값을 반환하며 앱은 받은 텍스트를 IF문으로 받아들여 현제 동작상태를 상태표기창에 표기합니다.   
   
![속도설정](https://user-images.githubusercontent.com/117341089/206369121-f55c2fea-5529-497e-869d-e8a273c63337.PNG)   
속도설정 버튼을 클릭하면 1단계 = 값 3 방식으로 반비례한 값을 SPEED에 저장합니다.    
![속도설정 피드백](https://user-images.githubusercontent.com/117341089/206369974-f2f6be9f-b39f-423f-ac92-84064370ef7a.PNG)   
속도가 변경되었음을 보여주기 위해서 버튼 클릭이후 사용자에서 피드백을 바로 보여주는 화면을 만들어 속도변경을 인지하도록 만들었습니다.   
   
![BtnON](https://user-images.githubusercontent.com/117341089/206371777-4009a2e7-594c-4e60-b78b-447f9f6aaf55.PNG)
![BtnOFF](https://user-images.githubusercontent.com/117341089/206371784-1b5d8494-5584-4407-a151-588b69e11d8f.PNG)   
열기/닫기 버튼을 누를경우 동작하는 함수입니다.   
서버는 텍스트 맨 끝에 0값을 먼져 읽고 맨앞에 SPEED값과 OPN또는CLS 값을 읽고 해당 요청을 수행합니다.   
뒤에있는 전역변수값은 서버에서 무시하기에 어떠한 값이 들어가도 상관없습니다.   
열기/닫기 버튼을 클릭했음을 사용자에게 알리기 위해 소리 피드백을 추가하였습니다. BBI-TONG 이라는 소리가 납니다.   
   
![BtnSTOP](https://user-images.githubusercontent.com/117341089/206372448-16fbe6f0-3e57-4f7c-a12d-e9c4f1f40e2e.PNG)   
정지버튼을 누를경우 동작하는 함수입니다.
모든 동작을 정지하도록 명령을 보내기에 서버에서는 0값을 먼저 읽고 앞에 SPEED값과 STP 텍스트만 읽고 기기에 동작을 정지합니다.   
정지 버튼을 클릭했음을 사용자에게 알리기 위해 소리 피드백을 추가하였으며 열기/닫기와는 다른 BBI-BBI 소리가 나도록 설정하여 열기/닫기와 다른 정지 기능을 사용했음을 사용자에게 분간할 수 있도록 하였습니다.   
    
![OPEN루틴설정](https://user-images.githubusercontent.com/117341089/206373200-53f62d8a-95ac-41a0-8829-82e15f0dabc0.PNG)
![CLOSE루틴설정](https://user-images.githubusercontent.com/117341089/206373184-bc539a43-e599-4fcf-8679-0e91682a4eb1.PNG)   
루틴 설정을 위해 버튼을 클릭하면 시간 설정 화면이 나옵니다.   
시간 설정이후 사용자가 설정한 시간값을 화면에 표기합니다.  
이후 전역변수에 설정한 시간값을 저장합니다. IF문을 통해 시간은 5분 단위로만 설정할 수 있도록 재한하였습니다.   
![루틴설정화면](https://user-images.githubusercontent.com/117341089/206374695-e0f10afa-f693-4a4d-996d-41b90f0c5832.PNG)
![루틴시간설정툴팁](https://user-images.githubusercontent.com/117341089/206374700-0ed06940-43e1-4aff-9c66-bfdcff3d617d.PNG)   
5분 단위가 아닐경우 5분으로 설정하도록 툴팁을 만들었습니다.   
전역변수를 성공적으로 설정하면 서버는 텍스트 맨뒤의 1값으 먼저 읽고 앞에 TIM 텍스트와 뒤에 나머지 4개의 전역변수 값을 읽고 서버에 저장합니다.   
이후 서버에 저장되어있는 현재시간 값과 루틴설정으로 만든 시간값이 일치하면 열기/닫기 동작을 수행합니다.   
   
## 하드웨어 제작 / 시제품 제작
   
![시선대각 모형 닫음](https://user-images.githubusercontent.com/117341089/206663714-3548313e-f805-4017-a40f-81b0fd8e3377.PNG)
![시선대각 모형](https://user-images.githubusercontent.com/117341089/206663724-0be49529-087c-4bfe-b700-74d41aee22b4.PNG)   
SketchUp 무료툴을 사용해 제작한 초기형 모델입니다.   
   
![image](./Docs/Genius-AutoCurtain-BeltandTensioner.jpg)    
> 출처 image by Geniusness / https://github.com/Geniusness/Genius-AutoCurtains      
처음에는 사진처럼 옆으로 슬라이딩하는 방식으로 제작하려 했지만 하드웨어 구조가 너무 복잡해진다고 판단되어서 위아래로 롤링하는 방식으로 변경하였습니다.   

![시제품 완성](https://user-images.githubusercontent.com/117341089/206667713-ef3639a9-a3af-4ed4-b57a-be0ea232816a.PNG)   
초기형 3D 모델을 토대로 만든 시제품입니다.   
   
## 어려웠던 부분 / 아쉬웠던 부분  
ESP8266이 다루기 굉장히 까다로운 부품이어서 코드를 작성할 수록 오류가 늘어났습니다.    
분명 논리적으로는 오류가 없는 코드이지만 ESP8266에서 지원하는 기능들이 많아 오히려 역으로 기능들이 서로 충돌하면서 코드를 짜는데 굉장히 어려움이 많았습니다.   
특히 루틴기능을 만들때 시간을 굉장히 많이 빼앗기게 되었습니다.   
루틴기능은 알람설정이라는 예제를 많이 참조하여 작성할 예정이었습니다.   
루틴 기능 구현을 위해서 코드안에서 돌아갈 시간데이터가 있고 사용자가 원하는 시간에 기기를 동작하도록 개념설개를 하였었습니다.   
코드안에서 돌아갈 시간데이터를 서버에서 돌아가고 있는 현재시간을 NTP(Network Time Protocol)라이브러리를 사용해서 시간데이를 가져오고 코드에 활용하려 하였으나 NTP라이브러리가 기존코드와 계속 충돌하여 중간에 코드가 계속 뻗어버렸고 이 때문에 3주 이상의 시간을 허비했습니다.   
정규적인 수업시간 이외에도 계속 시간을 가지고 기능을 구현하려 시도하였으나 코드가 계속 새로운 오류를 가져오는 바람에 결국 NTP라이브러리를 사용하지 못하고 앱에서 사용자가 처음 사용할 때 현재 시간을 입력하여 서버로 시간데이터를 전송하는 방식으로 해결하였습니다.   
그렇게 서버로부터 시작시간을 받아서 코드내부에서 시간데이터 값에 1초씩 더해서 가상시계를 만들었습니다.   
이후 코드내부에서 사용자가 원하는 기간까지 기다리다가 일치하면 동작하는 코드를 작성하던중 ESP8266의 약한 프로세스 처리 능력 때문에 시간처리와 동작처리를 원활하게 처리하지 못하는 상황을 발견하게 되었습니다.   
교수님께 자문을 구해 루틴코드를 인터럽트 방식으로 처리해보려 하였으나 이 역시 개발능력과 기기의 프로세스 처리에 한계 때문에 구현하지 못했습니다.   
여전히 시도해볼 방법으로 ESP8266보드만 쓰는것이 아닌 MEGA보드를 추가로 사용하여 각각 보드에 다른 코드를 사용하여 명령처리를 나누는 것을 시도해보고 싶었지만 시간적으로 개발이 불가능 할것 같아 결국 루틴기능 구현을 포기하게 되었습니다.   
물론 이 루틴기능이 처음 계획에서는 프로젝트에서 시간이 여유가 생기면 도전적으로 해볼 생각이었습니다만 시간적 여유가 어느정도 있었음에도 구현하지 못해서 아쉬움이 많이 남습니다.   
루틴기능 이외에도 기기의 동작상태와 속도를 알려주는 디스플레이를 추가한다거나 조도센서를 사용하여 빛의 세기로 기기를 자동으로 동작하게 만드는 기능을 구현하고 싶었으나 시간의 벽에 막혀 시도해보지 못해 프로젝트에 아쉬움이 많이 남습니다.   
   
## references

https://github.com/Geniusness/Genius-AutoCurtains //프로젝트 개념설계 참고자료

https://github.com/CemraJC/smart-autocurtains 

https://randomnerdtutorials.com/stepper-motor-esp8266-websocket/ //ESP8266 모터조작

https://diyver.tistory.com/125 //ESP8266서버 생성과 Wi-fi연결

https://blog.naver.com/speedprinse/221388363130 //앱인벤터를 이용한 ESP8266연결 개념설계 참고자료

https://medium.com/kidstronics/mit-app-inventor-2-meets-esp8266-03-kidserie-fa92cc8ae8a2 //앱인벤터를 이용한 ESP8266연결 개념설계 참고자료2

https://rasino.tistory.com/300 //앱인벤터를 이용한 ESP8266연결 개념설계 참고자료3

https://www.youtube.com/watch?v=_Ugw-8EPOa8 //앱인벤터를 이용한 ESP8266연결 개념설계 참고자료4

https://devicemart.blogspot.com/2019/05/4-iot-wifi-app-inventor-3.html //앱인벤터를 이용한 ESP8266연결 개념설계 참고자료5

https://kusson.tistory.com/400 //웹 인벤터 루틴기능 시간선택버튼 개념설계 참고자료
 
https://www.hackster.io/RoboticaDIY/iot-based-digital-world-clock-using-esp8266-b5f3bd //ESP8266서버 UTC 데이터 추출 방법
