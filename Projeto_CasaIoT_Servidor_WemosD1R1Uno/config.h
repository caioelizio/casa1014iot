/* Author: Caio Elizio - caioelizio@gmail.com 62 98223-9516"
 * Date: 20/06/20
 * Settings / Configurações
 * Versao 1.0 Servidor
 */
//Prototypes
void  configReset();
void initOutput(void);
//test void initOutput();
void initSerial();

//*defines - mapeamento de pins - WeMos D1 R1 Uno ESP-12E
#define D0    3   //RX 3.3v
#define D1    1   //SCL     TX 3.3v
#define D2    16  //SDA
#define D3    5   //SCL     D15
#define D4    4   //SDA     D14
#define D5    14  //SCK     D13
#define D6    12  //MISO ok D12
#define D7    13  //MOSI    D1  
#define D8    0   //SS RX0
#define D9    5   //TX0
#define D10   2   //SS
#define D11   15  //MOSI
#define D12   0   //MISO
#define D13   0   //SCL
#define D14   14  //SDA
#define D15   5   //SCL
//*/

//Pinagem-WeMos D1 R1 Uno ESP-12E---------------------------------
  #define pinLedonboard BUILTIN_LED //LED_BUILTIN ou BUILTIN_LED NAO FUNCIONA
  //LiquidCrystal_I2C lcd(0x3F,16,2);   //D4=SDA D3=SCL Wemos D1R1Uno LCD_Verde
  const int pinBuzzer = D5;  //sirene ok D1erro D2erro //erro usando #define
  #define pinRele1      D6   //
  #define pinDHT11      D1   //
  const int pinSensorPIR = D8; //Input for HC-SR501
//test  #define pinSensorPIR  D8
  #define pinButton     A0


//Variáveis e objetos globais--------------------------------------
  #define ON  1
  #define OFF 0
  boolean b_debug  = false;
  //boolean b_debug  = true;
  word bootCount; //Número de inicializações
//f_fade_led
  const int delayMillis = 10;
  int brightness = 0;      // how bright the LED is (0 = full, 512 = dim, 1023 = off)
  int fadeAmount = 5;      // how many points to fade the LED by
//f_timeClock
  int iTime        = 1; //1 inicia ligado
  int i_segAtivo   = 0;
  int i_minAtivo   = 0;
  int i_horasAtivo = 0;
  int i_valor      = 4825;
  long lastMsg = 0;
  int i_minutos   = 1;
  int i_horas     = 0;
  int i_minutos2  = 99;
  int i_horas2    = 99;
  int ihrsProgIni;
  int ihrsProgFin;

//Sensor PIR
  int iSensorPIR      = 0; //valor do sensor
  int i_ttl_movimento = 0;
  boolean bMovimento  = false;

#include <EEPROM.h>
  int iStatusLedOnboard = 0;
  int iStatus_Rele1 = 0;  
  int iStatus_Rele2 = 0;  
  String sStatus_Rele1  = "OFF";
  String sStatus_Rele2  = "OFF";
  String sStatusFitaLed = "OFF";
  String sStatusTime    = "OFF";


#include <ESP8266WiFi.h>
  WiFiClient espClient;
  IPAddress ipLocal;
  IPAddress ip(192,168,15,110);    //IP FIXO (ALTERE PARA FAIXA DE IP DISPONÍVEL DO SEU ROTEADOR. EX: 192.168.1.110)
  IPAddress gateway(192,168,15,1); //GATEWAY DE CONEXÃO (ALTERE PARA O GATEWAY DO SEU ROTEADOR)
  IPAddress subnet(255,255,255,0); //MASCARA DE REDE
  IPAddress apIP(192, 168, 4, 1);  //IP FIXO Web Server Wifi ESP
  bool WiFiAP = false;             // Do yo want the ESP as AP?

#include <ArduinoOTA.h>

