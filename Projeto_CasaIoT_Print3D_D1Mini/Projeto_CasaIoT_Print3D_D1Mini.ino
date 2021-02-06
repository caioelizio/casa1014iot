/*********************************************************************/
#define autor       "Caio Elizio - caioelizio@gmail.com 62 98223-9516"
#define programa    "Print3D "
#define Version     "v1.7.2"
#define dataVersion "19/08/2020"
#define placa       "WeMos D1 mini ESP-12E"
/*********************************************************************/
/*********************************************************************
***Referencias
   https://internetecoisas.com.br
****Lista de Peças:
   1 WeMos D1 mini ESP-12E
   1 Fonte 5v (carregador de celular)
   1 buzzer
   1 Sensor Temperatura Umidade DHT11
   2 Reles
   1 Remoto IR
   1 controle remoto creative
   1 Sensor detector de fogo   
*/

//#include "Arduino.h"
#include <EMailSender.h>
//grilo3dprint@gmail.com
//buceta13
uint8_t connection_state = 0;
uint16_t reconnect_interval = 10000;
//EMailSender emailSend("smtp.account@gmail.com", "buceta13");
EMailSender emailSend("smtp.grilo3dprint@gmail.com", "buceta13");


//Prototypes
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void button();
void rele1(boolean b_valor);
void rele2(boolean b_valor);
void tudoff();
void f_atualiza_Sensores();
void f_TimeRele1();
void f_atualiza_info();
//Functions Funções
void f_Serial_Sistema();
void f_Serial_Wifi();
//LCD
void f_LCD_Wifi_erro();
void f_LCD_DHT_erro();
//Websever
boolean configRead();
boolean configSave();
boolean dhtRead();
boolean dhtSave();
boolean TimeRead();
boolean TimeSave();
void iniThingspeak();

#include "config.h"
#include "funcoes.h"
#include "wifi.h"
#include "SensorFogoLM393.h"
#include "DHT11.h"
#include "firebase.h"
#include "lcd.h"
#include "websever.h"
#include "ThingSpeak_2.h"

#include "MQTT_Client.h"
//Definicoes mqtt
//Função: função de callback é chamada toda vez que uma informação de um dos tópicos subescritos chega
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    String msgRecebida;
    for(int i = 0; i < length; i++) { //obtem a string do payload recebido
       char c = (char)payload[i];
       msgRecebida += c;
    }
    Serial.println(); //Linha em branco
    Serial.println(F("<----------------------MQTT----------------------->"));  
    Serial.print(  "topic:       "); Serial.println(topic);
    Serial.print(  "msgRecebida: "); Serial.println(msgRecebida);
    Serial.println(F("<------------------------------------------------->"));

//Definicoes mqtt
//toma ação dependendo da string recebida:
    if (msgRecebida == "somteclason"){
      f_AtivaSomTeclas(ON);
    }
    if (msgRecebida == "somteclasoff"){
      f_AtivaSomTeclas(OFF);
    }
    if (msgRecebida == "rele1on"){
      rele1(ON);
      f_buzzbeep();
    }
    if (msgRecebida == "rele1off"){
      rele1(OFF);
      f_buzzbeep();
    }
    if (msgRecebida == "rele2on"){
      rele2(ON);
      f_buzzbeep();
    }
    if (msgRecebida == "rele2off"){
      rele2(OFF);
      f_buzzbeep();
    }
    if (msgRecebida == "tudoff"){
      tudoff();
      f_buzzbeep();
    }
} //void mqtt_callback(


void rele1(boolean b_valor) {
  Serial.println();
  if (b_valor == ON) {
    iStatus_Rele1 = ON;
    //EEPROM_Write(20, iStatus_Rele1); //limite de 10mil gravações
    Serial.print(cRele1); Serial.println("ON");
    digitalWrite(pinRele1,  ON);
//    ihrsProgMin = ihrsProgMin2;
    i_minutos_Time = ihrsProgMin;
    f_LedESP(ON);
    MQTT.publish(TOPICO_PUBLISH, "fitaledon");
  } else {
    iStatus_Rele1 = OFF;
    //EEPROM_Write(20, iStatus_Rele1); //limite de 10mil gravações
    Serial.print(cRele1); Serial.println("OFF");
    digitalWrite(pinRele1,  OFF);
    f_LedESP(OFF);
    MQTT.publish(TOPICO_PUBLISH, "fitaledoff");
  }
}
void rele2(boolean b_valor) {
  Serial.println();
  if (b_valor == ON) {
    f_LCD_Backlight(ON);

    //EEPROM_Write(21, 1);  //limite de 10mil gravações
    iStatus_Rele2 = ON;
    Serial.print(cRele2); Serial.println("ON"); //Luz portao
    //Alterado a posicao dos pins para usar o modulo com 2 reles
    //  digitalWrite(pinRele2,  ON);  //Luz
    digitalWrite(pinRele2,  OFF);     //Luz //Modulo com 2 rele invertido
    f_LedESP(ON);
  } else {
    f_LCD_Backlight(OFF);

    //EEPROM_Write(21, 0); //limite de 10mil gravações
    iStatus_Rele2 = OFF;
    Serial.print(cRele2); Serial.println("OFF");
    //    digitalWrite(pinRele2,  OFF);  //Luz
    digitalWrite(pinRele2,  ON);     //Luz //Modulo rele invertido
    f_LedESP(OFF);
  }
}
void tudoff() {
  Serial.println();
  Serial.println(F("Tudo off"));
  rele1(OFF);
  rele2(OFF);
  MQTT.publish(TOPICO_PUBLISH, "tudoff");
}

