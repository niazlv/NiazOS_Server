#ifdef LAMP_MODE

#define R_pin 10//15//D8
#define G_pin 13//D7
#define B_pin 4 //D2 

#define W_pin 14  //D6
#define Y_pin 12  //D5

#define lampONpin 2  //D4   Чтобы включить лампу

#define _50HZ_pin 5   //D1

bool blampinit = false;

void lampinit()
{
  if(!blampinit)
  {
    //pinMode(R_pin,OUTPUT);
    //pinMode(G_pin,OUTPUT);
    //pinMode(B_pin,OUTPUT);
    //pinMode(W_pin,OUTPUT);
    //pinMode(Y_pin,OUTPUT);
    pinMode(lampONpin,OUTPUT);
    //pinMode(_50HZ_pin,OUTPUT);
    blampinit = true;
    Serial.println("lamp: pins inited!");
  }
  digitalWrite(lampONpin, 0);
  analogWrite(W_pin, 255);
  analogWrite(Y_pin, 0);
  analogWrite(R_pin, 0);
  analogWrite(G_pin, 0);
  analogWrite(B_pin, 0);
  digitalWrite(lampONpin, 1);
}

int timerLamp = millis();
bool t = true;
bool lamp_status = false;

int hz = 50;

void lamp()
{
  /*
  if(millis() - timerLamp >= 1000/(hz*2))
  {
    digitalWrite(_50HZ_pin, !digitalRead(_50HZ_pin));
    timerLamp = millis();
  }
  */
  //lampinit();
}

void handle_lamp()
{
  String message = "OK";
  for (int i = 0; i < server.args(); i++)
  {
    if (server.argName(i) == "status")
    {
      if (server.arg(i) == "1")
        lamp_status = true;
      if (server.arg(i) == "0")
        lamp_status = false;
      digitalWrite(lampONpin, server.arg(i).toInt());
      message = "i'm okay!\n";
      message += "settings setted: ";
      message += lamp_status?"1":"0";
      //server.send(200, "text/plain", message);
    }
    if (server.argName(i) == "RGB")
    {
      digitalWrite(R_pin, !digitalRead(R_pin));
    }
    if (server.argName(i) == "W" || server.argName(i) == "Y" || server.argName(i) == "R" || server.argName(i) == "G" || server.argName(i) == "B")
    {
      if(0 <= server.arg(i).toInt() && server.arg(i).toInt() <= 255)
      {
        message += "\n"+server.argName(i);
        message += server.arg(i);
        if(server.argName(i) == "W")
          analogWrite(W_pin,server.arg(i).toInt());
        else if(server.argName(i) == "Y")
          analogWrite(Y_pin,server.arg(i).toInt());
        else if(server.argName(i) == "R")
          analogWrite(R_pin,server.arg(i).toInt());
        else if(server.argName(i) == "G")
          analogWrite(G_pin,server.arg(i).toInt());
        else if(server.argName(i) == "B")
          analogWrite(B_pin,server.arg(i).toInt());
      }
      else
      {
        message = "\nwrong argument! use 0-255 numbers in argument!!";
        //server.send(200, "text/plain", message);
      }
    }
    if (server.argName(i) == "hz")
    {
      hz = server.arg(i).toInt();
      server.send(200, "text/plain", "Setted");
    }
    if (server.argName(i) == "getPins")
    {
      message = "Pin status:\n";
      message += "R_pin ";
      message += digitalRead(R_pin);
      message += "\n";

      message += "G_pin ";
      message += digitalRead(G_pin);
      message += "\n";

      message += "B_pin ";
      message += digitalRead(B_pin);
      message += "\n";

      message += "W_pin ";
      message += digitalRead(W_pin);
      message += "\n";
      
      message += "Y_pin ";
      message += digitalRead(Y_pin);
      message += "\n";

      message += "lampONpin ";
      message += digitalRead(lampONpin);
      message += "\n";

      message += "_50HZ_pin ";
      message += analogRead(_50HZ_pin);
      message += "\n";
    }
  }
  server.send(200, "text/plain", message);
}
#endif