#include <ESP8266WebServer.h>
  char    ID_esp[30];        // Identificação do dispositivo
  char    ssid[30];        // Rede WiFi
  char    pw[30];          // Senha da Rede WiFi
  char    IPfixo[15];
  char    hostnameOTA[30]; // Rede WiFi
  String  wifi_id;
  long    l_rssi;
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
  char   cRele1[15];
  char   cRele2[15];
  char   cSensores[15];
  int iSomTeclas;
  const byte  WEBSERVER_PORT          = 80; //Porta Servidor Web
  const char* WEBSERVER_HEADER_KEYS[] = {ID_esp}; //Headers do Servidor Web
  ESP8266WebServer server(WEBSERVER_PORT); //Server on port 80

#include <DNSServer.h>
  DNSServer dnsServer;
const byte  DNSSERVER_PORT = 53; //Porta Servidor DNS

#include <FS.h>
#include <ArduinoJson.h>
//test  const size_t JSON_SIZE = JSON_OBJECT_SIZE(5) + 500; //Tamanho do Objeto JSON
  const size_t JSON_SIZE = JSON_OBJECT_SIZE(5) + 1500; //Tamanho do Objeto JSON

//Definicoes Data Hora / WiFiUdp NTP time
#include <TimeLib.h>
#include <WiFiUdp.h>
  void sendNTPpacket(IPAddress &address);
  //NTP Servers:  
  time_t getNtpTime();
  WiFiUDP Udp;
  unsigned int localPort = 8888;  // local port to listen for UDP packets
// static const char ntpServerName[] = "a.st1.ntp.br"; //ok
  static const char ntpServerName[]  = "time.nist.gov";  //ok
  const int timeZone = -3;    //Brasil
  int segundos    = 0;
  int minutos     = 0;
  int horas       = 0;
  int diadasemana = 0;
  int diadomes    = 0;
  int mes         = 0;
  int ano         = 0;

#include <DHT.h>
  #define DHTTYPE DHT11
  DHT dht(pinDHT11, DHTTYPE);
  float f_temp     = 0;
  float f_humid    = 0;
  float f_tempMin  = 0;
  float f_tempMax  = 0;
  float f_humidMax = 0;
  float f_humidMin = 0;
  String s_temp      = "0";
  String s_humid     = "0";
  String s_temp_qrt  = "0";
  String s_humid_qrt = "0";
  String s_temp_sala = "0";
  String s_humid_sala= "0";
  String s_temp_lab  = "0";
  String s_humid_lab = "0";
  String s_temp_3d   = "0";
  String s_humid_3d  = "0";


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



//Funções de Configuração ------------------------------
void initOutput(void){
//test void initOutput(){  
  pinMode(pinLedonboard,  OUTPUT);  //initialize onboard LED as output
  pinMode(pinBuzzer,      OUTPUT);
  pinMode(pinRele1,       OUTPUT);
  pinMode(pinDHT11,       INPUT);
  pinMode(pinSensorPIR,   INPUT);
  digitalWrite(pinLedonboard,ON);   //#define OFF 0
  digitalWrite(pinBuzzer,    ON);
  digitalWrite(pinRele1,     OFF);  //reles começam dependendo do status salvo
  digitalWrite(pinDHT11,     ON);
//  digitalWrite(pinSensorPIR, OFF);
  digitalWrite(pinSensorPIR, ON);
}
void initSerial(){
  Serial.begin(115200); //9600 da erro, caindo conexao...
  delay(10);
  Serial.println();
  Serial.println(F("Iniciando Serial..."));
//erro  f_buzz(pinBuzzer, 1800, 50);
//erro  f_LCD_Inf_Sistema();

  Serial.println();
  Serial.println(F("Definindo pinos como INPUT ou OUTPUT..."));
//  Serial.println();
  Serial.println(F("<---------Definindo pinos como ON ou OFF---------->"));
  Serial.print(F("pinLedonboard "));                             Serial.println(F(" OFF"));
  Serial.print(F("pinBuzzer     ")); Serial.print(pinBuzzer);    Serial.println(F(" ON"));
  Serial.print(F("pinDHT11      ")); Serial.print(pinDHT11);     Serial.println(F(" OFF"));
  Serial.print(F("pinRele1      ")); Serial.print(pinRele1);     Serial.print(F(" OFF ")); Serial.println(cRele1);
  Serial.println(F("<------------------------------------------------->"));
}

