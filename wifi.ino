void setup_wifi()
{
  // We start by connecting to a WiFi network
  // Удаляем предыдущие конфигурации WIFI сети
  WiFi.disconnect(); // обрываем WIFI соединения
  WiFi.softAPdisconnect(); // отключаем отчку доступа(если она была
  WiFi.mode(WIFI_OFF); // отключаем WIFI
  delay(500);

  // присваиваем статичесий IP адрес
  WiFi.mode(WIFI_STA); // режим клиента
  //WiFi.config(IPAddress(192, 168, 1, 73), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0), IPAddress(192, 168, 1, 1));

  delay(10);
  WiFi.begin(ssid, password);
  
}