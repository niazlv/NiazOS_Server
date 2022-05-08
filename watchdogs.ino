//проверяет время и спасает от переполнения таймера millis(), перезапуская систему раз в 30 суток
void check_uptime()
{
  //миллисекунды -> секунды -> часы -> дни 
  if(((millis()/1000)/3600)/24 > 30)
    ESP.restart();
}

//отключает блокировку кнопки, если нет интернета
void unblock_button_if_dnc()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    #ifdef _Button
      blockTime = 0;
    #endif
  }
}

void get_time()
{
  stamp.getDateTime(timeClient.getEpochTime());
}

void watchdogs()
{
  check_uptime();
  #ifdef _Button
    unblock_button_if_dnc();
  #endif
  //get_time();
}
