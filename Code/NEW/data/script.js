var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
window.addEventListener('load', onload);
var direction;
var step=1;
var speed=1;

function onload(event) {
    initWebSocket();
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection…');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');
}

function onClose(event) {
    console.log('Connection closed');
    document.getElementById("motor-state").innerHTML = "커튼 멈춤"
    setTimeout(initWebSocket, 2000);
}

function submitForm(){
    const rbs = document.querySelectorAll('input[name="direction"]');
    direction;
    for (const rb of rbs) {
        if (rb.checked) {
            direction = rb.value;
            break;
        }
    }


    document.getElementById("motor-state").innerHTML = "커튼이 움직이고 있습니다...";
    document.getElementById("motor-state").style.color = "blue";
    if (direction=="OPN"){
        document.getElementById("gear").classList.add("spin");
    }
    else{
        document.getElementById("gear").classList.add("spin-back");
    }
    websocket.send(speed+"&"+direction);

}

function closeForm(){
    direction = "stop";
    document.getElementById("motor-state").innerHTML = "커튼 멈춤"
    document.getElementById("motor-state").style.color = "red";
    document.getElementById("gear").classList.remove("spin", "spin-back");
    document.getElementById("gear").classList.remove("spin");
    websocket.send(0+"&"+direction);
}

function speed1(){
    speed=4;
}
function speed2(){
    speed=2;
}
function speed3(){
    speed=3;
}
function speed4(){
    speed=1;
}
function onMessage(event) {
    console.log(event.data);
    
    direction = event.data;
    if (direction=="stop"){ 
      document.getElementById("motor-state").innerHTML = "커튼 멈춤"
      document.getElementById("motor-state").style.color = "red";
      document.getElementById("gear").classList.remove("spin", "spin-back");
  }
  else if(direction=="OPN" || direction=="CLOSE"){
    document.getElementById("motor-state").innerHTML = "커튼이 움직이고 있습니다...";
    document.getElementById("motor-state").style.color = "blue";
    if (direction=="OPN"){
        document.getElementById("gear").classList.add("spin");
    }
    else{
        document.getElementById("gear").classList.add("spin-back");
    }
    
}

}