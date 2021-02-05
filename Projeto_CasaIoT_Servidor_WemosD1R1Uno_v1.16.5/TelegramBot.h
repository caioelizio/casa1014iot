/* Author: Caio Elizio - caioelizio@gmail.com 62 98223-9516"
 * Date: 07/08/20
 * Definicoes Telebot
 * Versao 1.0 Servidor
 * https://randomnerdtutorials.com/telegram-group-esp32-esp8266/
 */

#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h> // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
/*
* @myidbot para descobrir o ID de bate-papo de um indivíduo ou grupo
* precisa clicar em "iniciar" em um bot antes que ele possa 
* enviar uma mensagem para você
*/
#define CHAT_ID "233156735"
//#define CHAT_ID "-1227477795"
//grupo iot_elet._eng. s1227477795_4847115470765379312

// Initialize Telegram BOT
#define BOTtoken "1389247313:AAHxkAZ41YiiRUb4b6hVUeCSC0vfNvR3UbE"   // your Bot Token (Get from Botfather)


WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

//Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;


boolean StateLedEsp;
//Handle what happens when you receive new messages
//controlar quando você recebe novas mensagens
void handleNewMessages(int numNewMessages) {
//  Serial.print(F("Numero de novas mensagens: "));
//  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id); // Chat id of the requester
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }

    //Print the received message
    String text = bot.messages[i].text;
    Serial.println();
    Serial.print(F("Nova mensagens: ")); Serial.println(text);

    String from_name = bot.messages[i].from_name;
    if (text == "/start") {
      String welcome = "Bem vindo , " + from_name + ".\n";
             welcome += "Use os seguintes comandos: \n";
             welcome += "/temp_serv   \n";
             welcome += "/temp_qrt    \n";
             welcome += "/temp_sala   \n";
             welcome += "/led_on      \n";
             welcome += "/led_off     \n";
             welcome += "/VentSalaON  \n";
             welcome += "/VentSalaOFF \n";
      bot.sendMessage(chat_id, welcome, "");
    }
    if (text == "/temp_serv") {
      String mns = getRead_DHT11();
      bot.sendMessage(chat_id, mns  , "");
    }  
    if (text == "/temp_qrt") {
      String mns1 = getRead_DHT11_qrt();
      bot.sendMessage(chat_id, mns1  , "");
    }  
    if (text == "/temp_sala") {
      String mns2 = getRead_DHT11_sala();
      bot.sendMessage(chat_id, mns2  , "");
    }
    if (text == "/VentSalaON") {
      String mns = "Ventilador Sala ligado.";
      bot.sendMessage(chat_id, mns  , "");
      rele1(ON);
      myBroker.publish("casa1014/sala/sistema"        , "venton");
    }  
    if (text == "/VentSalaOFF") {
      String mns = "Ventilador Sala desligado.";
      bot.sendMessage(chat_id, mns  , "");
      rele1(OFF);
      myBroker.publish("casa1014/sala/sistema"      , "ventoff");
    }
    if (text == "/led_on") {
      bot.sendMessage(chat_id, "LED state set to ON", "");
      StateLedEsp = HIGH;
      f_LedESP(StateLedEsp);
    }    
    if (text == "/led_off") {
      bot.sendMessage(chat_id, "LED state set to OFF", "");
      StateLedEsp = LOW;
      f_LedESP(StateLedEsp);
    }
    
    if (text == "/state") {
      if (digitalRead(pinLedonboard)){
        bot.sendMessage(chat_id, "LED is ON", "");
      }
      else{
        bot.sendMessage(chat_id, "LED is OFF", "");
      }
    }
  
