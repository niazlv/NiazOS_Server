void handleRoot() {
  digitalWrite(LED, !statusLED);
  String message = "hello from ";
  #ifdef ESP32
    message += "ESP32\n";
  #endif
  #ifdef ESP8266
    message += "ESP8266\n";
  #endif
  message += "\nThis port/address is needed to control one of the modules of the smart home system. If you are not familiar with it or have not been "
  "Direct instructions from the chief administrator, then I suggest you leave the page.";
  server.send(200, "text/plain", message);
  digitalWrite(LED, statusLED);
}
#ifdef DISPLAY_ssd1306
void handleDisplay()
{
  String message = "Args null, nothing displayed";
  if (server.args() != 0)
  {
    message = "args ";
    message += server.args();
    message += "\n";
    for (int i = 0; i < server.args(); i++)
    {
      if(server.argName(i) == "text")
      {
        display.print(server.arg(i));
        message += "\ndisplayed text: ";
        message += server.arg(i);
        message += "\n";
      }
      if(server.argName(i) == "setCursor")
      {
        display.setCursor(0,0);
        message += "\nCursor placed a x=0 y=0\n";
      }
      if(server.argName(i) == "clearDisplay")
      {
        display.clearDisplay();
        message += "\nDisplay cleared\n";
      }
    }
  }
  server.send(200, "text/plain", message);
}
#endif

void handleInfo()
{
  String message = "Info about this microcontroller: \n\n";
  #ifdef DEBUG
    message += "Debug enable. Please upload a release mode(Debug mode have a some bugs\n";
  #endif
  message += "Mac address: " + (String)WiFi.macAddress() + "\n";
  message += "IP address: ";
  message += WiFi.localIP().toString();
  message += "\n";
  message += "Connected to: '" + (String)WiFi.SSID() + "', RSSI: " + (String)WiFi.RSSI() + " dBm\n";
  message += "NiazOS version: " + (String)OSVERSION + "\n";
  message += "Server uptime: " + (String)(millis()/1000) + " seconds\n";
  message += "Real time: " + (String)daysOfTheWeek[timeClient.getDay()] + ", " + (String)timeClient.getFormattedTime() + "\n";
  message += "About: " +about+"\n";


  
  message += "\n\nInfo for devolopers:\n\n";
  message += "Fingerprint: " + (String)fingerprint + (String)"\n";
  message += "MK at board: ";
  #ifdef ESP32
    message += "ESP32\n";
  #endif
  #ifdef ESP8266
    message += "ESP8266\n";
  #endif
  #if !defined ESP32 && !defined ESP8266
    message += "none\n"
  #endif
  message += "CPU Freq: " + (String)ESP.getCpuFreqMHz() + (String)" MHz\n";
  message += "ESP CPU cycle count: " + (String)ESP.getCycleCount() + (String)"\n";
  message += "ESP Chip Model: ";
  #ifdef ESP8266
    message += "\n";
    message += "ESP Chip ID: " + (String)ESP.getChipId() + (String)"\n";
    message += "Core Version: " + (String)ESP.getCoreVersion() + (String)"\n";
  #endif
  #ifdef ESP32
    message += (String)ESP.getChipModel() + (String)"\n";
    message += "ESP Chip ID: " + (String)((uint32_t)ESP.getEfuseMac()) + (String)"\n";
    message += "Core count: " + (String)ESP.getChipCores() + (String)"\n";
  #endif
  message += "SDK Version: " + (String)ESP.getSdkVersion() + (String)"\n";
  message += "MD5 sign a OS: " + (String)ESP.getSketchMD5() + (String)"\n";
  message += "Flash size(visible via SDK): " + (String)(ESP.getFlashChipSize()/1024) + (String)" KiB\n";
  message += "Flash size(shoud be based on Chip ID): ";
  #ifdef ESP8266
    message += (String)(ESP.getFlashChipRealSize()/1024) + (String)" KiB\n";
  #endif
  #ifdef ESP32
    message += "\n";
  #endif
  message += "Flash Freq: " + (String)(ESP.getFlashChipSpeed()/1000000) + (String)"MHz\n";
  message += "Flash ID: ";
  #ifdef ESP8266
    message += (String)ESP.getFlashChipId() + (String)"\n";
  #endif
  #ifdef ESP32
    message += "\n";
  #endif
  message += "Size of Sketch: " + (String)(ESP.getSketchSize()/1024) + "/" + (String)(ESP.getFlashChipSize()/1024) + (String)" KiB\n";
  message += "Free memory: " +  (String)(ESP.getFreeSketchSpace()/1024) + "/" + (String)(ESP.getFlashChipSize()/1024) + (String)" KiB\n";
  message += "Free HEAP: " + (String)ESP.getFreeHeap() + (String)"\n";
  message += "Heap fragmentation: ";
  #ifdef ESP8266
    message += (String)ESP.getHeapFragmentation() + (String)"%\n";
  #endif
  #ifdef ESP32
    message += "\n";
  #endif
  message += "CRC: ";
  #ifdef ESP8266
    message += (String)((ESP.checkFlashCRC()) ? "true":"false") + (String)"\n";
  #endif
  #ifdef ESP32
    message += "\n";
  #endif
  
  server.send(200, "text/plain", message);
}

