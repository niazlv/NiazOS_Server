#include "NiazOS.h"
//#include <Ticker.h>
/*
 * ------------------------SETTINGS-------------------------
 */
//Включит в компоненты компиляции Модуль лампы
#define LAMP_MODE

//подключает управление кнопкой на пинах
//#define _Button

//включает режим дебаг(выставляет нужные значения для голого МК)
//#define DEBUG_MODE

//Включает телеграмм в компиляцию
//#define TELEGRAM

//Включает функции дисплея SSD1306 
//#define DISPLAY_ssd1306

#define OSVERSION 0.62

#if !defined LAMP_MODE && !defined _Button
  String about = "Multi microcontroller";
#endif
#if defined _Button && !defined LAMP_MODE
  String about = "Lamp button. Control light in the room";
#endif
#if defined LAMP_MODE && !defined _Button
  String about = "Lamp controller.";
#endif



const int httpsPort = 443;  //Адрес порта для HTTPS= 443 или HTTP = 80
const char fingerprint[] PROGMEM = "5B:FB:D1:D4:49:D3:0F:A9:C6:40:03:34:BA:E0:24:05:AA:D2:E2:01"; //ключ для шифрования

/*
 * -------------------------------------------------------------
 */


#ifndef DEBUG_MODE
# define RELEASE_MODE
# ifdef _Button
  # define btninput 14 
  # define rele 13
# endif
#endif

//aka debug mode
#ifdef DEBUG_MODE
# define Blink      //Включаем Blink
# ifdef _Button
  # define btninput 5
  # define rele 4
# endif
#endif

char ssid[32] = "Home";
char password[32] = "34ValI45";

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
  #define SCREEN_WIDTH 128
  #define SCREEN_HEIGHT 64 
  uint8_t sda=12,scl=14;
  
  #define SCREEN_ADDRESS 0x3C 
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
#endif

char daysOfTheWeek[7][12] PROGMEM = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Определение NTP-клиента для получения времени
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", UTC * 60 * 60);  //UTC +3 = 3 * 60 * 60


UnixTime stamp(0);    //timeClient и так уже добавил UTC 
#ifdef ESP8266
  ESP8266HTTPUpdateServer httpUpdater;
#endif

IPAddress ip;

//Ticker timerz;

//даёт псевдо уникальное число строки
int ft_len(char *str)
{
  int i = 0,c = 0,d = 0;
  while(*(str+i) != 0)
    i++;
  c = i;
  for(i = 0; i < c;i++)
  {
    d += *(str+i) % 2;
    d += *(str+i) % 16;
    d += *(str+i) % 64;
  }
  d += c;
  return (d);
}

void setup(void)
{
  //timerz.attach(0.125,lamp);
  Serial.begin(115200);
  //LittleFS.format();
  #ifdef _on_LittleFS
  LittleFS.begin();
  
  LittleFS.setTimeCallback(get_unix);
  //небезопасная работа с файлом. Читаем конфиг.
  if (!LittleFS.exists("/config.txt"))
  {
    Serial.println("/config.txt don't exist. Create file...");
    File file = LittleFS.open("/config.txt", "w");
    file.println("SSID: " + (String)ssid);
    file.println("password: " + (String)password);
    file.close();
  }
  else
  {
    Serial.println("/config.txt is exist");
    File file = LittleFS.open("/config.txt", "r");
    int c = 0;
    while(file.available())
    {
      String str = file.readString();
      //Требуется переписать с использованием String.
      //Почитай https://alexgyver.ru/lessons/strings/
      char *st = strtochar(str);
      char *stt;
      char **split = ft_split_c((const char *)st, '\n');
      int len;
      len = ft_find((const char *)split[0], "SSID: ",true);
      if(len != -1)
      {
        stt = ft_cut((const char *)split[0],len);
        ft_strcpy(ssid,(const char *)stt);
        Serial.print("CHAR *= ");
        Serial.println(ssid);
      }
      len = ft_find((const char *)split[1], "password: ",true);
      if(len != -1)
      {
        stt = ft_cut((const char *)split[1],len);
        ft_strcpy(password,(const char *)stt);
        Serial.print("CHAR *= ");
        Serial.println(password);
      }
      //освободим память, которую вызвали(нужно, ибо память не бесконачная)
      //как раз по этой причине стоит переписать "библиотеку" работы с "строками" в динамический вид
      free(st);
      free(stt);
      ft_clear(split);
      
      c++;
    }
    file.close();
  }
  #endif
  /*
  EEPROM.begin(128);    //выделим память под EEPROM 
  int t;
  int start_addr = 10;
  int uniid = ft_len(ssid)+ft_len(password);
  EEPROM.get(start_addr - 4, t);
  if(t != uniid)
  {
    EEPROM.put(start_addr, ssid);
    EEPROM.put(sizeof(ssid) + start_addr, password);
    EEPROM.put(start_addr - 4, uniid);
    EEPROM.commit();
  }
  else
  {
    EEPROM.get(start_addr, ssid);
    EEPROM.get(sizeof(ssid) + start_addr, password);
  }
  */
  

  #ifdef _Button
    pinMode(btninput, INPUT);
    pinMode(rele, OUTPUT);
    digitalWrite(rele, 1);
  #endif
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
  #ifdef LAMP_MODE
    lampinit();
  #endif
  int c = 0;
   
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    #ifdef _Button
      handleButton();
    #endif
    #ifdef LAMP_MODE
      lamp();
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
  WiFi.softAP(hostssid, hostpassword,1,true,8);
  Serial.print("AP: "+(String)hostssid+"\npaswd: "+(String)hostpassword+"\n"+"\nip: ");
  Serial.println(WiFi.softAPIP());
  #ifdef LAMP_MODE
    lampinit();
  #endif
}





int timer = millis();
int previousMillis_wifi = millis();
uint8_t _t = 0;
void loop(void)
{
  watchdogs();
  #ifdef _Button
    handleButton();
  #endif
  
  #ifdef LAMP_MODE
    lamp();
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
