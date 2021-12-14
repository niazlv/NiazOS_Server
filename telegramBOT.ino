#ifdef TELEGRAM
#include <UniversalTelegramBot.h>

String BOT_TOKEN = "xxxxxxxxxx:!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;

#define TELEGRAM_DEBUG 

UniversalTelegramBot bot(BOT_TOKEN, secured_client);

unsigned long bot_lasttime = millis(); // last time messages' scan has been done

int numNewMessages;

void tg_init(){
  configTime(0, 0, "pool.ntp.org");
  secured_client.setTrustAnchors(&cert);
  bot.longPoll = 60;
}

void handleNewMessages(int numNewMessages)
{
  for (int i = 0; i < numNewMessages; i++)
  {
    Serial.println(bot.messages[i].text);
    digitalWrite(rele, ((bot.messages[i].text == "1")?1:0));
    bot.sendMessage(bot.messages[i].chat_id, bot.messages[i].text, "");
  }
}

void tg_handler(){
  if (millis()- bot_lasttime > 5000){
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    handleNewMessages(numNewMessages);
    bot_lasttime = millis();
  }
}
#endif