#ifdef ESP32
void handleBluetooth()
{
  String message = "Function is anavable\nmicrocontroller not enough memory to import created by someone a dev-ops. Need to write his manualy.";
  server.send(404, "text/plain", message);
}
#endif

void handleWiFi()
{
  String message = "found ";
  if (server.args() == 0)
  {
    int n = WiFi.scanNetworks();
    #ifdef DISPLAY_ssd1306
      display.clearDisplay();
    #endif
    if (n != 0)
    {
      message += n + 1;
      message += "\n";
      for (int i = 0; i < n; i++)
      {
        #ifdef DISPLAY_ssd1306
          display.setCursor(0, i * 8);
          display.print(i+1);
          display.print(". ");
          display.print(WiFi.SSID(i));
          display.print(" ");
          display.print(WiFi.RSSI(i));
        #endif
        Serial.print(i+1);
        Serial.print(". ");
        Serial.print(WiFi.SSID(i));
        Serial.print(" ");
        Serial.println(WiFi.RSSI(i));
        message += i + 1;
        message += ". ";
        message += WiFi.SSID(i);
        message += " ";
        message += WiFi.RSSI(i);
        message += "\n";
      }
      server.send(200, "text/plain", message);
    }
    else
      server.send(404, "text/plain", "Nothing SSID found.");
  }
  else
  {
    for (int i = 0; i < server.args(); i++)
    {
      if (server.argName(i) == "mac")
      {
        #ifdef DISPLAY_ssd1306
          display.clearDisplay();
          display.setCursor(0,0);
          display.print(WiFi.macAddress());
        #endif
        Serial.println(WiFi.macAddress());
        message = "mac address: ";
        message += WiFi.macAddress();
      }
      //if (server.argName(i) == "connect")
    }
    server.send(200, "text/plain", message);
  }
}

void handleNotFound() {
  digitalWrite(LED, !statusLED);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(LED, statusLED);
}

void handlers()
{
  server.on("/", handleRoot);
  
  #ifdef DISPLAY_ssd1306
    server.on("/display", handleDisplay);
  #endif
  
  #ifdef DEBUG_MODE
    server.on("/inline", []() {
      server.send(200, "text/plain", "this works as well");
    });
  #endif
  #ifdef Blink
    server.on("/LED", response);
    server.on("/LEDOn", handleLedOn);
    server.on("/LEDOff", handleLedOff); 
  #endif
  server.on("/wifi", handleWiFi);
  
  #ifdef ESP32
    server.on("/bluetooth", handleBluetooth);
  #endif
  
  server.on("/restart", []() {
    ESP.restart();
  });

  server.on("/info", handleInfo);
  #ifdef _Button
    server.on("/pin", handlePins);
  #endif
  #ifdef LAMP_MODE
    server.on("/lamp", handle_lamp);
  #endif
  
  server.onNotFound(handleNotFound);
}
