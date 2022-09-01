#include <DHT.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

//--------------------------------------------------------------------------
// define macros, GPIO and sensor pins
//---------------------------------------------------------------------------
#define LED_01_PIN 23
#define LED_02_PIN 22
#define LED_03_PIN 21
#define LED_04_PIN 19
#define LDR_PIN A3
#define DHT_PIN 1
#define DHT_TYPE DHT11

// ----------------------------------------------------------------------------
// Definition of the LED component
// ----------------------------------------------------------------------------

struct Led
{
    // state variables
    uint8_t pin;
    bool on;
    // methods
    void update()
    {
        digitalWrite(pin, on ? HIGH : LOW);
    }
};

//----------------------------------------------------------------------------
// Define network credentials with WiFi details
//----------------------------------------------------------------------------
const char *WIFI_SSID = "your_wifi_ssid";
const char *WIFI_PASS = "your_wifi_password";

//---------------------------------------------------------------------------
// Define global variables
//---------------------------------------------------------------------------

// DHT Sensors
DHT dht(DHT_PIN, DHT_TYPE);
float temp = 0.00;
float hum = 0.00;

button[4] = [0,0,0,0];

//

// LEDs
Led onboard_led = {2, false};
Led led_1 = {LED_01_PIN, false};
Led led_2 = {LED_02_PIN, false};
Led led_3 = {LED_03_PIN, false};
Led led_4 = {LED_04_PIN, false};

//---------------------------------------------------------------------------
// Define global functions =
//---------------------------------------------------------------------------

// Update DHT Readings
void updateDHTReadings()
{
   // delay(100);

    float t = float(dht.readTemperature());
    float h = float(dht.readHumidity());

    if (!(isnan(t)))
    {
        temp = t;
    }
    else
    {
        Serial.println("Failed to read Temperature from DHT12 sensor!");
    }

    if (!isnan(h))
    {
        hum = h;
    }
    else
    {
        Serial.println("Failed to read from DHT12 sensor!");
    }
}

//---------------------------------------------------------------------------
//  Create AsyncWebServer object at PORT 80
//---------------------------------------------------------------------------
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void notFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Not found");
}

//-----------------------------------------------------------------------------
// SPIFFS initialization - to load webpage files
//-----------------------------------------------------------------------------
void initSPIFFS()
{
    if (!SPIFFS.begin())
    {
        Serial.println("Cannot mount SPIFFS volume...");
        while (1)
        {
            onboard_led.on = millis() % 200 < 50;
            onboard_led.update();
        }
    }
}

// ----------------------------------------------------------------------------
// Connecting to the WiFi network
// ----------------------------------------------------------------------------

void initWiFi()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.printf("Trying to connect [%s] ", WiFi.macAddress().c_str());
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }
    Serial.printf(" %s\n", WiFi.localIP().toString().c_str());
}

// ----------------------------------------------------------------------------
// Web server initialization
// ----------------------------------------------------------------------------
String processor(const String &var)
{
    if (var == "STATE")
        return F("State!");
    return String();
}

void onRootRequest(AsyncWebServerRequest *request)
{
    request->send(SPIFFS, "/index.html", "text/html", false, processor);
}

void initWebServer()
{
    server.on("/", onRootRequest);
    server.serveStatic("/", SPIFFS, "/");
    server.begin();
}

// ----------------------------------------------------------------------------
// Web server initialization
// ----------------------------------------------------------------------------

// Callback for incoming event
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
        Serial.printf("Client connected: \n\tClient id:%u\n\tClient IP:%s\n",
                      client->id(), client->remoteIP().toString().c_str());
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("Client disconnected:\n\tClient id:%u\n", client->id());
        break;
    case WS_EVT_DATA:
        handlingIncomingData(arg, data, len);
        break;
    case WS_EVT_PONG:
        Serial.printf("Pong:\n\tClient id:%u\n", client->id());
        break;
    case WS_EVT_ERROR:
        Serial.printf("Error:\n\tClient id:%u\n", client->id());
        break;
    }
}

// ----------------------------------------------------------------------------
// WebSocket initialization
// -----------------------------------------------------------------------------

// inform client of updates
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

// void handlingIncomingData

void handlingIncomingData(void *arg, uint8_t *data, size_t len)
{
    AwsFrameInfo *info = (AwsFrameInfo *)arg;

    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
        data[len] = 0;
        String message = (char *)data;

        if (message.indexOf("1_") >= 0 && message.substring(2) == "toggle")
        {
            led_1.on = !led_1.on;
            led_1.update();
            notifyClients();
        }
        if (message.indexOf("2_") >= 0 && message.substring(2) == "toggle")
        {
            led_2.on = !led_2.on;
            led_2.update();
            notifyClients();
        }
        if (message.indexOf("3_") >= 0 && message.substring(2) == "toggle")
        {
            led_3.on = !led_3.on;
            led_3.update();
            notifyClients();
        }
        if (message.indexOf("4_") >= 0 && message.substring(2) == "toggle")
        {
            led_4.on = !led_4.on;
            led_4.update();
            notifyClients();
        }
    }
}

void initWebSocket()
{
    ws.onEvent(onEvent);
    server.addHandler(&ws);
}

void setup()
{
    pinMode(onboard_led.pin, OUTPUT);
    pinMode(LED_01_PIN, OUTPUT);
    pinMode(LED_02_PIN, OUTPUT);
    pinMode(LED_03_PIN, OUTPUT);
    pinMode(LED_04_PIN, OUTPUT);
    pinMode(LDR_PIN, INPUT);

    dht.begin();

    Serial.begin(115200);
    delay(500);

    initSPIFFS();
    initWiFi();
    initWebSocket();
    initWebServer();
}

void loop()
{
    onboard_led.on = millis() % 1000 < 50;
    delay(500);
    updateDHTReadings();
    ws.cleanupClients();
    onboard_led.update();
    notifyClients();
}
