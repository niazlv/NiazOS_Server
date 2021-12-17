#ifdef LAMP_MODE

#define R_pin 6
#define G_pin 8
#define B_pin 10

#define W_pin 9
#define Y_pin 7

//doen't work!
#define off_controler_pin 16  //0   Чтобы выключить контроллер
#define onpins_pin 13         //1   Чтобы включить пины

#define _50HZ_pin 14
#define duty_pin 12

int timerLamp = millis();
bool t = true;
bool lamp_status = false;


void lamp()
{
  /*if (millis() - timerLamp >=20)
  {
    t = !t;
    digitalWrite(_50HZ_pin, t);
    timerLamp = millis();
  }
  digitalWrite(duty_pin, HIGH);
  */
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
      message = "i'm ";
      digitalWrite(off_controler_pin, !lamp_status);
      message += "not!";
      message += i;
      //server.send(200, "text/plain", message);
      //digitalWrite(onpins_pin, lamp_status);
      message = "i'm okay!";
      message += i;
      server.send(200, "text/plain", message);
    }
    if (server.argName(i) == "RGB")
    {
      digitalWrite(R_pin, !digitalRead(R_pin));
    }
    if (server.argName(i) == "getPins")
    {
      //digitalWrite(onpins_pin, HIGH);
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

      message += "off_controler_pin ";
      message += digitalRead(off_controler_pin);
      message += "\n";

      message += "onpins_pin ";
      message += digitalRead(onpins_pin);
      message += "\n";

      message += "_50HZ_pin ";
      message += analogRead(_50HZ_pin);
      message += "\n";

      message += "duty_pin ";
      message += analogRead(duty_pin);
      message += "\n";
      //digitalWrite(onpins_pin, LOW);
    }
  }
  server.send(200, "text/plain", message);
}
#endif
