# 코드 설명

    #include <ESPAsyncTCP.h>
    #include <Arduino.h>//아두이노 명령셋
    #include <ESP8266WiFi.h>//와이파이 기능용
    #include <ESPAsyncWebServer.h>//웹서버 열기 위해
    #include <ESP8266mDNS.h>
    #include "LittleFS.h"//파일시스템 사용 위해
    #include <AccelStepper.h>//스테퍼 라이브러리.
    #include <GDBStub.h> //for Debug.
코드를 위해 Library 파일들을 가져오게 됩니다.

    //define Motor driver's pin
    #define IN1 5 
    #define IN2 4
    #define IN3 14
    #define IN4 12

모터 드라이버의 핀을 정의하여줍니다.
	

    char *substr(int s, int e, char *str); //배열용 substring for c ref by codingdog.
    char *message = "";//앱으로부터 받아오는 정보를 저장할 문자열입니다.
    const char* ssid = "각자_쓰는_와이파이_이름";//상수로 SSID 지정.
    const char* password = "비밀번호";//상수로 PASSWORD 기록(보안에는 안좋음),공개형 와이파이인 경우 공란으로 둡니다.
    AccelStepper stepper(AccelStepper::HALF4WIRE, IN1, IN3, IN2, IN4);//스테퍼모터의 타입, 와이어를 지정합니다.
    AsyncWebServer server(80);//웹서버 오브젝트를 80 포트를 통해 받도록 지정합니다. 80포트는 http 프로토콜의 기본 포트입니다.
    AsyncWebSocket ws("/ws");//웹소켓 오브젝트 생성하여 웹소켓 패킷은 해당 주소로만 받도록 지정합니다.
    char *Direction = "STP"; //기본 상태를 STP 즉, 정지 상태로 지정해둡니다.
    int steps = 0; //스텝(회전)을 0으로 초기화 합니다.
    char *Speed;//Websocket 방식으로 받은 속도 저장하기 위해 생성합니다.
    int R_speed;//int 형 속도 변수.
    bool notifyStop = false;//정지 알림 Default로 false 설정
    /*
    char *SRO_Hour = ""; char *SRO_Min = "";  char *SRC_Hour = ""; char *SRC_Min = ""; //스트링 형태로 열고 닫는 시간을 저장합니다.(R=Remote,앱에서 지정한 시간)
    int RO_Hour; int RO_Min; int RC_Hour; int RC_min;//위의 스트링 형태로 받은 시간을 atoi 함수를 통해 정수형으로 변환한 뒤 저장하기 위해 쓰이는 함수입니다.
    */
    char *isTrigger = ""; int isTriggerI; //int inRoutine;루틴 상태인지,어떤 모드를 요청하였는지 구분하기 위해 쓰이는 변수입니다.
    char *SC_Hour = ""; char *SC_Min = ""; char *SC_Sec; char *C_Time = ""; String SC_Time = ""; 		
    //기기 시간 String형태로 받아오기 위해 쓰이는 변수입니다. 
    int C_Hour=0; int C_Min=0; int C_Sec=0;//받아온 시간을 int 형태로 변환하여 저장하기 위해 쓰이는 변수입니다.
    
   코드 내에서 사용할 변수들을 선언해 줍니다.
   
    unsigned long currentMillis;
    unsigned long previousMillis;
    const long interval = 1000;
 delay()를 사용할 때 생기는 문제점을 해결하기 위해, millis()를 이용하기 위해 변수를 선언해 줍니다.
    
    char *substr(int star, int en, char *str) { //Substring 기능 구현
    
      char *niew = (char *)malloc(sizeof(char) * (en - star + 2)); //niew 차형 포인터 선언해서 en-star+2한 만큼 동적으로 지정
      strncpy(niew, str + star, en - star + 1); //입력받은 스트링(niew)에서 str+star부터 en-star+1만큼 복사
      niew[en - star + 1] = 0; //strncpy는 \0을 가져오지 않음으로 마지막에 추가해 줍니다. niew 마지막에 널문자 추가.
      return niew;//niew 값 돌려보내기.
    }
