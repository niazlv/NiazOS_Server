
#ifndef NIAZOS_H
# define NIAZOS_H
//Для загрузки прошивки по воздуху
# include <WiFiUdp.h>
# include <ArduinoOTA.h>
# include "NTPClient.h"

//# include "LittleFS.h" // LittleFS is declared

//Для работы со временем(внутренняя библиотека)
# include "UnixTime.h"

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


#endif