void f_TimeRele1(){
  if (iTime == 1){  
    long now = millis()/1000;
    if (i_minutos_Time = ihrsProgMin){
      i_minutos_Time--;
      Serial.println();
      Serial.println(F("Time - Fita Led OFF"));
      rele1(OFF);
      lastMsg = now;
//    Serial.print(F("lastMsg: ")); Serial.println(lastMsg);
    }
  }
}

  boolean b_button1 = false;
void button() { //Apaga o led enquanto precionado o botao
  int estadoBotao = digitalRead(pinButton);
  if (estadoBotao == 0) {        //se estado do botão on...
    Serial.println(); 
    Serial.print(F("Botao: ")); Serial.println(estadoBotao);
    if (iStatus_Rele1 == ON){
      rele1(ON);
    }else{
      rele1(OFF);
    }
  } else {
//    Serial.print(F("Botao1 ")); Serial.print(F("OFF"));
    //sirene();
  }
}

void f_atualiza_info(){
  long now = millis()/1000;
  if (now - lastMsg > 10){
    lastMsg = now;
    Serial.println();
    Serial.println(F("<---------------Atualizando Informações----------->"));
    Serial.print(F("lastMsg: ")); Serial.println(lastMsg);
    Serial.println(F("<------------------------------------------------->"));
  }
}
void f_atualiza_Sensores() {
  Serial.println();
  Serial.println(F("<---------------Atualizando Sensores-------------->"));
  //f_TimeAtualiza();
  //f_Serial_DHT11(); //Atualiza Sensor DHT11

  Serial.println(F("<------------------------------------------------->"));
}


//Definicoes Setup---------------------------------------
void setup() {
  EEPROM.begin(512);
  initOutput(); //Define a porta status de saida e entrada
  initSerial();
  iniLCD();
  initWiFi();
  //f_WifiAP();
  iniWebsever();
  initOTA();
  initUDP();    //Definicoes Data Hora / WiFiUdp NTP time
  initMQTT();
  iniDHT11();
//test  iniThingspeak();
  //iniFirebase();
  //
  a = new Thread(10);          //Webserver / OTA
  b = new Thread(1000);        //a cada 1seg f_digitalClock / f_atualiza_LCD
  c = new Thread(1000*60);     //a cada 60seg f_Serial_digitalClock
  d = new Thread((1000*59)*5); //5m a cada 59seg FirebaseDHT11
  e = new Thread(1000*45);     //a cada 45seg f_Serial_Wifi
  f = new Thread(1000*2);      //10m a cada 59seg f_atualiza_info

//  f_EMailSender();
  i_minutos_Time = ihrsProgMin;

  f_piscaLedESP(3);
  f_buzzbeep(); //1 bip
  delay(10);
}
//erro  long now = 0;
//Repetição programa principal---------------------------
void loop() {
  yield();                          //WatchDog
  f_timeClock();
  if (a->posDelay()) { a->setCount(); //a cada 10m //OTA / Server DNS / Webserver
    MQTT.loop();                    //keep-alive da comunicação com broker MQTT
    ArduinoOTA.handle();              //OTA
    dnsServer.processNextRequest();   //Server DNS
    server.handleClient();            //Webserver
    //trava  VerificaConexoesWiFIEMQTT();  //garante funcionamento das conexões WiFi e ao broker MQTT
  }
  if (b->posDelay()) { b->setCount(); //a cada 1seg //f_digitalClock / f_atualiza_LCD
    reconectWiFi_v2();
    f_digitalClock();                 //atualiza variaveis time digital
    f_atualiza_LCD();
    //f_SensorFogoLM393();
    button();
  }
  if (c->posDelay()) { c->setCount(); //a cada 60seg
    f_Serial_Sistema();
    f_Serial_Wifi();
    f_Serial_digitalClock();
    f_Serial_DHT11(); //Serial
    f_TimeRele1();
  }
  if (d->posDelay()) { d->setCount(); //a cada 5 minutos
    //FirebaseDHT11();
  }
  if (e->posDelay()) { e->setCount(); //a cada 45seg
    //f_atualiza_Sensores();
    f_SensorDHT11();
    Thingspeak();
//test    VerificaConexoesWiFIEMQTT();  //garante funcionamento das conexões WiFi e ao broker MQTT
  }
  if (f->posDelay()) { f->setCount(); //a cada 2seg
  }
}