아두이노 기본에 들어있는 substring 함수는 string 형으로 선언된 변수에만 사용이 가능하여, char 포인터 형에도 사용이 가능한 substring 함수를 구현하였습니다.

    void Clocker(){//시간 측정.
    

      currentMillis=millis();//값을 millis로 지정.
    
      if(currentMillis-previousMillis >=interval){//만약 현재 밀리초값-이전 밀리초값이 1000보다 크다면
        previousMillis=currentMillis;//이전 밀리초값을 현재 밀리초값으로 지정
        C_Sec=C_Sec+1;//초에 초+1
        if(C_Sec==60){//만약 초가 60이라면
          C_Sec=0;//초를 0으로
          C_Min=C_Min+1;//분을 분+1로
        }
        if(C_Min==60){//만약 분이 60이라면
          C_Min=0;//분을 0으로
          C_Hour=C_Hour+1;//시를 시+1로
        }if(C_Hour==24){//만약 시가 24라면
          C_Hour=0;//시를 0으로
        }
      }
    }
 

   안에서 구동하는 시계 함수입니다. 지금의 밀리초와 과거의 밀리초 차가 1000ms가 되면 함수는 작동합니다.
   delay를 사용하는 경우 해당 함수를 실행하기 위해 모든 프로세스가 일시정지를 함으로 작동하지 않은 문제가 있어 millis 함수를 이용하여 제작하였습니다.
   

     void initFS() {//LittleFS 파일 시스템 사용(ESP만의 기능. 내부 저장 공간 일부를 파일 저장하는데 사용 가능.)
          if (!LittleFS.begin()) {//LittleFS가 시작되지 않는다면
            Serial.println("An error has occurred while mounting LittleFS");//메시지 출력
          }
          else {
            Serial.println("LittleFS mounted successfully");//메시지 출력
          }
        }
  

ESP8266의 기능 중 하나인 FS 시스템을 사용하기 위한 함수입니다. 내부 저장공간의 일부를 파일 저장을 위한 공간으로 사용이 가능하여 여러가지 시스템을 구현할 수 있습니다.
   

     void initWiFi() {// Initialize WiFi
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
    

와이파이 연결 하는 함수입니다. 와이파이를 연결한 뒤 MDNS 서비스를 시작합니다. MDNS 서비스를 사용하면 IP를 몰라도 접속이 가능하지만 현재는 애플사의 기기만 온전히 지원하며, 타사의 기기에서는 지원이 안되거나 반쪽으로 지원하는 단점이 있습니다.

 
        void notifyClients(char *state) {//클라이언트에 신호 전송
          ws.textAll(state);//받은거 그대로 웹소켓 통해 클라이언트로 전송.
        }
  

클라이언트(앱)에 웹소켓 프로토콜을 통해 현재 상태를 전송하는 코드입니다.
   

     void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {//웹소켓 메시지 해독
          AwsFrameInfo *info = (AwsFrameInfo*)arg;
          if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
            data[len] = 0;//형식은 /DOCS내 이미지 참고.
            message = (char*)data;//웹소켓 메시지를 캐릭터 포인터형으로 변환하여 message 문자열에 넣기
            isTrigger = substr(18, 18, message); //맨 마지막 숫자를 isTrigger 에 저장.
            isTriggerI = atoi(isTrigger); //if문 통한 비교를 위해 int형으로 변환
            Serial.print("value :");
            Serial.println(isTriggerI);//모드 값 출력합니다.
            if(isTriggerI==0){//만약 모드가 0이면
              Speed = substr(0, 0, message); //message에서 & 앞의 문자 추출
              int Speed_i=atoi(Speed);//Speed 값을 정수형으로 변환하여 Speed_i에 저장합니다.
              Direction = substr(2, 4, message); //& 뒤의 문자를 추출.
              Serial.println(message);//표시
              Serial.println(Speed);//표시
              Serial.println(Direction);//표시
              steps = 20480;//스텝을 20480으로 지정합니다.
              switch (Speed_i) {//받아온 속도값을 스위치에 넣습니다.
                case(1)://1이라면
                R_speed = 500;//회전 속도를 500으로 지정합니다.
                break;//나가기
                case(2)://2라면
                R_speed = 750;//회전 속도를 750으로 설정합니다.
                break;//나가기
                case(3)://3이라면
                R_speed = 1000;//회전 속도를 1000으로 설정합니다.
                break;//나가기
              }
              Serial.println(R_speed);//R_Speed를 출력합니다.
              if (strcmp(Direction, "OPN") == 0) { //입력받은 string 이 OPN이면
                notifyClients(Direction);
                ESP.wdtDisable();//와치독을 잠시 꺼줍니다.
                stepper.move(-steps);//스텝수만큼 회전.
                Serial.print("Opened");//열었다고 시리얼에 표시
                //delay(5000);//5초 대기
                ESP.wdtEnable(5600);//작동 후 다시 켜줌.
                //Direction="STP";
                notifyClients(Direction);
              } else if (strcmp(Direction, "CLS") == 0) { //입력받은 string이 CLS라면
                notifyClients(Direction);
                ESP.wdtDisable();//와치독 임시 비활성화.
                stepper.move(10240);//10240스탭만큼 회전.
                Serial.print("CLOSE");//닫았다고 표시.
                //delay(5000);//5초 대기.
                ESP.wdtEnable(5600);//다시 와치독 활성화.
                //Direction="STP";
                notifyClients(Direction);
              } else { //입력받은 string이 STP라면
                notifyClients(Direction);
                ESP.wdtDisable();//와치독 비활성화
                stepper.stop();//모터 정지(거의 바로 정지함.)
                Serial.print("STOP");//시리얼에 정지했다고 표시.
                //delay(5000);//5초 대기
                ESP.wdtEnable(5600);//다시 와치독 킴.
                Direction="STP";
                notifyClients(Direction);
              }
        
            }else if(isTriggerI==1){
              SC_Hour=substr(6,7,message);//메세지의 6번째부터 7번째까지의 패킷을 추출합니다.
              SC_Min=substr(9,10,message);//메세지의 9번째부터 10번째까지의 패킷을 추출합니다.
              SC_Sec="0";//초를 0으로 지정합니다.
              C_Hour=atoi(SC_Hour);//받아온 시를 정수형으로 바꿔 저장합니다.
              C_Min=atoi(SC_Min);//받아온 분을 정수형으로 바꿔 저장합니다.
              C_Sec=atoi(SC_Sec);//받아온 초를 정수형으로 바꿔 저장합니다.
              Serial.print(SC_Hour);//시리얼에 표시합니다.
              Serial.print(SC_Min);//시리얼에 표시합니다.
              Serial.print(C_Sec);//시리얼에 표시합니다.
            }else{
            }
          }
        }
    

