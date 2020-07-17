/* Author: Caio Elizio - caioelizio@gmail.com 62 98223-9516"
 * Date: 20/06/20
 */

//defines - mapeamento de pins do NodeMCU
#define D0    16
#define D1    5  //* 3.3v
#define D2    4  //*
#define D3    0  //scl
#define D4    2  //sda
#define D5    14  //*
#define D6    12  //*
#define D7    13  //*
#define D8    15


//Pinagem--------------------------------------------------
  #define pinLedESP     LED_BUILTIN   //LED_BUILTIN ou BUILTIN_LED NAO FUNCIONA
  const int pinBuzzer = D5;  //sirene ok D1erro D2erro //erro usando #define
  #define pinRele1      D6   //vent
  #define pinRele2      D1   //luz 
  #define pinDHT11      D3   //sensor DHT temperatura e umidade
  #define pinSensorPIR  D4
  uint16_t pinIRrecv  = D2;
//  #define pinSensorLDR  A0   //Entrada do sensor LDR
  #define pinButton     A0
  #define RED   D0
  #define GREEN D7
  #define BLUE  D8


//Variáveis e objetos globais--------------------------------------
  #define ON  1
  #define OFF 0
  #define TempoAtualizacao 30
//  boolean b_debug    = false;
  boolean b_debug    = true;
  word bootCount; //Número de inicializações
  boolean bsomteclas;
  boolean ledOn;  //Estado do LED
  int icont_atualiza_sensores = 0;
  boolean bEstatisticaDHT11   = false;
//f_tempo
  int i_seg    = 0;
  int i_min    = 0;
  int i_horas  = 0;
  int i_valor  = 4825;
//Sensor PIR
  int iSensorPIR        = 0;  //Variavel para guardar valor do sensor
  int i_ttl_movimento   = 0;
  boolean movimento     = false;
//f_fade_led
  const int delayMillis = 10;
  int brightness = 0;        // how bright the LED is (0 = full, 512 = dim, 1023 = off)
  int fadeAmount = 5;        // how many points to fade the LED by
//Definicoes websever
  char    apID[30];        // Identificação do dispositivo
  char    ssid[30];        // Rede WiFi
  char    pw[30];          // Senha da Rede WiFi
  char    IPfixo[15];
  char    hostnameOTA[30]; // Rede WiFi
  String  s_idConectado;
  long    l_rssi;
//Definicoes mqtt
  char   ID_MQTT[30];           //id mqtt (para identificação de sessão) *deve ser o único no broker
  char   HOST_MQTT[30];         //URL do broker MQTT que se deseja utilizar
  int    PORT_MQTT = 1883;
  char   USER_MQTT[30];         //usuario broker
  char   PASS_MQTT[30];
  char   TOPICO_SUBSCRIBE[30];  //tópico MQTT de escuta
  char   TOPICO_PUBLISH[30];    //tópico MQTT de envio de informações para Broker

  
