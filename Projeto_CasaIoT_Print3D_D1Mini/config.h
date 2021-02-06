/* Author: Caio Elizio - caioelizio@gmail.com 62 98223-9516"
 * Date: 20/06/20
 * Settings / Configurações
 * Version 1.0 Print3D
 */ 
//Prototypes
void configReset();
//void initOutput(void);
void initOutput();
void initSerial();

//defines - mapeamento de pins - WeMos D1 mini
  //            A0 //Analog / ADC0 / Max 3.3v
  #define D0    16 //WAKE
  #define D5    14 //CLK / SCK  / SPI
  #define D6    12 //MISO / SPI
  #define D7    13 //MOSI / SPI
  #define D8    15 //SS   / SPI
  #define TX    1  //TXD0
  #define RX    3  //RXD0
  #define D1    5  //SCL I2C
  #define D2    4  //SDA I2C
  #define D3    0  //IO0 / FLASH
  #define D4    2  //IO2 / TXD1

//Pinagem-ESP-12E-WeMos-D1-mini------------------------------------
  #define pinLedonboard LED_BUILTIN   //LED_BUILTIN ou BUILTIN_LED NAO FUNCIONA
//LiquidCrystal_I2C lcd(0x27,16,2); //Esp LCD_Azul  D2=SDA D1=SCL 
  const int pinBuzzer = D5;  //sirene ok D1erro D2erro //erro usando #define
  #define pinRele1      D6   //vent
  #define pinRele2      D1   //luz 
  #define pinDHT11      D3   //sensor DHT temperatura e umidade
//test #define pinSensorPIR  0
  const int pinSensorPIR = 0; //Input for HC-SR501
//#define pinSensorLDR  A0   //Entrada do sensor LDR
//#define pinButton     A0   //Analog / ADC0
  #define pinButton     D4   //Analog / ADC0
  const int pinSensorFogo  = D8;  //Sensor Fogo

//Variáveis e objetos globais--------------------------------------
  #define ON  1
  #define OFF 0
//  boolean b_debug  = false;
  boolean b_debug  = true;
  word bootCount; //Número de inicializações
//f_fade_led
  const int delayMillis = 10;
  int brightness = 0;     // how bright the LED is (0 = full, 512 = dim, 1023 = off)
  int fadeAmount = 5;     // how many points to fade the LED by
//f_timeClock
  int iTime        = 1; //1 inicia ligado
  int i_segAtivo   = 0;
  int i_minAtivo   = 0;
  int i_horasAtivo = 0;
  int i_valor      = 4825;
  long lastMsg    = 0;
  int i_minutos   = 1;
  int i_horas     = 0;
  int i_minutos2  = 99;
  int i_horas2    = 99;
  int i_minutos_Time = 0;

//Sensor PIR
  int iSensorPIR      = 0; //Variavel para guardar valor do sensor
  int i_ttl_movimento = 0;
  boolean movimento   = false;

#include <EEPROM.h>
  int iStatusLedOnboard = 0;
  int iStatus_Rele1     = 0;
  int iStatus_Rele2     = 0;
  String sStatusTime    = "OFF";


#include <ESP8266WiFi.h>
  WiFiClient client;
  IPAddress ipLocal;
  IPAddress ip(192,168,15,115);        //IP FIXO (ALTERE PARA FAIXA DE IP DISPONÍVEL DO SEU ROTEADOR. EX: 192.168.1.110)
  IPAddress gateway(192,168,15,1);     //GATEWAY DE CONEXÃO (ALTERE PARA O GATEWAY DO SEU ROTEADOR)
  IPAddress subnet(255,255,255,0);     //MASCARA DE REDE
  IPAddress apIP(192, 168, 4, 1);      //IP FIXO Web Server Wifi ESP

#include <ArduinoOTA.h>

#include <ESP8266WebServer.h>
  char   ID_esp[30];      // Identificação do dispositivo
  char   ssid[30];        // Rede WiFi
  char   pw[30];          // Senha da Rede WiFi
  char   IPfixo[15];
  char   hostnameOTA[30]; // Rede WiFi
  String wifi_id;
  long   l_rssi;
  char cRele1[15];
  char cRele2[15];
  char cSensor[15];
  int ihrsProgMin  = 0;
  int ihrsProgMin2 = 0;
  int iSomTeclas   = 0;
  const byte  WEBSERVER_PORT          = 80;       //Porta Servidor Web
  const char* WEBSERVER_HEADER_KEYS[] = {ID_esp}; //Headers do Servidor Web
