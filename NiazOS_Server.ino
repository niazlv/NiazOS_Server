#include "NiazOS.h"

/*
 * ----------Settings----------
 */

//Включит в компоненты компиляции Модуль лампы
//#define LAMP_MODE

//Включает функции дисплея SSD1306 
//#define DISPLAY_ssd1306

#define OSVERSION 0.1



const char* ssid = "ssid";
const char* password = "pass";

const int httpsPort = 443;  //Адрес порта для HTTPS= 443 или HTTP = 80
const char fingerprint[] PROGMEM = "5B:FB:D1:D4:49:D3:0F:A9:C6:40:03:34:BA:E0:24:05:AA:D2:E2:01"; //ключ для шифрования


/*
 * ------------END-------------
 */




//Если плата ESP32
#ifdef HAL_ESP32_HAL_H_
  #define ESP32
  #include <WiFi.h>
  #include <WiFiClient.h>
  #include <WebServer.h>
  #include <ESPmDNS.h>
  
  WebServer server(80);
  #define LED 13
  int statusLED = LOW; // Начальный статус светодиода ВЫКЛЮЧЕН
#endif

//Если плата ESP8266
#ifdef __CORE_ESP8266_VERSION_H
  #define ESP8266
  #include <ESP8266WiFi.h>
  #include <WiFiClient.h>
  #include <WiFiClientSecure.h>
  #include <ESP8266WebServer.h>
  #include <ESP8266mDNS.h>
  #include <ESP8266HTTPUpdateServer.h>

  ESP8266WebServer server(80);
  #define LED 2
  int statusLED = HIGH; // Начальный статус светодиода ВЫКЛЮЧЕН
#endif



//Определим модуль дисплея
#ifdef DISPLAY_ssd1306
  #include <SPI.h>
  #include <Wire.h>
  //#include <SoftwareWire.h>
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>
  #define SCREEN_WIDTH 128 // OLED display width, in pixels
  #define SCREEN_HEIGHT 64 // OLED display height, in pixels
  uint8_t sda=12,scl=14;
  
  #define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
#endif



const long utcOffsetInSeconds = 10800;  //UTC +3 = 3 * 60 * 60

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Определение NTP-клиента для получения времени
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);


boolean __connected = false;

#ifdef ESP8266
  ESP8266HTTPUpdateServer httpUpdater;
#endif

IPAddress ip;

void setup(void)
{
  pinMode(LED, OUTPUT);
  digitalWrite(LED, 0);
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  
  #ifdef DISPLAY_ssd1306
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    }
    display.clearDisplay();
    display.setTextSize(1);             
    display.setTextColor(WHITE);        
    display.setCursor(0,0);
    display.print("Module started");
    display.display();
  #endif

  setup_wifi();
  int c = 0;
   
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    c++;
    if(c > 20)
    {
      WiFi.softAP("esp32", "12345678");
      ip = WiFi.softAPIP();
      Serial.println("\nTimeout. Created new AP. esp32");
      break;
    }
  }
  
  
  Serial.println("");
  if (c <= 20)
  {
    ip = WiFi.localIP();
    Serial.print("Connected to ");
    Serial.println(ssid);
    //если подключились к сети, то запустим NTPClient 
    timeClient.begin();
    __connected = true;
  }
  Serial.print("IP address: ");
  Serial.println(ip);


  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }
  #ifdef DISPLAY_ssd1306
    display.clearDisplay();
    display.setCursor(0,0);
    if (c <= 20)
    {
      display.print("Connected to ");
      display.println(ssid);
    }
    else
    {
      display.print("AP: esp32\npaswd: 12345678\n");
    }
    display.print("IP:");
    display.println(ip);
    
  #endif
  handlers();
  server.begin();
  
  digitalWrite(LED, statusLED);
  Serial.println("HTTP server started");

  
  //обновления по воздуху
  #ifdef ESP32
    webOTA_ESP32();
  #endif
  #ifdef ESP8266
    httpUpdater.setup(&server);
    MDNS.addService("http", "tcp", 80);
  #endif
  __OTA_init__();
  ArduinoOTA.begin();
  Serial.println("OTA started");
}






int timer = millis();
int previousMillis_wifi = millis();
uint8_t _t = 0;
void loop(void)
{
  if (WiFi.status() == WL_CONNECTED)
  {
      if(_t)
        Serial.println("Connected");
      _t = 0;
      int c = 0;
      server.handleClient();
      ArduinoOTA.handle();
      #ifdef DISPLAY_ssd1306
        display.display();
      #endif 
      #ifdef LAMP_MODE
        lamp();
      #endif
      if(__connected)
      {
          if(millis() - timer > 1000)
          {
            timeClient.update();
            timer = millis();
          }
      }
  }
  else
  {
    //запуск подключения wifi
    if (millis() - previousMillis_wifi >= 5000)
    {
      Serial.println("Reconnecting");
      previousMillis_wifi = millis();
      setup_wifi();
      _t++;
    }
    if (_t > 5)
    {
      WiFi.softAP("esp32", "12345678");
      ip = WiFi.softAPIP();
      Serial.println("\nTimeout. Created new AP. esp32");
      _t = 0;
    }
  }
  
  delay(2);//allow the cpu to switch to other tasks
}
