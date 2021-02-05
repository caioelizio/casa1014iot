/*********************************************************************/
  #define autor       "Caio Elizio - caioelizio@gmail.com 62 98223-9516"
  #define programa    "Servidor "
  #define versao      "v1.16.5"
  #define dataversao  "29/07/20"
  #define placa       "WeMos D1 R1 Uno ESP-12E - Broker"
/*********************************************************************/
/****Referencias 
 * 
 *     SERIAL_ECHOLNPGM("Data compilado: " __DATE__);
 * 
 * https://internetecoisas.com.br  
 ****Lista de Peças: 
 * 1 WeMos D1 R1 Uno ESP-12E 
 * 1 Fonte 5v (carregador de celular)
 * 1 buzzer
 * 1 Sensor Temperatura Umidade DHT11
 * 2 Reles
 * 1 Sensor PIR
 * 1 Remoto IR
 * 1 controle remoto creative
 * 1 P8 modulo tensao
*/
#include "config.h"


//Prototypes
void button();
void rele1(boolean b_valor);
void rele2(boolean b_valor);
void tudoff();
void f_TimeRele1(boolean b_valor);
void f_TimeRele2();
void f_atualiza_Sensores();
void EnviaMnsMQTT();
//Websever
boolean configRead();
boolean configSave();
boolean dhtRead();
boolean dhtSave();
void f_LCD_Backlight(boolean b_valor);


//import------------------------------------------------
#include "funcoes.h"
#include "wifi.h"
#include "websever.h"
#include "firebase.h"
#include "lcd.h"
#include "uSeverMQTTBroker.h"
#include "DHT11_MQTTBroker.h"
#include "SensorMovimentoPIR.h"
#include "SinricPro_v2.h"
#include "TelegramBot.h"


void rele1(boolean b_valor){
  if (b_valor == ON){
    EEPROM_Write(20,1);  //bStatusR1 = 1
    sStatus_Rele1 = "ON";
    Serial.println();
    Serial.print(cRele1); Serial.println(sStatus_Rele1);
    digitalWrite(pinRele1,  ON);
//    f_LedESP(ON);
    //Broker_pub("R1on");
    myBroker.publish(TOPICO_PUBLISH, "R1on");
  } else {
    EEPROM_Write(20,0);
    sStatus_Rele1 = "OFF";
    Serial.println();
    Serial.print(cRele1); Serial.println(sStatus_Rele1);
    digitalWrite(pinRele1,  OFF);
//    f_LedESP(OFF);
    myBroker.publish(TOPICO_PUBLISH, "R1off");
  }
}
void rele2(boolean b_valor){
  if (b_valor == ON){
    f_LCD_Backlight(ON);

    EEPROM_Write(21,1);
    sStatus_Rele2 = "ON";
    Serial.print(cRele2); Serial.println(sStatus_Rele2);
//    digitalWrite(pinRele2,  ON);
//    f_LedESP(ON);
    myBroker.publish(TOPICO_PUBLISH, "R2on");
  } else {
    f_LCD_Backlight(OFF);

    EEPROM_Write(21,0);
    sStatus_Rele2 = "OFF";
    Serial.print(cRele2); Serial.println(sStatus_Rele2);
//    digitalWrite(pinRele2,  OFF);
//    f_LedESP(OFF);
    myBroker.publish(TOPICO_PUBLISH, "R2off");
  }
}
void tudoff(){
  Serial.println();
  Serial.println(F("Tudo off"));
  rele1(OFF);
  rele2(OFF);
  f_TimeRele1(OFF);
  //f_LedESP(OFF);
  myBroker.publish(TOPICO_PUBLISH, "tudoff");
}

  boolean b_button1 = false;