void  configReset() {
  Serial.println(F("Assumindo valores da configuração padrão"));
  strlcpy(ID_esp,           "Servidor"          , sizeof(ID_esp)); 
//  bootCount  = 0;
  strlcpy(ssid,             ""                  , sizeof(ssid));
  strlcpy(pw,               ""                  , sizeof(pw)); 
//  strlcpy(ssid,             "wiff_sala"         , sizeof(ssid));
//  strlcpy(pw,               "pedroludovico"     , sizeof(pw)); 
  strlcpy(hostnameOTA,      "EspD1miniServ"     , sizeof(hostnameOTA)); 
  strlcpy(IPfixo,           "192.168.0.0"       , sizeof(IPfixo));
  //MQTT
  strlcpy(ID_MQTT,          "Serv"      , sizeof(ID_MQTT));
  //strlcpy(HOST_MQTT,      "broker.hivemq.com" , sizeof(HOST_MQTT));
  strlcpy(HOST_MQTT,        "192.168.0.0"    , sizeof(HOST_MQTT)); //broker ESP_Sala
  PORT_MQTT = 1883;
  strlcpy(USER_MQTT,        "admin"            , sizeof(USER_MQTT)); 
  strlcpy(PASS_MQTT,        "****"   , sizeof(PASS_MQTT)); 
  strlcpy(TOPICO_SUBSCRIBE, "/serv/sub", sizeof(TOPICO_SUBSCRIBE)); 
  strlcpy(TOPICO_PUBLISH,   "/serv/pub", sizeof(TOPICO_PUBLISH)); 
  strlcpy(TOPICO_PUBLISH_T, "/serv/t"  , sizeof(TOPICO_PUBLISH_T)); 
  strlcpy(TOPICO_PUBLISH_H, "/serv/h"  , sizeof(TOPICO_PUBLISH_H)); 
//
  strlcpy(cRele1,           "rele1"            , sizeof(cRele1));
  strlcpy(cRele2,           "rele2"            , sizeof(cRele2));
//  strlcpy(cSensores,        "Servidor "        , sizeof(cSensores));
  i_horas     = 0;
  i_minutos   = 0;
  ihrsProgIni = 8;
  ihrsProgFin = 17;
  iSomTeclas  = 0;
  
//Serial.print(F("bootCount: "));   Serial.println(bootCount);
  Serial.print(F("ID_esp: "));      Serial.println(ID_esp);
  Serial.print(F("ssid: "));        Serial.println(ssid);
  Serial.print(F("pw: "));          Serial.println(pw);
  Serial.print(F("hostnameOTA: ")); Serial.println(hostnameOTA);
  Serial.print(F("IPfixo: "));      Serial.println(IPfixo);
  
  Serial.print(F("ID_MQTT: "));          Serial.println(ID_MQTT);
  Serial.print(F("Broker MQTT: "));      Serial.println(HOST_MQTT);
  Serial.print(F("Broker PORT: "));      Serial.println(PORT_MQTT);
  Serial.print(F("USER_MQTT: "));        Serial.println(USER_MQTT);
  Serial.print(F("PASS_MQTT: "));        Serial.println(PASS_MQTT);
  Serial.print(F("TOPICO_PUBLISH: "));   Serial.println(TOPICO_PUBLISH);
  Serial.print(F("TOPICO_SUBSCRIBE: ")); Serial.println(TOPICO_SUBSCRIBE);
  Serial.print(F("TOPICO_PUBLISH_T: ")); Serial.println(TOPICO_PUBLISH_T);
  Serial.print(F("TOPICO_PUBLISH_H: ")); Serial.println(TOPICO_PUBLISH_H);

  Serial.print(F("Rele1: "));      Serial.println(cRele1);
  Serial.print(F("Rele2: "));      Serial.println(cRele2);
  Serial.print(F("iSomTeclas: ")); Serial.println(iSomTeclas);

  Serial.print(F("i_horas: "));    Serial.println(i_horas);
  Serial.print(F("i_minutos: "));  Serial.println(i_minutos);

  Serial.print(F("Horas programada inicio: ")); Serial.println(ihrsProgIni);
  Serial.print(F("Horas programada final:  ")); Serial.println(ihrsProgFin);
}
