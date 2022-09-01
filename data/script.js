/* ESP32 WEBSOCKET jAVASCRIPT FILE
/   -----------------------
/ © 2022 Olayide AJAYI 
/--------------------------
source: https://canvas-gauges.com/documentation/user-guide/using-as-component
*/
var gateway = `ws://${window.location.hostname}/ws`;
var websocket;


// ----------------------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------------------

window.addEventListener('load', onLoad);

function onLoad(event) {
    initWebSocket();
}

// ----------------------------------------------------------------------------
// WebSocket handling
// ----------------------------------------------------------------------------

function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
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
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
   
    var myObj = JSON.parse(event.data);
   
    gaugeTemp.value = myObj.temp;
    gaugeHum.value = myObj.hum;
    gaugeLdr.value = myObj.ldr;
    document.getElementById("led1").className = myObj.led_01 == "on" ? "led on" : "led";
    document.getElementById("led2").className = myObj.led_02 == "on" ? "led on" : "led";
    document.getElementById("led3").className = myObj.led_03 == "on" ? "led on" : "led";
    document.getElementById("led4").className = myObj.led_04 == "on" ? "led on" : "led";


}


//--------------------------------------------------------------------------
// sensor info : sensorID_value/action e.g 1_on, 2_50, 10_toggle
//--------------------------------------------------------------------------

function onToggle(element) {    
       var  ledNumber = element.id.charAt(element.id.length - 1);
        console.log(ledNumber + '_' + "toggle");
        websocket.send(ledNumber + '_' + "toggle");
}


// Create Temperature Gauge
var gaugeTemp = new LinearGauge({
    renderTo: 'gauge-temperature',
    width: 120,
    height: 400,
    units: "Temperature C",
    minValue: 0,
    startAngle: 90,
    ticksAngle: 180,
    maxValue: 40,
    colorValueBoxRect: "#049faa",
    colorValueBoxRectEnd: "#049faa",
    colorValueBoxBackground: "#f1fbfc",
    valueDec: 2,
    valueInt: 2,
    majorTicks: [
        "0",
        "5",
        "10",
        "15",
        "20",
        "25",
        "30",
        "35",
        "40"
    ],
    minorTicks: 4,
    strokeTicks: true,
    highlights: [
        {
            "from": 30,
            "to": 40,
            "color": "rgba(200, 50, 50, .75)"
        }
    ],
    colorPlate: "#fff",
    colorBarProgress: "#CC2936",
    colorBarProgressEnd: "#049faa",
    borderShadowWidth: 0,
    borders: false,
    needleType: "arrow",
    needleWidth: 2,
    needleCircleSize: 7,
    needleCircleOuter: true,
    needleCircleInner: false,
    animationDuration: 1500,
    animationRule: "linear",
    barWidth: 10,
}).draw();

// Create Humidity Gauge
var gaugeHum = new RadialGauge({
    renderTo: 'gauge-humidity',
    width: 300,
    height: 300,
    units: "Humidity (%)",
    minValue: 0,
    maxValue: 100,
    colorValueBoxRect: "#049faa",
    colorValueBoxRectEnd: "#049faa",
    colorValueBoxBackground: "#f1fbfc",
    valueInt: 2,
    majorTicks: [
        "0",
        "20",
        "40",
        "60",
        "80",
        "100"

    ],
    minorTicks: 4,
    strokeTicks: true,
    highlights: [
        {
            "from": 80,
            "to": 100,
            "color": "#03C0C1"
        }
    ],
    colorPlate: "#fff",
    borderShadowWidth: 0,
    borders: false,
    needleType: "line",
    colorNeedle: "#007F80",
    colorNeedleEnd: "#007F80",
    needleWidth: 2,
    needleCircleSize: 3,
    colorNeedleCircleOuter: "#007F80",
    needleCircleOuter: true,
    needleCircleInner: false,
    animationDuration: 1500,
    animationRule: "linear"
}).draw();

//LDR gauge
var gaugeLdr = new LinearGauge({
    renderTo: 'gauge-ldr',
    width: 400,
    height: 80,
    minValue: 0,
    maxValue: 100,
    majorTicks: [
        "0",
        "20",
        "40",
        "60",
        "80",
        "100"
    ],
    minorTicks: 10,
    strokeTicks: true,
    colorPlate: "#ccc",
    borderShadowWidth: 0,
    borders: false,
    barBeginCircle: false,
    tickSide: "left",
    numberSide: "left",
    needleSide: "left",
    needleType: "line",
    needleWidth: 3,
    colorNeedle: "#007F80",
    colorNeedleEnd: "#007F80",
    animationDuration: 1500,
    animationRule: "linear",
    animationTarget: "plate",
    barWidth: 5,
    ticksWidth: 50,
    ticksWidthMinor: 15,
    units: 'Brightness %',
    colorValueBoxRect: "#049faa",
    colorValueBoxRectEnd: "#049faa",
    colorValueBoxBackground: "#f1fbfc",
    valueDec: 2,
    valueInt: 2

}).draw();