//  ESP8266WebServer server(WEBSERVER_PORT);        //Server on port 80
  ESP8266WebServer server(WEBSERVER_PORT);        //Server on port 80

#include <DNSServer.h>
  DNSServer dnsServer;
  const byte  DNSSERVER_PORT = 53; //Porta Servidor DNS

#include <FS.h>
#include <ArduinoJson.h>
  const size_t JSON_SIZE = JSON_OBJECT_SIZE(5) + 500; //Tamanho do Objeto JSON
  
//Definicoes Data Hora / WiFiUdp NTP time
#include <TimeLib.h>
#include <WiFiUdp.h>
  WiFiUDP Udp;
  void sendNTPpacket(IPAddress &address); //NTP Servers:  
  time_t getNtpTime();
  unsigned int localPort = 8888;  // local port to listen for UDP packets
  static const char ntpServerName[] = "a.st1.ntp.br"; //ok
//static const char ntpServerName[] = "time.nist.gov";  //ok
  const int timeZone = -3;    //Brasil
  int segundos    = 0;
  int minutos     = 0;
  int horas       = 0;
  int diadasemana = 0;
  int diadomes    = 0;
  int mes         = 0;
  int ano         = 0;


#include <PubSubClient.h>
  PubSubClient MQTT(client);   //Instancia o Cliente MQTT passando o objeto espClient
  //PubSubClient client(server, 1883, callback, ethClient);
//Definicoes mqtt
  char   ID_MQTT[30];           //id mqtt (para identificação de sessão) *deve ser o único no broker
  char   HOST_MQTT[30];         //URL do broker MQTT que se deseja utilizar
  int    PORT_MQTT = 1883;
//  char   PORT_MQTT[5];
  char   USER_MQTT[30];         //usuario broker
  char   PASS_MQTT[30];
  char   TOPICO_SUBSCRIBE[30];  //tópico MQTT de escuta
  char   TOPICO_PUBLISH[30];    //tópico MQTT de envio de informações para Broker
  char   TOPICO_PUBLISH_T[30];
  char   TOPICO_PUBLISH_H[30];

#include <DHT.h>
  #define DHTTYPE DHT11
  DHT dht(pinDHT11, DHTTYPE);
  float f_temp     = 0;
  float f_humid    = 0;
  float f_tempMin  = 0;
  float f_tempMax  = 0;
  float f_humidMax = 0;
  float f_humidMin = 0;

    
class Thread{
  private:
    long _delay;
    unsigned long tempo;
    long count;
  public:
    Thread(long delay);
    boolean Timer();
    boolean Refresh();
    boolean posDelay();
    int  getCount();
    void setCount();
};
Thread::Thread(long delay){
  _delay = delay;
  tempo = 0;
  count = 0;
}
boolean Thread::Timer (){
  return (tempo < millis())? true: false;
}
boolean Thread::Refresh(){
  tempo = millis() + _delay;
  return true;
}
boolean Thread::posDelay(){
  return (Timer()== true)? Refresh(): false;
}
int Thread::getCount(){
  return count;
}
void Thread::setCount(){
  count++;
}
Thread *a, *b, *c, *d, *e, *f;
long contador;


  //void initOutput(void){
