#include "NiazOS.h"
/*
 * ------------------------SETTINGS-------------------------
 */
 //Включит в компоненты компиляции Модуль лампы
//#define LAMP_MODE

//подключает управление кнопкой на пинах
//#define _Button

//включает режим дебаг(выставляет нужные значения для голого МК)
//#define DEBUG_MODE

//Включает телеграмм в компиляцию
//#define TELEGRAM

//Включает функции дисплея SSD1306 
//#define DISPLAY_ssd1306

#define OSVERSION 0.42

#if !defined LAMP_MODE && !defined _Button
  String about = "Multi microcontroller";
#endif
#if defined _Button && !defined LAMP_MODE
  String about = "Lamp button. Control light in the room";
#endif
#if defined LAMP_MODE && !defined _Button
  String about = "Lamp. Can't control lamp. Because Scheme don't work";
#endif



const int httpsPort = 443;  //Адрес порта для HTTPS= 443 или HTTP = 80
const char fingerprint[] PROGMEM = "5B:FB:D1:D4:49:D3:0F:A9:C6:40:03:34:BA:E0:24:05:AA:D2:E2:01"; //ключ для шифрования

/*
 * -------------------------------------------------------------
 */


#ifndef DEBUG_MODE
# define RELEASE_MODE
# define btninput 14 
# define rele 13
#endif

//aka debug mode
#ifdef DEBUG_MODE
# define Blink      //Включаем Blink
# define btninput 5
# define rele 4
#endif

const char* ssid = "Home";
const char* password = "34ValI45";

const int UTC = 3; //UTC +3, Moscow

#ifdef ESP32
  const char* hostssid = "esp32";
#endif
#ifdef ESP8266
  const char* hostssid = "esp8266";
#endif
const char* hostpassword = "12345678";




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

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Определение NTP-клиента для получения времени
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", UTC * 60 * 60);  //UTC +3 = 3 * 60 * 60

#ifdef ESP8266
  ESP8266HTTPUpdateServer httpUpdater;
#endif

IPAddress ip;

void setup(void)
{
  pinMode(LED, OUTPUT);
  digitalWrite(LED, statusLED);
  Serial.begin(115200);
  //pinMode(13, OUTPUT);
  //digitalWrite(13, LOW);

  //LittleFS.begin();
  
  pinMode(btninput, INPUT);
  pinMode(rele, OUTPUT);
  digitalWrite(rele, 1);
  
  #ifdef DISPLAY_ssd1306
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      Serial.println(F("SSD1306 allocation failed"));
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
    #ifdef _Button
      handleButton();
    #endif
    c++;
    if(c > 20)
    {
      WiFi.softAP(hostssid, hostpassword);
      ip = WiFi.softAPIP();
      Serial.println("\nTimeout. Created new AP. "+(String)hostssid);
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
  }
  Serial.print("IP address: ");
  Serial.println(ip);


  if (MDNS.begin(hostssid)) {
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
      display.print("AP: "+(String)hostssid+"\npaswd: "+(String)hostpassword+"\n");
    }
    display.print("IP:");
    display.println(ip);
    
  #endif
  handlers();
  server.begin();
  #ifdef TELEGRAM
    tg_init();
  #endif
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
  #ifdef _Button
    handleButton();
  #endif
  if (WiFi.status() == WL_CONNECTED)
  {
      if(_t)
        Serial.println("Connected");
      _t = 0;
      int c = 0;
      server.handleClient();
      ArduinoOTA.handle();
      #ifdef TELEGRAM
        tg_handler();
      #endif
      #ifdef DISPLAY_ssd1306
        display.display();
      #endif 
      #ifdef LAMP_MODE
        lamp();
      #endif
      if(millis() - timer > 1000)
      {
        timeClient.update();
        timer = millis();
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
      WiFi.softAP(hostssid, hostpassword);
      ip = WiFi.softAPIP();
      Serial.println("\nTimeout. Created new AP. " + (String)hostssid);
      _t = 0;
    }
  }
  
  delay(2);//allow the cpu to switch to other tasks
}