웹소켓 패킷을 받아 분석하여 그에 맞는 동작을 하는 코드입니다.

    void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {//웹소켓 패킷을 처리하기 위한 코드입니다.
      switch (type) {//받아온 타입에 따라 처리하는 코드입니다.
        case WS_EVT_CONNECT://웹소켓 연결 상태라면
          Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());//시리얼에 출력(id,아이피,원격 아이피)
          notifyClients(Direction);//방향 클라이언트에 전송
          break;//나가기
        case WS_EVT_DISCONNECT://연결 끊기면
          Serial.printf("WebSocket client #%u disconnected\n", client->id());//끊겼다고 시리얼에 출력
          break;//나가기
        case WS_EVT_DATA://데이터 들어오면
          handleWebSocketMessage(arg, data, len);//함수에 던지기
          break;//나가기
        case WS_EVT_PONG://PING 입력을 받은 상태일때 실행할 케이스이지만, 핑 입력을 보낼 일이 없어 제외했습니다.
        case WS_EVT_ERROR:
          break;
      }
    }
클라이언트로부터 받아온 웹소켓 패킷을 분석하고 처리하기 위한 코드입니다.

    void initWebSocket() {//웹소켓 세팅
      ws.onEvent(onEvent);//온이벤트 함수를 호출합니다.
      server.addHandler(&ws);//헨들러를 추가합니다.
    }
웹소켓을 시작하는 코드입니다.

    void setup() {
      // Serial port for debugging purposes
    
      Serial.begin(115200);//시리얼 활성화
      initWiFi();//와이파이를 활성화합니다.
      initWebSocket();//웹소켓을 활성화합니다.
      initFS();//FS를 활성화합니다.
      /*
      server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) { //루트로 요청하면
        request->send(LittleFS, "/index.html", "text/html");//여기 파일 보여주기
      });//웹 클라이언트 개발의 흔적입니다,
      */
      server.serveStatic("/", LittleFS, "/");//고정 상태로 지정합니다.
      server.begin();//서버를 시작합니다.
      stepper.setAcceleration(1000);//가감속력 설정합니다. 1000의 경우 즉각적인 정지에 가깝습니다.
      //gdbstub_init();gdb 디버깅 용도입니다.
    }
ESP8266이 시작할때 작동하는 코드입니다.

    void loop() {
      if (notifyStop == true) { //만약 스탑이 트루면
        Direction = "STP";//방향 스탑
        notifyClients(Direction);//클라이언트에 방향 전달
        notifyStop = false;//스탑 폴스로
      }
      //Clocker();루틴 기능 개발의 흔적입니다.
      //TimeRoutine();루틴 기능 개발의 흔적입니다.
      //ws.cleanupClients();//클라이언트를 주기적으로 클린업합니다.
      stepper.setMaxSpeed(R_speed);//속도 맥시멈 지정. 위에서 변경한대로.
      stepper.run(); //계속해서 호출해야 모터가 굴러갑니다.
    
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
   
    void TimeRoutine() { //루틴 함수.
      if ((RO_Hour==C_Hour) && (RO_Min == C_Min)) { //만약 여는시간과 시스템 시간이 일치한다면
        Direction = "OPN";//열기
        inRoutine=0;
      } else if ((RC_Hour == C_Hour) && (RC_min, C_Min)) { //아니라면 닫는 시간과 시스템 시간이 일치한다면
        Direction = "CLS";//닫기
        inRoutine=0;
      } else { //그것도 아니라면.
        if(currentMillis-previousMillis >=interval){
        previousMillis=currentMillis;
        Serial.println("");
        Serial.println("여는시간 :");
        Serial.print("T:");
        Serial.print(RO_Hour);//출력.
        Serial.println("");
        Serial.print("M:");
        Serial.print(RO_Min);
        Serial.println("");
        Serial.println("닫는시간 :");
        Serial.print("T:");
        Serial.print(RC_Hour);//출력.
        Serial.println("");
        Serial.print("M:");
        Serial.print(RC_min);
        Serial.println("");
      } 
      
      }
    }
    */
루틴의 함수입니다.
