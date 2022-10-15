//ESP library 가져오기
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "와이파이 SSID 입력";
const char* password = "비밀번호";
const int led = 13;

ESP8266WebServer server(80);





void handleRoot() {//200(정상)때 출력할 함수
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp8266!");
  digitalWrite(led, 0);
}


void handleNotFound(){//404(못 찾을시)에러때 출력할 함수
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void){//Arduino 셋업
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.begin(ssid, password);//미리 받은 ssid와 password를 이용하여 와이파이에 연결
  Serial.println("");//한줄 띄우기
  // 연결 기다릴때 출력할 코드(와이파이 연결할때까지 0.5초 기다리고 . 찍기)
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");//한줄 띄우기
 //Connected To '미리 입력받은 ssid' 출력
  Serial.print("연결됨 : ");
  Serial.println(ssid);
 //아이피 주소 출력
  Serial.print("아이피 주소: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
      Serial.println("MDNS 수신기 시작");
    }
    //서버 설정
  server.on("/", handleRoot);// 루트로 접근 요청 받을 시 handleRoot 함수 내용 실행
  server.on("/inline", [](){// inline으로 접근 요청 받을 시 아래의 내용 실행
      server.send(200, "application/json", "{test: test}");
    });
  server.onNotFound(handleNotFound);//404(못찾으면)handleNotFound 실행
  server.begin();//서버 실행
    Serial.println("HTTP 서버 시작됨");
  }

void loop(void){
  server.handleClient();//서버 요청 계속 받기
}