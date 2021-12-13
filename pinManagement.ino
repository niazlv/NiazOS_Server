#ifdef _Button
uint8_t buttonpos = 0;
int t = millis();
int blockTime = 0;
void handleButton()
{
  if(digitalRead(btninput) == 1 && !buttonpos)
  {
    if(millis()-t > blockTime)
    {
      digitalWrite(rele, !digitalRead(rele));
      buttonpos = 1;
      blockTime = 0;
    }
  }
  else if (digitalRead(btninput) == 0)
    buttonpos = 0;
}

void handlePins()
{
  String message = "button1 = " + (String)(!(digitalRead(rele))? "On": "Off");
  if (server.args() != 0)
  {
    for(int i = 0; i < server.args();i++)
    {
      if (server.argName(i)== "button1")
      {
        if(server.arg(i) == "")
          message = (String)(!(digitalRead(rele))? 1 : 0);
        else
        {
          //inverted
          digitalWrite(rele, (server.arg(i)== "1")? 0 : 1);
          message = "Button1 "+ (String)((server.arg(i)=="1")? "On" : "Off");
        }
      }
      if (server.argName(i)== "blockTime")
      {
        blockTime = server.arg(i).toInt();
        t = millis();
        message = "block time setted " + (String)(blockTime/1000) + " seconds";
      }
    }
  }
  server.send(200, "text/plain", message);
}
#endif
