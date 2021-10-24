int buttonTimer = millis();
uint8_t buttonpos = 0;
void handleButton()
{
  if(digitalRead(btninput) == 1 && !buttonpos)
  {
    if(millis() - buttonTimer >= 500)
    {
      buttonTimer = millis();
      digitalWrite(rele, !digitalRead(rele));
    }
    buttonpos = 1;
  }
  else
    buttonpos = 0;
}

void handlePins()
{
  String message = "Args null";
  if (server.args() != 0)
  {
    for(int i = 0; i < server.args();i++)
    {
      if(server.argName(i)== "button1")
      {
        digitalWrite(rele, (server.arg(i)== "1")? 0 : 1);
        message = "Button1 "+ (String)((server.arg(i)=="1")? "On" : "Off");
      }
    }
  }
  server.send(200, "text/plain", message);
}