//
  }
}
void bot_receiver() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while(numNewMessages) {
//      Serial.println(F("Nova Mns Bot cas)a_iot");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
//*/


//https://core.telegram.org/bots/api
/*//getMe sendMessage
Use this method to send text messages. On success, the sent Message is returned.
chat_id                   Integer or String Yes Unique identifier for the target chat or username of the target channel (in the format @channelusername)
text                      String  Yes Text of the message to be sent, 1-4096 characters after entities parsing
parse_mode                String  Optional  Mode for parsing entities in the message text. See formatting options for more details.
disable_web_page_preview  Boolean Optional  Disables link previews for links in this message
disable_notification      Boolean Optional  Sends the message silently. Users will receive a notification with no sound.
reply_to_message_id       Integer Optional  If the message is a reply, ID of the original message
reply_markup              InlineKeyboardMarkup or ReplyKeyboardMarkup or ReplyKeyboardRemove or ForceReply  Optional  Additional interface options. A JSON-serialized object for an inline keyboard, custom reply keyboard, instructions to remove reply keyboard or to force a reply from the user.
//*/

/*//User
This object represents a Telegram user or bot.
id  Integer   Unique identifier for this user or bot
is_bot          Boolean True, if this user is a bot
first_name      String  User's or bot's first name
last_name       String  Optional. User's or bot's last name
username        String  Optional. User's or bot's username
language_code   String  Optional. IETF language tag of the user's language
can_join_groups Boolean Optional. True, if the bot can be invited to groups. Returned only in getMe.
can_read_all_group_messages Boolean Optional. True, if privacy mode is disabled for the bot. Returned only in getMe.
supports_inline_queries     Boolean Optional. True, if the bo
//*/
/*//Update
This object represents an incoming update.
Este objeto representa uma atualização de entrada.
update_id             Integer The update's unique identifier. O identificador exclusivo da atualização.
message               Message Optional. New incoming message of any kind — text, photo, sticker, etc.
edited_message        Message Optional. New version of a message that is known to the bot and was edited
channel_post          Message Optional. New incoming channel post of any kind — text, photo, sticker, etc.
edited_channel_post   Message Optional. New version of a channel post that is known to the bot and was edited
inline_query          InlineQuery Optional. New incoming inline query
chosen_inline_result  ChosenInlineResult  Optional. The result of an inline query that was chosen by a user and sent to their chat partner. Please see our documentation on the feedback collecting for details on how to enable these updates for your bot.
callback_query        CallbackQuery Optional. New incoming callback query
shipping_query        ShippingQuery Optional. New incoming shipping query. Only for invoices with flexible price
pre_checkout_query    PreCheckoutQuery  Optional. New incoming pre-checkout query. Contains full information about checkout
poll                  Poll  Optional. New poll state. Bots receive only updates about stopped polls and polls, which are sent by the bot
poll_answer           PollAnswer  Optional. A user changed their answer in a non-anonymous poll. Bots receive new votes only in polls that were sent by the bot itself.
//*/
/*//getUpdates
offset           Integer Optional  Identifier of the first update to be returned
limit            Integer Optional  Limits the number of updates to be retrieved. Values between 1-100 are accepted. Defaults to 100.
timeout          Integer Optional  Timeout in seconds for long polling. Defaults to 0, i.e. usual short polling. Should be positive, short polling should be used for testing purposes only.
allowed_updates  Array of String Optional  A JSON-serialized list of the update types you want your bot to receive. For example, specify [“message”, “edited_channel_post”, “callback_query”] to only receive updates of these types. See Update for a complete list of available update types. Specify an empty list to receive all updates regardless of type (default). If not specified, the previous setting will be used.
//*/
/*//setWebhook
url             String  Yes HTTPS url to send updates to. Use an empty string to remove webhook integration
certificate     InputFile Optional  Upload your public key certificate so that the root certificate in use can be checked. See our self-signed guide for details.
max_connections Integer Optional  Maximum allowed number of simultaneous HTTPS connections to the webhook for update delivery, 1-100. Defaults to 40. Use lower values to limit the load on your bot's server, and higher values to increase your bot's throughput.
allowed_updates Array of String Optional  A JSON-serialized list of the update 
//*/
