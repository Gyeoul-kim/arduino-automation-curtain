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
  >루틴 기능 아두이노 코드 개발   
  >Wi-Fi통신 코드 개발   
  >동작 속도 단계 조절 코드 개발   
  >동작 정지 기능 코드 개발

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
- ~서버의 시간을 받아와~ 현재시간을 설정하고 특정시간에 기기가 동작하는 코드 개발(루틴기능)
   
## 동작 예시 사진(우리가 만들고 싶은것) / working pictures(what we want to do)   
![GIF](./Docs/curtains-opening.gif)   
> 출처 image by Geniusness / https://github.com/Geniusness/Genius-AutoCurtains   
   
![image](./Docs/Genius-AutoCurtain-BeltandTensioner.jpg)   
> 출처 image by Geniusness / https://github.com/Geniusness/Genius-AutoCurtains   
   
## 아두이노 코드 개발
-
-
### 앱 개발
사진 순서는 앱 사용시 동작순서와 같도록 배치하였습니다.   
    
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
![루틴설정화면](https://user-images.githubusercontent.com/117341089/206374695-e0f10afa-f693-4a4d-996d-41b90f0c5832.PNG)
![루틴시간설정툴팁](https://user-images.githubusercontent.com/117341089/206374700-0ed06940-43e1-4aff-9c66-bfdcff3d617d.PNG)   
이후 전역변수에 설정한 시간값을 저장합니다. IF문을 통해 시간은 5분 단위로만 설정할 수 있도록 재한하였습니다.   
5분 단위가 아닐경우 5분으로 설정하도록 툴팁을 만들었습니다.   
전역변수를 성공적으로 설정하면 서버는 텍스트 맨뒤의 1값으 먼저 읽고 앞에 TIM 텍스트와 뒤에 나머지 4개의 전역변수 값을 읽고 서버에 저장합니다.   
이후 서버에 저장되어있는 현재시간 값과 루틴설정으로 만든 시간값이 일치하면 열기/닫기 동작을 수행합니다.   
   
## 어려웠던 부분 / 아쉬웠던 부분  
ESP8266이 다루기 굉장히 까다로운 부품이어서 코드를 작성할 수록 오류가 늘어났습니다.    
분명 논리적으로는 오류가 없는 코드이지만 ESP8266에서 지원하는 기능들이 많아 오히려 역으로 기능들이 서로 충돌하면서 코드를 짜는데 굉장히 어려움이 많았습니다.   
특히 루틴기능을 만들때 시간을 굉장히 많이 빼앗기게 되었습니다.    
루틴 기능 구현을 위해서 코드안에서 돌아갈 시간데이터가 있고 사용자가 원하는 시간에 기기를 동작하도록 개념설개를 하였었습니다.   
이 시간을 서버에서 돌아가고 있는 시계를 NTP(Network Time Protocol)로 시간데이를 가져와서 코드에 활용하려 하였으나 NTP라이브러리가 기존코드와 계속 충돌하여 중간에 코드가 계속 뻣었고 이 때문에 2주 이상의 시간을 허비했습니다.   
계속 서버에 시간을 가져오려고 노력하였으나 계속 코드가 터지는 바람에 결국 앱에서 사용자가 처음 사용할 때 현제시간을 입력하여 보내는 방식으로 해결하였습니다.   
루틴 설계에서 많은 시간을 쓰다보니 속도조절 값과 진행상황을 보여주는 디스플레이를 만들고 싶었지만 시간이 부족하여 시도하지 못하여 아쉬움이 남습니다.   
   
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
