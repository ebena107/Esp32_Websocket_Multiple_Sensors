# Esp32_Websocket
 An IOT System using Esp32 and WebSockets to demonstrate reading and sending data from multiple sources to ESP32 via websocket.

![](/src/Screenshot2.png)

## plugins and Library Used
- ![ESPAsyncWebServer]()
- ![AsyncTCP]()
- ![SPIFFS]()
- ![ArduinoJson](https://arduinojson.org/v6/doc/)
- ![DHT]

```
#include <DHT.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
```

## GAUGES API
  ![canvas gauges](https://canvas-gauges.com/documentation/user-guide/) is a powerful api for drawing various custom gauges, linear or radial, for iot projects.



## PIN Connections
![](/src/wiring_diagram.png)

 * Device 1 (DHT 11) => GPIO 1 
 * Device 2 (LDR)    => GPIO 39 =>  A3
 * Device 3 (LED)    => GPIO 19
 * Device 4 (LED)    => GPIO 21
 * Device 5 (LED)    => GPIO 22
 * Device 6 (LED)    => GPIO 23

 ![](/src/esp32_pinout.png)

 ## Websocket Incoming Data to ESP
 format {SensorId _ action/data, e.g L01_on, 3_stop}
 ```
    function onToggle(element) {    
       var  ledNumber = element.id.charAt(element.id.length - 1);
        console.log(ledNumber + '_' + "toggle");
        websocket.send(ledNumber + '_' + "toggle");
}
 ```

 ## Websocket outgoing Data to webpage
``` 
void notifyClients()
{
    const int size = JSON_OBJECT_SIZE(8);
    StaticJsonDocument<size> json;
    
    json["led_01"] .set( led_1.on ? "on" : "off");
    json["led_02"] .set( led_2.on ? "on" : "off");
    json["led_03"] .set( led_3.on ? "on" : "off");
    json["led_04"] .set( led_4.on ? "on" : "off");
    json["ldr"] .set( String(analogRead(LDR_PIN)*100/4049));
    json["temp"] .set( String(temp));
    json["hum"] .set( String(hum));

    String data;
    serializeJson(json, data);
    ws.textAll(data);
}

```