void button(){ //Apaga o led enquanto precionado o botao
  int estadoBotao = digitalRead(pinButton);
  if (estadoBotao == 0){         //se estado do botão on...
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

void f_TimeRele1(boolean b_valor){
  if (b_valor == ON){
    sStatusTime = "ON";
    iTime = 1;
    EEPROM_Write(32,iTime);
    Serial.println(F("Time: ON"));
    Serial.print(F("Hora programada inicial: ")); Serial.println(ihrsProgIni);
    Serial.print(F("Hora programada final  : ")); Serial.println(ihrsProgFin);
    myBroker.publish(TOPICO_PUBLISH, "timeservon");
  }else{
    sStatusTime = "OFF";
    iTime = 0;
    EEPROM_Write(32,iTime);
    Serial.println(F("Time: OFF"));
    myBroker.publish(TOPICO_PUBLISH, "timeservoff");
  }
}
void f_TimeRele2(){
  if (iTime = 1){
//    Serial.println(); 
    Serial.println(F("Time ON"));
    if (ano == 1970){    //erro data
      //rele2(OFF);
      Serial.println(F("Reconectar timer"));
      initUDP();
      horas   = EEPROM_Read(1); //ler horas salva na memoria
      minutos = EEPROM_Read(2);
    }else{
      EEPROM_Write(1,horas);    //salvar hora na memoria
      EEPROM_Write(2,minutos);
    }
    if ( (horas >= ihrsProgIni) 
    or   (horas < ihrsProgFin) ) {
      rele2(ON);
    }else {
      rele2(OFF);
    }
  }else{
    Serial.println(F("Time rele2 OFF"));
  }
  if (b_debug == true){
    Serial.print(F("horas: "));   Serial.println(horas);
    Serial.print(F("minutos: ")); Serial.println(minutos);
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
void f_atualiza_Sensores(){
  Serial.println();
  Serial.println(F("<---------------Atualizando Sensores-------------->"));
  //f_TimeAtualiza();
//  f_SensorDHT11(); //Atualiza Sensor DHT11

  Serial.println(F("<------------------------------------------------->"));
}


//Definicoes Setup---------------------------------------
void setup(){
  EEPROM.begin(512);
  initOutput(); //Define a porta status de saida e entrada
  initSerial();
  client.setInsecure(); //TelegramBot
  iniLCD();

  //f_WifiAP(); //ou OTA ou AP
//  initWiFi();
  if (WiFiAP) f_WifiAP(); else initWiFi();
  iniWebsever();

  //iniFirebase();  
  initOTA();
  initUDP();      //Definicoes Data Hora / WiFiUdp NTP time
  initMQTT();
  iniDHT11();
//  setupSinricPro();

  a = new Thread(10);           //Webserver / OTA / MQTT / f_LerSensorIRrecv
  b = new Thread(1000);         //a cada 1seg  f_digitalClock / f_atualiza_LCD
  c = new Thread(1000*60);      //a cada 10seg f_Serial_digitalClock
  d = new Thread((1000*60)*2);  //a cada 5min f_firebaseDHT11
  e = new Thread(1000*45);      //a cada 45seg f_Serial_Wifi f_Serial_Sistema f_Serial_DHT11
  f = new Thread(1111);         //a cada 100ms f_atualiza_Sensores

//  f_piscaLedESP(3);
  f_buzzbeep();            //1 bip
  delay(10);
}
//Repetição programa principal---------------------------
void loop(){
  //bot_receiver();
  yield();                          //WatchDog
  f_timeClock();
  if(a->posDelay()){ a->setCount();
    ArduinoOTA.handle();            //OTA
    //SinricPro.handle();
    dnsServer.processNextRequest(); //Server DNS
    server.handleClient();          //Webserver
  }
  if(b->posDelay()){ b->setCount(); //a cada 1seg
    reconectWiFi_v2();
    f_digitalClock();        //ok atualiza variaveis time digital
    f_atualiza_LCD();
//    button();
  }
  if(c->posDelay()){ c->setCount(); //a cada 61seg
    f_Serial_Sistema();
    f_Serial_Wifi();
    f_Serial_digitalClock();        //escreve serial data hora
//    EnviaMnsMQTT();
  }
  if(d->posDelay()){ d->setCount(); //a cada 10min
    //f_atualiza_Sensores();
//erro serial    f_SensorDHT11(); //Atualiza Sensor DHT11
    f_SensorDHT11(); //Atualiza Sensor DHT11
  }
  if(e->posDelay()){ e->setCount(); //a cada 45seg
    //f_firebaseDHT11();
  }
  if(f->posDelay()){ f->setCount(); //a cada 100ms
//    f_SensorMovimentoPIR_v3();
  }
}
