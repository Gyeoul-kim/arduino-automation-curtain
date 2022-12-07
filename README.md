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
- ~서버의 시간을 받아와 특정시간에 기기가 동작하는 코드 개발(루틴기능)~
- 현재시간을 설정하고 특정시간에 기기가 동작하는 코드 개발(루틴기능)

## 개발 과정
-


## 동작 예시 사진(우리가 만들고 싶은것) / working pictures(what we want to do)
![GIF](./Docs/curtains-opening.gif)
> 출처 image by Geniusness / https://github.com/Geniusness/Genius-AutoCurtains

![image](./Docs/Genius-AutoCurtain-BeltandTensioner.jpg)
> 출처 image by Geniusness / https://github.com/Geniusness/Genius-AutoCurtains

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
