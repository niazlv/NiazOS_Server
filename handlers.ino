void handleRoot() {
  digitalWrite(LED, !statusLED);
  server.send(200, "text/plain", "hello from esp32!");
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
  message += "Mac address: " + (String)WiFi.macAddress() + "\n";
  message += "IP address: ";
  message += WiFi.localIP().toString();
  message += "\n";
  message += "Connected to: '" + (String)WiFi.SSID() + "', RSSI: " + (String)WiFi.RSSI() + " dBm\n";
  message += "NiazOS version: " + (String)OSVERSION + "\n";
  message += "Server uptime: " + (String)(millis()/1000) + " seconds\n";
  message += "Real time: " + (String)daysOfTheWeek[timeClient.getDay()] + ", " + (String)timeClient.getFormattedTime() + "\n";
  message += "About: " +about+"\n";
  server.send(200, "text/plain", message);
}

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
  
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.on("/LED", response);
  server.on("/LEDOn", handleLedOn);
  server.on("/LEDOff", handleLedOff); 

  server.on("/wifi", handleWiFi);
  
  server.on("/restart", []() {
    ESP.restart();
  });

  server.on("/info", handleInfo);

  server.on("/pin", handlePins);
  
  #ifdef LAMP_MODE
    server.on("/lamp", handle_lamp);
  #endif
  
  server.onNotFound(handleNotFound);
}