void initOutput(){
  pinMode(pinLedonboard, OUTPUT);  //initialize onboard LED as output
  pinMode(pinBuzzer,     OUTPUT);
  pinMode(pinRele1,      OUTPUT);
  pinMode(pinRele2,      OUTPUT);
  pinMode(pinDHT11,      INPUT);
//  pinMode(pinSensorFogo, INPUT);
//  pinMode(pinSensorPIR,  INPUT);
  pinMode(pinButton,     INPUT);
  digitalWrite(pinLedonboard,     ON); //  #define OFF 0
  digitalWrite(pinBuzzer,         ON);
  digitalWrite(pinRele1,          OFF);   //Modulo rele duplo invertido //reles começam ligados
  digitalWrite(pinRele2,          OFF);   //Modulo rele duplo invertido //reles começam ligados
  digitalWrite(pinDHT11,          ON);
  digitalWrite(pinButton,         OFF);
//digitalWrite(pinSensorFogo,     OFF);
//  digitalWrite(pinSensorFogo,     ON);
//  digitalWrite(pinSensorPIR,      ON);

//  iStatusLedOnboard = EEPROM_Read(13);  //ler status salvo na memoria
//  iStatus_Rele1     = EEPROM_Read(20);  //ler status salvo na memoria / value: pinRele1
//  iStatus_Rele2     = EEPROM_Read(21);  //ler status salvo na memoria / value: pinRele1
}
void initSerial(){
  Serial.begin(115200); //9600 da erro, caindo conexao...
  delay(10);
  Serial.println();
  Serial.println(F("Iniciando Serial..."));  
//erro  f_Serial_Sistema();
//erro  f_buzz(pinBuzzer, 1800, 50);

  Serial.println();
  Serial.println(F("Definindo pinos como INPUT ou OUTPUT..."));
  Serial.println();
  Serial.println(F("<---------Definindo pinos como ON ou OFF---------->"));
  Serial.print(F("pinLedonboard "));                              Serial.println(F(" OFF"));
  Serial.print(F("pinBuzzer     ")); Serial.print(pinBuzzer);     Serial.println(F(" ON"));
  Serial.print(F("pinDHT11      ")); Serial.print(pinDHT11);      Serial.println(F(" OFF"));
  Serial.print(F("pinSensorFogo ")); Serial.print(pinSensorFogo); Serial.println(F(" OFF"));
//Serial.print(F("pinSensorPIR  ")); Serial.print(pinSensorPIR);  Serial.println(F(" ON"));
  Serial.print(F("pinRele1      ")); Serial.print(pinRele1);      Serial.print(F(" OFF ")); Serial.println(cRele1);
  Serial.print(F("pinRele2      ")); Serial.print(pinRele2);      Serial.print(F(" OFF ")); Serial.println(cRele2);
  Serial.println(F("<------------------------------------------------->"));
}
//Funções de Configuração ------------------------------
void configReset() {
  Serial.println(F("Assumindo valores da configuração padrão"));
//  bootCount  = 0;
  strlcpy(ID_esp,           "Print3D"          , sizeof(ID_esp)); 
  strlcpy(ssid,             "x"                , sizeof(ssid));
  strlcpy(pw,               "***"              , sizeof(pw)); 
  strlcpy(hostnameOTA,      "EspD1miniPrint3D" , sizeof(hostnameOTA)); 
  strlcpy(IPfixo,           "192.168.15.115"   , sizeof(IPfixo));
  strlcpy(cRele1,           "Fita Led "        , sizeof(cRele1)); 
  strlcpy(cRele2,           " "                , sizeof(cRele2)); 
  strlcpy(cSensor,          "Temp./Umid."      , sizeof(cSensor));
  iSomTeclas = 0;
  ihrsProgMin = 5;
  i_horas     = 0;
  i_minutos   = 0;
//MQTT
  strlcpy(ID_MQTT,          "Print3D"             , sizeof(ID_MQTT));
  //strlcpy(HOST_MQTT,      "broker.hivemq.com"   , sizeof(HOST_MQTT)); 
  strlcpy(HOST_MQTT,        "mqtt.thingspeak.com" , sizeof(HOST_MQTT)); 
  //strlcpy(HOST_MQTT,      "192.168.15.110"      , sizeof(HOST_MQTT)); //broker ESP_Sala
  PORT_MQTT = 1883;
  strlcpy(USER_MQTT,        "admin"               , sizeof(USER_MQTT)); 
  strlcpy(PASS_MQTT,        "***"                 , sizeof(PASS_MQTT)); 
  strlcpy(TOPICO_SUBSCRIBE, "/qrt/sub" , sizeof(TOPICO_SUBSCRIBE)); 
  strlcpy(TOPICO_PUBLISH,   "/qrt/pub" , sizeof(TOPICO_PUBLISH)); 
  strlcpy(TOPICO_PUBLISH_T, "/qrt/t"   , sizeof(TOPICO_PUBLISH_T)); 
  strlcpy(TOPICO_PUBLISH_H, "/qrt/h"   , sizeof(TOPICO_PUBLISH_H)); 
}
