//ESP library 가져오기
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "Team31";
const char* password = "team31wifi";
const int led = 13;
const int motorPin1 = 4;
const int motorPin2 = 5;
const int motorPin3 = 6;
int status;
ESP8266WebServer server(80);





void handleRoot() {  //200(정상)때 출력할 함수,추 후 코드작성시 여기가 메인
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head>");
  client.println("<meta charset=" utf - 8 ">");
  client.println("<meta name=" viewport " content=" width = device - width, initial - scale = 1 ">");
  client.println("<style type=" text / css ">");
  client.println("*{");
  client.println("font-family: "맑은 고딕 "," Apple SD Gothic NEO "," Noto Sans CJK ";");
  client.println("}");
  client.println("table,th{");
  client.println("border: 2px black solid; border-collapse: collapse; width: 100%; height: 100%; margin: auto;");
  client.println("}");
  client.println("td{");
  client.println("border: 2px black solid; border-collapse: collapse;");
  client.println("}");
  client.println("body{");
  client.println("");
  client.println("}");
  client.println(".banner{");
  client.println("text-align: center; padding-top: 10%; padding-bottom: 10%; width: 100%;");
  client.println("}");
  client.println(".content{");
  client.println("text-align: center; padding-top: 5%; padding-bottom: 5%;");
  client.println("}");
  client.println(".status{");
  client.println("padding-top: 5%; padding-bottom: 5%; padding-left: 2%; padding-right: 2%;");
  client.println("}");
  client.println(".button{");
  client.println("font-size:187.5% ;");
  client.println("}");
  client.println("</style>");
  client.println("<title>Arduino SMART HOME</title>");
  client.println("</head>");
  client.println("<body>");
  client.println("<table>");
  client.println("<tr>");
  client.println("<td class=" banner " colspan=" 3 "><h1>Arduino SmartHome Control Web</h1></td>");
  client.println("</tr>");

  //status 조작 파트
  client.println("<tr>");
  if (status == 1) {
    client.println("<td class=" status " colspan=" 3 "><h2>Status : ON</h2> </td>");
  } else {
    client.println("<td class=" status " colspan=" 3 "><h2>Status : OFF</h2> </td>");
  }
  client.println("<td class=" status " colspan=" 2 "><h2>Status :</h2> </td>");
  client.println("</tr>");
  client.println("<tr>");
  //Status Control Part END

  //Control Button
  client.println("<td class=" content "><input class=" button " type=" button " name=" left " value=" Left " onclick=" location.href = './gpio/1' "></td>");
  client.println("</tr>");
  client.println("<tr>");
  client.println("<td class=" content "><input class=" button " type=" button " name=" right " value=" Right " onclick=" location.href = './gpio/2' "></td>");
  client.println("</tr>");
  client.println("<tr>");
  client.println("<td class=" content "><input class=" button " type=" button " name=" Stop " value=" Stop " onclick=" location.href = './gpio/3' "></td>");
  client.println("</tr>");
  //Control Button END

  //
  client.println("<tr>");
  client.println("<td class=" content " colspan=3 >루틴(Routine)</td>");
  client.println("</tr>");
  client.println("<tr>");
  client.println("<td style=" text = align
                 : center "><b>루틴(Routine)<b/></td>");
  client.println("</tr>");
  client.println("1");
  client.println("2");
  client.println("3");
  client.println("4");
  //server.send(200, "text/plain", "hello from esp8266!");//OK 요청,hello from esp8266 브라우저에 출력
}
void CurtainLeft() {
  status = 1;
  digitalWrite(motorPin1,HIGH);
  digitalWrite(motorPin2,LOW);
}

void CurtainRight() {
  status = 1;
  digitalWrite(motorPin1,LOW);
  digitalWrite(motorPin2,HIGH);
}

void CurtainSTOP() {
  digitalWrite(motorPin1,LOW);
  digitalWrite(motorPin2,LOW);
  status = 0;
}


void handleNotFound() {  //404(못 찾을시)에러때 출력할 함수
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {  //Arduino 셋업
  pinMode(led, OUTPUT);
  pinMode(motorPin1,OUTPUT);
  pinMode(motorPin2,OUTPUT);
  pinmode(motorPin3,OUTPUT);
  Serial.begin(115200);
  WiFi.begin(ssid, password);  //미리 받은 ssid와 password를 이용하여 와이파이에 연결
  Serial.println("");          //한줄 띄우기
  // 연결 기다릴때 출력할 코드(와이파이 연결할때까지 0.5초 기다리고 . 찍기)
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");  //한줄 띄우기
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
  server.on("/", handleRoot);  // 루트로 접근 요청 받을 시 handleRoot 함수 내용 실행
  server.on("/gpio/1", CurtainLeft);
  server.on("/gpio/2", CurtainRight);
  server.on("/gpio/3", CurtainSTOP);
  server.onNotFound(handleNotFound);  //404(못찾으면)handleNotFound 실행
  server.begin();                     //서버 실행
  Serial.println("HTTP 서버 시작됨. 아래의 링크로 접속해 주세요.");
  Serial.print(Wifi.localIP());
  Serial.println(":80");
}

void loop(void) {
  server.handleClient();  //서버 요청 계속 받기
}

//1.스마트
//2.