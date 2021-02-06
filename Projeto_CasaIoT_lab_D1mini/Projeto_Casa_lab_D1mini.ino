/*********************************************************************/
  #define autor       "Caio Elizio - caioelizio@gmail.com 62 98223-9516"
  #define programa    "Casa iot Lab "
  #define versao      "v1.13.1"
  #define dataversao  "17/06/19"
  #define placa       "ESP-12E WeMos D1 mini"
  #define cRele1      "Vent. Lab "
  #define cRele2      "Luz portao "
  #define cSensores   "Sensores Lab"
/*********************************************************************/
//Definicoes programacao por serial
//Definicoes websever
//Definicoes IRrecv
//Definicoes Sensor DHT11
/*Lista de Peças: 
 * 1 ESP-12E WeMos D1 mini
 * 1 Fonte 5v (carregador de celular)
 * 1 buzzer
 * 1 Sensor Temperatura Umidade DHT11
 * 1 Remoto IR
 * 1 Sensor PIR
 * 1 controle remoto creative
 * 1 P8 modulo tensao
*/
/*Versoes
  v1.0 ...
  v1.10 ...
  v1.11 ...
  v1.14 Web sever Json / SPIFFS
*/
  #include "config.h"


#include <EEPROM.h>
  int iStatusLedOnboard = 0;
  int iStatus_Rele1     = 0;
  int iStatus_Rele2     = 0;
  String sStatus_Rele1  = "OFF";
  String sStatus_Rele2  = "OFF";
//f_timeluzportao
  int iTimeLuzPortao    = 1; //1 inicia ligado
  int iStatusTimeLuz    = 0;
  String sStatusTimeLuz = "OFF";
  int i_horas_programada_ini = 18;
  int i_horas_programada_fin = 6;


//Definicoes wifi
#include <ESP8266WiFi.h>
  WiFiClient espClient;
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
//Instâncias -------------------------------------------
  IPAddress ipLocal;
  IPAddress ip(000,000,000,000);        //IP FIXO (ALTERE PARA FAIXA DE IP DISPONÍVEL DO SEU ROTEADOR. EX: 192.168.1.2)
  IPAddress gateway(192,168,0,1);     //GATEWAY DE CONEXÃO (ALTERE PARA O GATEWAY DO SEU ROTEADOR)
  IPAddress subnet(255,255,255,0);     //MASCARA DE REDE
  IPAddress apIP(192, 168, 4, 1);     //IP FIXO Web Server Wifi ESP
// Constantes -------------------------------------------
const byte        WEBSERVER_PORT          = 80; //Porta Servidor Web
const byte        DNSSERVER_PORT          = 53; //Porta Servidor DNS
const char*       WEBSERVER_HEADER_KEYS[] = {"ESP_Lab"}; //Headers do Servidor Web

  
#include <FS.h>
#include <DNSServer.h>
#include <ArduinoJson.h>
//#include "FS.h"
// Tamanho do Objeto JSON
  const   size_t    JSON_SIZE = JSON_OBJECT_SIZE(5) + 130;
  ESP8266WebServer server(WEBSERVER_PORT); //Server on port 80
  DNSServer         dnsServer;


#include <PubSubClient.h>
  PubSubClient MQTT(espClient);   //Instancia o Cliente MQTT passando o objeto espClient
  //PubSubClient client(server, 1883, callback, ethClient);


//Definicoes Sensor DHT11
#include <DHT.h>
  #define DHTTYPE DHT11 
  DHT dht(pinDHT11, DHTTYPE);
  float f_temp     = 0;
  float f_humid    = 0;
  float f_tempMin  = 0;
  float f_tempMax  = 0;
  float f_humidMax = 0;
  float f_humidMin = 0;  


//Definicoes Data Hora / WiFiUdp NTP time
#include <TimeLib.h>
#include <WiFiUdp.h>
//  void sendNTPpacket(IPAddress &address);
  //NTP Servers:  
  time_t getNtpTime();
  WiFiUDP Udp;
  unsigned int localPort = 8888;  // local port to listen for UDP packets
//  static const char ntpServerName[] = "a.st1.ntp.br"; //ok
  static const char ntpServerName[] = "time.nist.gov";  //ok
  //static const char ntpServerName[] = "pool.ntp.org";
  //static const char ntpServerName[] = "us.pool.ntp.org";
  const int timeZone = -3;    //Brasil
  //const int timeZone = 1;   //Central European Time
  //const int timeZone = -8;  //Pacific Standard Time (USA)
  int segundos    = 0;
  int minutos     = 0;
  int i_minutos   = 1;
  int horas       = 0;
  int diadasemana = 0;
  int diadomes    = 0;
  int mes         = 0;
  int ano         = 0;


//Definicoes IRrecv
#include <IRremoteESP8266.h>
#include <IRrecv.h>
  IRrecv irrecv(pinIRrecv);
  decode_results results;
  boolean btecla1    = false;
  boolean btecla2    = false;
  boolean btecla3    = false;
  boolean btecla4    = false;
  boolean btecla5    = false;
  boolean btecla6    = false;
  boolean btecla9    = false;

void initSerial();
void initOutput(void);
void f_Controle_Serial();

void f_atualiza_sensores();
void f_WifiAP();


//Funções Genéricas--------------------------------------
  #include "funcoes.h"


void initSerial(){
  Serial.begin(115200); //9600 da erro, caindo conexao...
  delay(10);
  Serial.println(); //Linha em branco
  Serial.println(F("Iniciando Serial..."));
  Serial.println(); //Linha em branco
  Serial.print(F("Autor:       ")); Serial.println(autor);
  Serial.print(F("Sistema:     ")); Serial.print(programa); Serial.println(versao);
  Serial.print(F("Data versao: ")); Serial.println(dataversao);
//  buzz(pinBuzzer, 1800, 50);   //erro nessa linha
}
void initOutput(void){
  pinMode(pinLedESP,    OUTPUT);  //initialize onboard LED as output
  pinMode(pinBuzzer,    OUTPUT);
  pinMode(pinDHT11,     INPUT);
  pinMode(pinSensorPIR, INPUT);
  pinMode(pinRele1,     OUTPUT);
  pinMode(pinRele2,     OUTPUT);
  pinMode(RED,          OUTPUT);
  pinMode(GREEN,        OUTPUT);
  pinMode(BLUE,         OUTPUT);

  Serial.println(); //Linha em branco
  iStatusLedOnboard = EEPROM_Read(13);  //ler status salvo na memoria
  Serial.print(F("iStatusLedOnboard: "));  Serial.println(iStatusLedOnboard);
  iStatus_Rele1     = EEPROM_Read(20);  //ler status salvo na memoria / value: pinRele1
  Serial.print(F("iStatus_Rele1: "));      Serial.println(iStatus_Rele1);
  iStatus_Rele2     = EEPROM_Read(21);  //ler status salvo na memoria / value: pinRele1
  Serial.print(F("iStatus_Rele2: "));      Serial.println(iStatus_Rele2);
  
  Serial.println(); //Linha em branco
  Serial.println(F("Definindo pinos como ON ou OFF"));
  digitalWrite(pinLedESP,         OFF); //  #define OFF 0
  digitalWrite(pinBuzzer,         ON);
  digitalWrite(pinDHT11,          OFF);
  digitalWrite(pinSensorPIR,      ON);
  digitalWrite(pinRele1,          iStatus_Rele1);   //reles começam dependendo do status salvo
  digitalWrite(pinRele2,          ON);   //Modulo rele invertido //reles começam desligados
  digitalWrite(RED ,              OFF);
  digitalWrite(GREEN ,            OFF);
  digitalWrite(BLUE ,             OFF);

  Serial.print(F("pinLedESP             "));  Serial.print(pinLedESP);    Serial.println(F("  OFF"));
  Serial.print(F("pinBuzzer             "));  Serial.print(pinBuzzer);    Serial.println(F(" ON"));
  Serial.print(F("pinDHT11              "));  Serial.print(pinDHT11);     Serial.println(F("  OFF"));
  Serial.print(F("pinSensorPIR          "));  Serial.print(pinSensorPIR); Serial.println(F("  ON"));
  Serial.print(F("pinRele1              "));  Serial.print(cRele1);       Serial.println(F(" OFF"));
  Serial.print(F("pinRele2              "));  Serial.print(cRele2);       Serial.println(F(" OFF"));
}


//Definicoes programacao por serial
void f_Controle_Serial(){
  char c_LeituraSerial = ' ';
  String s_leitura = ""; //testar
  int inByte2      = 0;  //incoming serial byte
  if (Serial.available() > 0){ //if we get a valid byte, read analog ins:
    c_LeituraSerial = Serial.read();
    inByte2         = Serial.read();
    Serial.print(F("Leitura serial: ")); Serial.println(c_LeituraSerial);   //Imprimir dados da serial
    if (c_LeituraSerial == 'L' || c_LeituraSerial == 'l'){
      f_LedESP(ON);
    };
    if (c_LeituraSerial == 'D' || c_LeituraSerial == 'd'){
      Serial.println(F("Estatistica DHT11 desligada"));
      bEstatisticaDHT11 = false;
      f_LedESP(OFF);
    };
    if (c_LeituraSerial == 'E' || c_LeituraSerial == 'e'){
      Serial.println(F("Estatistica DHT11 ligada"));
      bEstatisticaDHT11 = true;
    };
    if (c_LeituraSerial == 'I' 
     || c_LeituraSerial == 'i'
     || c_LeituraSerial == '?' ){
      Serial.println("Digite L para liga led esp");
      Serial.println("Digite D para desliga led esp");
      Serial.println("Digite E para ver estatistida DTH11");
      Serial.println("Digite I, ? Informacoes MQTT");
    };
//erro    else{
//      Serial.println( " Tecla errada. Digite L, D, E, I");
//    };
  };
}


void f_WifiAP() {
//WiFi AP Access Point
  WiFi.hostname(apID);
  WiFi.softAP(apID, pw);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  log("");
  log("*********************************************************");
  log("WiFi AP: " + String(apID) + " - IP " + ipStr(WiFi.softAPIP()));
  log("Conecte-se nesta rede para acessar a configuração da ESP");
  log("Password igual da rede Wifi");
  log("*********************************************************");
  log("");
  
  // Habilita roteamento DNS
  dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
  dnsServer.start(DNSSERVER_PORT, "*", apIP);
}


int  i_contTentWifi = 0;
void initWiFi(){
  ++i_contTentWifi;
  delay(10);
  Serial.println(); //Linha em branco
  Serial.print  (F("Conectando a rede WI-FI: "));  Serial.println(ssid);
  Serial.println(F("Aguarde..."));
//  WiFi.mode(WIFI_STA);            //OTA
  WiFi.begin(ssid, pw);             //Conecta na rede WI-FI
  WiFi.config(ip, gateway, subnet); //PASSA OS PARÂMETROS PARA A FUNÇÃO QUE VAI SETAR O IP FIXO NO NODEMCU
  s_idConectado = WiFi.SSID();
  l_rssi        = WiFi.RSSI();
  ipLocal       = WiFi.localIP();
  if (i_contTentWifi == 10){
    i_contTentWifi = 0;
    ESP.restart();
  }
  reconectWiFi();
  printWifiStatus();
}
void reconectWiFi(){
  int cont = 0;
  if (WiFi.status() == WL_CONNECTED) return;
  Serial.print(F("ERRO ao conectando WI-FI: ")); Serial.println(s_idConectado);
  while (WiFi.status() != WL_CONNECTED) {
      ++cont;
      Serial.print(F("."));
      if (cont == 50){
        Serial.print(F("Numero de Tentativas: ")); Serial.println(i_contTentWifi);
        initWiFi();
      }      
      delay(100);
  }
  if (WiFi.status() == WL_CONNECT_FAILED){
    Serial.println(F("Senha incorreta"));
  }
/*      if (WiFi.status() == WL_NO_SSID_AVAILin){
    Serial.println(F("SSID configurado não possa ser alcançado"));
  }//*/
}
void printWifiStatus(){
  Serial.println(); //Linha em branco
  Serial.print(F("Conectado com sucesso WI-FI: ")); Serial.println(s_idConectado);
  Serial.print(F("IP Local: "));                    Serial.println(ipStr(WiFi.localIP()));
  Serial.print(F("Força do sinal (RSSI):       ")); Serial.print(l_rssi);         Serial.println(F(" dBm"));
  Serial.print(F("Tempo para conectar na rede: ")); Serial.print(millis()/1000);  Serial.println(F(" segundos"));
//erro  MQTT.publish("casa1014/qrt/ip", ipStr(ipLocal));
  MQTT.publish("casa1014/qrt/ip", IPfixo);
}
void initOTA(){
  ArduinoOTA.setPort(8266); //Port defaults to 8266
  ArduinoOTA.setHostname(hostnameOTA);
  ArduinoOTA.setPassword((const char *)pw);  //OPTIONAL
  Serial.println(); //Linha em branco
  ArduinoOTA.onStart([]() { Serial.println(F("Start")); });
  ArduinoOTA.onEnd([]()   { Serial.println(F("\nEnd")); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total){
    Serial.printf("ProgFailed to read from DHT senress: %u%%\r"), (progress / (total / 100));
    //erro printf(F(
  });
  ArduinoOTA.onError([](ota_error_t error){
    Serial.println(); //Linha em branco
    Serial.printf("Error[%u]: ", error); //erro printf(F(
    Serial.println(F("erro - 2 bip na sirene"));
    if      (error == OTA_AUTH_ERROR)    Serial.println(F("Auth Failed"));
    else if (error == OTA_BEGIN_ERROR)   Serial.println(F("Begin Failed"));
    else if (error == OTA_CONNECT_ERROR) Serial.println(F("Connect Failed"));
    else if (error == OTA_RECEIVE_ERROR) Serial.println(F("Receive Failed"));
    else if (error == OTA_END_ERROR)     Serial.println(F("End Failed"));
  });
  ArduinoOTA.begin();
  Serial.println(); //Linha em branco
  Serial.println(F("Iniciando OTA..."));
  Serial.print(F("Hostname OTA: ")); Serial.println(hostnameOTA);
}
void initUDP(){
  Serial.println(); //Linha em branco
  Serial.println(F("Iniciando UDP...")); 
  Udp.begin(localPort);
  Serial.print(F("Local port: "));  Serial.println(Udp.localPort());
  Serial.println(F("waiting for sync"));
  setSyncProvider(getNtpTime);
  setSyncInterval(300);
}


//Definicoes mqtt
//Função: inicializa conexão MQTT(endereço do broker, porta e seta função de callback)
void initMQTT(){
    MQTT.setServer(HOST_MQTT, PORT_MQTT); //informa qual broker e porta deve ser conectado
    MQTT.setCallback(mqtt_callback);          //atribui função de callback (função chamada quando qualquer 
                                              //informação de um dos tópicos subescritos chega)
}
//Função: reconecta-se ao broker MQTT (caso ainda não esteja conectado ou em caso de a conexão cair)
//        em caso de sucesso na conexão ou reconexão, o subscribe dos tópicos é refeito.
long l_timeMQTT = 0;
int  i_contTentMQTT = 0;
void reconnectMQTT(){
//    if (MQTT.connect(ID_MQTT)){
  if (MQTT.connect(ID_MQTT,USER_MQTT,PASS_MQTT)){
    Serial.println(); //Linha em branco
    Serial.println(F("Cotectado Broker MQTT"));
    Serial.print(F("Broker MQTT: "));      Serial.println(HOST_MQTT);
    Serial.print(F("Broker PORT: "));      Serial.println(PORT_MQTT);
    Serial.print(F("TOPICO_SUBSCRIBE: ")); Serial.println(TOPICO_SUBSCRIBE);
    Serial.print(F("TOPICO_PUBLISH: "));   Serial.println(TOPICO_PUBLISH);
    MQTT.subscribe(TOPICO_SUBSCRIBE); 
    MQTT.publish(TOPICO_PUBLISH, "Cotectado Broker MQTT");
  } else {
    long now = millis()/1000;
    if (now - l_timeMQTT > 3){
      f_piscaLedESP(3);
      l_timeMQTT = now;
      ++i_contTentMQTT;
      Serial.println(); //Linha em branco
      Serial.println(F("Falha ao reconectar no broker... reconectando em 3 seg."));
      Serial.print(F("Numero de Tentativas: ")); Serial.println(i_contTentMQTT);
      MQTT.publish(TOPICO_PUBLISH, "erro broker");
    }
  }
}
//Função: verifica o estado das conexões WiFI e ao broker MQTT. 
void VerificaConexoesWiFIEMQTT(void){
    if (!MQTT.connected()) reconnectMQTT(); //se não há conexão com o Broker, a conexão é refeita
    reconectWiFi();                         //se não há conexão com o WiFI, a conexão é refeita
}
//Função: função de callback é chamada toda vez que uma informação de um dos tópicos subescritos chega
void mqtt_callback(char* topic, byte* payload, unsigned int length){
    String msgRecebida;
    for(int i = 0; i < length; i++){ //obtem a string do payload recebido
       char c = (char)payload[i];
       msgRecebida += c;
    }
    Serial.println(); //Linha em branco
    Serial.println(F("<----------------------MQTT----------------------->"));  
    Serial.print(F(  "topic:       ")); Serial.println(topic);
    Serial.print(F(  "msgRecebida: ")); Serial.println(msgRecebida);
    Serial.println(F("<------------------------------------------------->"));

//Definicoes mqtt
//toma ação dependendo da string recebida:
    if (msgRecebida == "somteclason"){
      f_AtivaSomTeclas(ON);
    }
    if (msgRecebida == "somteclasoff"){
      f_AtivaSomTeclas(OFF);
    }
    if (msgRecebida == "venton"){
      rele1(ON);
      f_somteclas();
    }
    if (msgRecebida == "ventoff"){
      rele1(OFF);
      f_somteclas();
    }
    if (msgRecebida == "luzon"){
      rele2(ON);
      f_somteclas();
    }
    if (msgRecebida == "luzoff"){
      rele2(OFF);
      f_somteclas();
    }
    if (msgRecebida == "timeon"){
      f_timeluzportao(ON);
      f_somteclas();
    }
    if (msgRecebida == "timeoff"){
      f_timeluzportao(OFF);
      f_somteclas();
    }
    if (msgRecebida == "tudoff"){
      tudoff();
      f_somteclas();
    }

    if (msgRecebida == "t"){
      f_temp  = 1;
    }
    if (msgRecebida == "h"){
      f_humid = 1;
    }
} //void mqtt_callback(


void button(){ //Apaga o led enquanto precionado o botao
  int estadoBotao = digitalRead(pinButton);
  if (estadoBotao == 0){         //se estado do botão on...
    Serial.println();  //Linha em branco
    Serial.print(F("Botao: ")); Serial.println(estadoBotao); 
//    f_LedESP(ON);
  } else {
//    Serial.print(F("Botao1 ")); Serial.print(F("OFF")); 
//    f_LedESP(OFF);
    //sirene();
  }
}


void rele1(boolean b_valor){
  if (b_valor == ON){
    EEPROM_Write(20,1);  //bStatusR1 = 1
    sStatus_Rele1 = "ON";
    iStatus_Rele1 = ON;
//    Serial.println(); //Linha em branco
    Serial.print(cRele1); Serial.println(sStatus_Rele1);
//    digitalWrite(pinRele1,  ON); //Ventilador
    digitalWrite(pinRele1,  OFF);  //Ventilador //Modulo rele invertido
    f_LedESP(ON);
    MQTT.publish(TOPICO_PUBLISH, "venton");
  } else {
    EEPROM_Write(20,0);
    sStatus_Rele1 = "OFF";
    iStatus_Rele1 = OFF;
//    Serial.println(); //Linha em branco
    Serial.print(cRele1); Serial.println(sStatus_Rele1);
//    digitalWrite(pinRele1,  OFF); //Ventilador
    digitalWrite(pinRele1,  ON);    //Ventilador //Modulo com 2 rele invertido
    f_LedESP(OFF);
    MQTT.publish(TOPICO_PUBLISH, "ventoff");
  }
}
void rele2(boolean b_valor){
  if (b_valor == ON){
    EEPROM_Write(21,1);
    iStatus_Rele2 = ON;
    Serial.print(cRele2); Serial.println("ON"); //Luz portao
    //Alterado a posicao dos pins para usar o modulo com 2 reles
    //  digitalWrite(pinRele2,  ON);  //Luz
    digitalWrite(pinRele2,  OFF);     //Luz //Modulo com 2 rele invertido
    f_LedESP(ON);
    MQTT.publish(TOPICO_PUBLISH, "luzon");
  } else {
    EEPROM_Write(21,0);
    iStatus_Rele2 = OFF;
    Serial.print(cRele2); Serial.println("OFF");
//    digitalWrite(pinRele2,  OFF);  //Luz
    digitalWrite(pinRele2,  ON);     //Luz //Modulo rele invertido
    f_LedESP(OFF);
    MQTT.publish(TOPICO_PUBLISH, "luzoff");
  }
}
void tudoff(){
  Serial.println(); //Linha em branco
  Serial.println(F("Tudo off"));
  rele1(OFF);
  rele2(OFF);
  f_AtivaSomTeclas(OFF);
//  f_LedESP(OFF);
  MQTT.publish(TOPICO_PUBLISH, "tudoff");
}


void f_TimeRele1(){
    //se n houver movimento em 30min desl. rele1 ventilador
    if ((horas <= 10) or (horas >= 18)) {
//      Serial.println(F("Horario em q é proavel esta em casa"));
//      EEPROM_Write(2,1);  //timeVent = 1
    }else{
//      Serial.println(F("off"));
//      EEPROM_Write(2,0);  //timeVent = 0
    }
}
void f_TimeRele2(){
    //se n houver movimento em 30min desl. rele1 ventilador
    if ((horas <= 10) or (horas >= 18)) {
//      Serial.println(F("Horario em q é proavel esta em casa"));
//      EEPROM_Write(2,1);  //timeVent = 1
    }else{
//      Serial.println(F("off"));
//      EEPROM_Write(2,0);  //timeVent = 0
    }
}
void f_timeluzportao(boolean b_valor){
  if (b_valor == ON){
    sStatusTimeLuz = "ON";
    iTimeLuzPortao = 1;
    EEPROM_Write(32,iTimeLuzPortao);
    Serial.println(F("Time portao: ON"));
    Serial.print(F("Hora programada inicial: ")); Serial.println(i_horas_programada_ini);
    Serial.print(F("Hora programada final  : ")); Serial.println(i_horas_programada_fin);
    MQTT.publish(TOPICO_PUBLISH, "timelabon");
  }else{
    sStatusTimeLuz = "OFF";
    iTimeLuzPortao = 0;
    EEPROM_Write(32,iTimeLuzPortao);
    Serial.println(F("Time portao: OFF"));
    MQTT.publish(TOPICO_PUBLISH, "timelaboff");
  }
}
void f_timeluzportao_atualiza(){
  if (iTimeLuzPortao = 1){
//    Serial.println();  //Linha em branco
    Serial.println(F("Time Luz portao ON"));
    if (ano == 1970){ //erro data
  //    rele2(OFF);
      Serial.println(F("Reconectar timer"));
      initUDP();
      horas = EEPROM_Read(1);   //ler horas salva na memoria
      horas = EEPROM_Read(2);
    }else{
      EEPROM_Write(1,horas);   //salvar hora na memoria
      EEPROM_Write(2,minutos);
    }
    if ( (horas >= i_horas_programada_ini) 
    or   (horas < i_horas_programada_fin) ) { //Luz portão
      rele2(ON);
    }else {
      rele2(OFF);
    }
  }else{
    Serial.println(F("Time Luz portao OFF"));
  }
  if (b_debug == true){
    Serial.print(F("horas: ")); Serial.println(horas);
  }
}


//Definicoes IRrecv
void iniSensorIRrecv(){
//  irsend.begin();
  irrecv.enableIRIn(); //Inicializa o receptor IR      
  Serial.println();  //Linha em branco
  Serial.println(F("Iniciando receptor IR..."));
  Serial.println(F("Comandos IR:"));
  Serial.println(F("T1 Rele1 - Vent Lab on/off"));
  Serial.println(F("T2 Rele2 - Luz  Lab on/off"));
//  Serial.println(F("T4 Rele2 - Luz  Sala on"));
//  Serial.println(F("T5 Rele2 - Luz  Sala off"));
  Serial.println(F("T4 Rele2 - Luz Portao Lab on"));
  Serial.println(F("T5 Rele2 - Luz Portao Lab off"));  
}
void f_LerSensorIRrecv(){
  float valor_IR;  
//se receber sinal IR
  if (irrecv.decode(&results)) {
    f_LedESP(ON);
    valor_IR = (results.value);  
    if (valor_IR != 0){ Serial.print(F("Valor lido IR HEX: ")); Serial.println(valor_IR , HEX); }
//    if (valor_IR != 0){ Serial.print(F("Valor lido IR DEC: ")); Serial.println(valor_IR , DEC); }
    // || valor_IR == 2218068992) //4controle creative mini
    // || valor_IR == 2218069745) //5controle creative mini
    // || valor_IR == 2218070000) //6controle creative mini
    // || valor_IR == 2218070528) //7controle creative mini
    // || valor_IR == 2218070765) //8controle creative mini
    // || valor_IR == 2218071040) //9controle creative mini
 
    // || valor_IR == 2218068480) //1controle creative mini
     //Rele Vent. Lab
    if (valor_IR == 2218068470){  //?controle creative mini
      rele1(ON);
    }  
    if (valor_IR == 2218068725){  //2controle creative mini
      rele1(OFF);
    }  
    //Rele Luz Portao
    if (valor_IR == 2218068992){  //4controle creative mini
      rele2(ON);
    }  
    if (valor_IR == 2218069745){  //5controle creative mini
      rele2(OFF);
    }
    if (valor_IR == 2218071040){  //9controle creative mini
      tudoff();
    }  

//  piscaLed(1);
  irrecv.resume(); //Le o próximo valor  
  }
//  f_LedESP(OFF)
}  //void LerSensorIRrecv(){


//Definicoes websever
void f_Websever() {
  if (!SPIFFS.begin()) {
    log(F("SPIFFS ERRO"));
    while (true);
  }
  configRead(); //Lê configuração Json
  bootCount++; //Incrementa contador de inicializações
  Serial.print(F("N. Reset: ")); Serial.println(bootCount);
  configSave(); //Salva configuração Json
  SetSomteclas();

  // WebServer
  server.on(F("/dashboard")  , handleDashboard);
  server.on(F("/info")       , handleInfo);
  server.on(F("/config")     , handleConfig);
  server.on(F("/configSave") , handleConfigSave);
  server.on(F("/rele1on")    , handleRele1on);
  server.on(F("/rele1off")   , handleRele1off);
  server.on(F("/rele2on")    , handleRele2on);
  server.on(F("/rele2off")   , handleRele2off);
  server.on(F("/timeluzon")  , handleTimeluzon);
  server.on(F("/timeluzoff") , handleTimeluzoff);
  server.on(F("/tudoff")     , handleTudoff);
  server.on(F("/reconfig")   , handleReconfig);
  server.on(F("/reboot")     , handleReboot);
  server.onNotFound(handleDashboard); //pagina default
  server.collectHeaders(WEBSERVER_HEADER_KEYS, 1);
  server.begin();
  log(F("Pronto"));
}


//Funções Dashboard ------------------------------------
void handleRele1on(){
  rele1(ON);
  server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>history.back()</script></html>"));
}
void handleRele1off(){
  rele1(OFF);
  server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>history.back()</script></html>"));
}
void handleRele2on(){
  rele2(ON);
  server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>history.back()</script></html>"));
}
void handleRele2off(){
  rele2(OFF);
  server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>history.back()</script></html>"));
}
void handleTimeluzon(){
  f_timeluzportao(ON);
  server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>history.back()</script></html>"));
}
void handleTimeluzoff(){
  f_timeluzportao(OFF);
  server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>history.back()</script></html>"));
}
void handleTudoff(){
  rele1(OFF);
  rele2(OFF);
//  f_timeluzportao(OFF);
//somteclas off
  server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>history.back()</script></html>"));
}


//Requisições Web --------------------------------------
void handleDashboard() {
  // Config
  File file = SPIFFS.open(F("/Dashboard.htm"), "r");
  if (file) {
    file.setTimeout(100);
    String s = file.readString();
    file.close();
    log(F("\nAtualiza conteúdo dinâmico Dashboard.htm"));
    s.replace(F("#apID#")          , apID);
    s.replace(F("#Rele1#")         , cRele1);
    s.replace(F("#Rele2#")         , cRele2);
    s.replace(F("#StatusRele1#")   , sStatus_Rele1);
    s.replace(F("#StatusRele2#")   , sStatus_Rele2);
    s.replace(F("#StatusTimeLuz#") , sStatusTimeLuz);
    s.replace(F("#Sensores#")      , cSensores);
    s.replace(F("#f_temp#")        , String(f_temp));
    s.replace(F("#f_humid#")       , String(f_humid));
    
    server.send(200, F("text/html"), s); //Send data //Envia dados
    log("Dashboard: " + ipStr(server.client().remoteIP()));
  } else {
    server.send(500, F("text/plain"), F("Dashboard - ERROR 500"));
    log(F("Dashboard - ERRO lendo arquivo"));
  }
}
void handleInfo() {
  File file = SPIFFS.open(F("/Info.htm"), "r");
  if (file) {
    file.setTimeout(100);
    String s = file.readString();
    file.close();
    log(F("\nAtualiza conteúdo dinâmico Info.htm"));
    s.replace(F("#bootCount#"), String(bootCount));
    s.replace(F("#apID#")     , apID);
    s.replace(F("#led#")      , bsomteclas ? F("Ligado") : F("Desligado"));
    s.replace(F("#serial#")   , hexStr(ESP.getChipId()));
    s.replace(F("#software#") , softwareStr());
    s.replace(F("#versao#")   , versao);
    s.replace(F("#sysIP#")    , ipStr(WiFi.status() == WL_CONNECTED ? WiFi.localIP() : WiFi.softAPIP()));
    s.replace(F("#clientIP#") , ipStr(server.client().remoteIP()));
    s.replace(F("#active#")   , longTimeStr(millis() / 1000));
    s.replace(F("#userAgent#"), server.header(F("User-Agent")));
    s.replace(F("#autor#")    , autor);
    
    server.send(200, F("text/html"), s); //Send data //Envia dados
    log("Info - Cliente: " + ipStr(server.client().remoteIP()) +
        (server.uri() != "/" ? " [" + server.uri() + "]" : ""));
  } else {
    server.send(500, F("text/plain"), F("ERROR 500"));
    log(F("Info - ERRO lendo arquivo"));
  }
}
void handleConfig() {
  // Config
  File file = SPIFFS.open(F("/Config.htm"), "r");
  if (file) {
    file.setTimeout(100);
    String s = file.readString();
    file.close();
    log(F("\nAtualiza conteúdo dinâmico Config.htm"));
    s.replace(F("#apID#")             , apID);
    s.replace(F("#ssid#")             , ssid);
    s.replace(F("#pw#")               , pw);
    s.replace(F("#somteclason#")     ,  bsomteclas ? " checked" : "");
    s.replace(F("#somteclasoff#")    , !bsomteclas ? " checked" : "");
    s.replace(F("#hostnameOTA#")      , hostnameOTA);    
    s.replace(F("#IPfixo#")           , IPfixo);
    s.replace(F("#ID_MQTT#")          , ID_MQTT);    
    s.replace(F("#HOST_MQTT#") , HOST_MQTT);
    s.replace(F("#PORT_MQTT#") , String(PORT_MQTT));
    s.replace(F("#USER_MQTT#")        , USER_MQTT);
    s.replace(F("#PASS_MQTT#")        , PASS_MQTT);
    s.replace(F("#TOPICO_SUBSCRIBE#") , TOPICO_SUBSCRIBE);
    s.replace(F("#TOPICO_PUBLISH#")   , TOPICO_PUBLISH);

    server.send(200, F("text/html"), s); //Send data //Envia dados
    log("Config - Cliente: " + ipStr(server.client().remoteIP()));
  } else {
    server.send(500, F("text/plain"), F("Config - ERROR 500"));
    log(F("Config - ERRO lendo arquivo"));
  }
}

void handleConfigSave() {
  log(F("\nSalvando configuração "));
  int iNumArgumentos = 13;
  iNumArgumentos = iNumArgumentos + 1; //ESP8266 gera o campo adicional "plain" via post

  if (server.args() == iNumArgumentos) {  //Verifica número de campos recebidos
    String s;
    
    s = server.arg("apID"); s.trim();
    if (s == "") { s = apID; }
    strlcpy(apID, s.c_str(), sizeof(apID));
    s = server.arg("ssid"); s.trim();
    if (s != "") { strlcpy(ssid, s.c_str(), sizeof(ssid)); }
    s = server.arg("pw"); s.trim();
    if (s != "") { strlcpy(pw, s.c_str(), sizeof(pw)); }
    
    bsomteclas = server.arg("somteclas").toInt();
    SetSomteclas(); //Atualiza status 
    
    s = server.arg("hostnameOTA"); s.trim();
    if (s != "") { strlcpy(hostnameOTA, s.c_str(), sizeof(hostnameOTA)); }
    s = server.arg("IPfixo"); s.trim();
    if (s != "") { strlcpy(IPfixo, s.c_str(), sizeof(IPfixo)); }

//Grava dados do broker
    s = server.arg("ID_MQTT"); s.trim();
    if (s != "") { strlcpy(ID_MQTT, s.c_str(), sizeof(ID_MQTT)); }
    s = server.arg("HOST_MQTT"); s.trim();
    if (s != "") { strlcpy(HOST_MQTT, s.c_str(), sizeof(HOST_MQTT)); }
    s = server.arg("PORT_MQTT"); s.trim();
    if (s != "") { PORT_MQTT = PORT_MQTT; }
    s = server.arg("USER_MQTT"); s.trim();
    if (s != "") { strlcpy(USER_MQTT, s.c_str(), sizeof(USER_MQTT)); }
    s = server.arg("PASS_MQTT"); s.trim();
    if (s != "") { strlcpy(PASS_MQTT, s.c_str(), sizeof(PASS_MQTT)); }
    s = server.arg("TOPICO_SUBSCRIBE"); s.trim();
    if (s != "") { strlcpy(TOPICO_SUBSCRIBE, s.c_str(), sizeof(TOPICO_SUBSCRIBE)); }
    s = server.arg("TOPICO_PUBLISH"); s.trim();
    if (s != "") { strlcpy(TOPICO_PUBLISH, s.c_str(), sizeof(TOPICO_PUBLISH)); }

    if (configSave()) { // Grava configuração
      server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>alert('Configuração salva.');history.back()</script></html>"));
      log("ConfigSave - Cliente: " + ipStr(server.client().remoteIP()));
    } else {
      server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>alert('Falha salvando configuração.');history.back()</script></html>"));
      log(F("ConfigSave - ERRO salvando Config"));
    }
  } else {
    server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>alert('Erro de parâmetros.');history.back()</script></html>"));
  }
}
void handleReconfig() {
  configReset(); // Reinicia Config
  SetSomteclas();// Atualiza SomTeclas

  // Grava configuração
  if (configSave()) {
    server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>alert('Configuração reiniciada.');window.location = '/'</script></html>"));
    log("Reconfig - Cliente: " + ipStr(server.client().remoteIP()));
  } else {
    server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>alert('Falha reiniciando configuração.');history.back()</script></html>"));
    log(F("Reconfig - ERRO reiniciando Config"));
  }
}
void handleReboot() {
  // Reboot
  File file = SPIFFS.open(F("/Reboot.htm"), "r");
  if (file) {
    server.streamFile(file, F("text/html"));
    file.close();
    log("Reboot - Cliente: " + ipStr(server.client().remoteIP()));
    delay(100);
    ESP.restart();
  } else {
    server.send(500, F("text/plain"), F("Reboot - ERROR 500"));
    log(F("Reboot - ERRO lendo arquivo"));
  }
}


//Funções de Configuração ------------------------------
void  configReset() {
//  log("");
  log(F("\nAssumindo valores da configuração padrão"));
  // Define configuração padrão
//  bootCount  = 0;
  strlcpy(apID,        "<ID_APP_NAME>"   , sizeof(apID)); 
  strlcpy(ssid,        "wifi"   , sizeof(ssid));
  strlcpy(pw,          "***"      , sizeof(pw)); 
  bsomteclas = false;
  strlcpy(hostnameOTA,      "<HOST_NAME>"   , sizeof(hostnameOTA)); 
  strlcpy(IPfixo,           "xxx.xxx.xxx.xxx" , sizeof(IPfixo));
  //
  strlcpy(ID_MQTT,          "<ID_MQTT>"      , sizeof(ID_MQTT));
//  strlcpy(HOST_MQTT, "broker.hivemq.com", sizeof(HOST_MQTT)); 
  strlcpy(HOST_MQTT, "broker", sizeof(HOST_MQTT)); 
  PORT_MQTT          = 1883;
  strlcpy(USER_MQTT,        "<YOUR_USER>"            , sizeof(USER_MQTT)); 
  strlcpy(PASS_MQTT,        "***"   , sizeof(PASS_MQTT)); 
  strlcpy(TOPICO_SUBSCRIBE, "YOUR/TOPIC/sub"  , sizeof(TOPICO_SUBSCRIBE)); 
  strlcpy(TOPICO_PUBLISH,   "YOUR/TOPIC/pub" , sizeof(TOPICO_PUBLISH)); 

  Serial.print(F("apID: "));        Serial.println(apID);
  Serial.print(F("ssid: "));        Serial.println(ssid);
  Serial.print(F("pw: "));          Serial.println(pw);
  Serial.print(F("somteclas: "));   Serial.println(bsomteclas);
  Serial.print(F("hostnameOTA: ")); Serial.println(hostnameOTA);
  Serial.print(F("IPfixo: "));      Serial.println(IPfixo);
  
  Serial.print(F("ID_MQTT: "));          Serial.println(ID_MQTT);
  Serial.print(F("Broker MQTT: "));      Serial.println(HOST_MQTT);
  Serial.print(F("Broker PORT: "));      Serial.println(PORT_MQTT);
  Serial.print(F("USER_MQTT: "));        Serial.println(USER_MQTT);
  Serial.print(F("PASS_MQTT: "));        Serial.println(PASS_MQTT);
  Serial.print(F("TOPICO_SUBSCRIBE: ")); Serial.println(TOPICO_SUBSCRIBE);
  Serial.print(F("TOPICO_PUBLISH: "));   Serial.println(TOPICO_PUBLISH);
}
boolean configRead() {
  log(F("Lê configuração JSON"));
  StaticJsonDocument<JSON_SIZE> jsonConfig;

  File file = SPIFFS.open(F("/Config.json"), "r");
  if (deserializeJson(jsonConfig, file)) {
    log(F("Falha lendo Config.json, assumindo valores padrão."));
    configReset();
    return false;
  } else {
    log(F("Sucesso na leitura da configuração JSON "));
    bootCount =     jsonConfig["bootCount"]   | 0;
    strlcpy(apID,   jsonConfig["apID"]        | "", sizeof(apID)); 
    strlcpy(ssid,   jsonConfig["ssid"]        | "", sizeof(ssid)); 
    strlcpy(pw,     jsonConfig["pw"]          | "", sizeof(pw));
    bsomteclas =    jsonConfig["somteclas"] | false;
    strlcpy(hostnameOTA, jsonConfig["hostnameOTA"] | "", sizeof(hostnameOTA));
    strlcpy(IPfixo,      jsonConfig["IPfixo"]      | "", sizeof(IPfixo));

    strlcpy(ID_MQTT,          jsonConfig["ID_MQTT"]          | "", sizeof(ID_MQTT)); 
    strlcpy(HOST_MQTT, jsonConfig["HOST_MQTT"] | "", sizeof(HOST_MQTT)); 
    PORT_MQTT =        jsonConfig["PORT_MQTT"] | 0;
    strlcpy(USER_MQTT,        jsonConfig["USER_MQTT"]        | "", sizeof(USER_MQTT)); 
    strlcpy(PASS_MQTT,        jsonConfig["PASS_MQTT"]        | "", sizeof(PASS_MQTT)); 
    strlcpy(TOPICO_SUBSCRIBE, jsonConfig["TOPICO_SUBSCRIBE"] | "", sizeof(TOPICO_SUBSCRIBE)); 
    strlcpy(TOPICO_PUBLISH,   jsonConfig["TOPICO_PUBLISH"]   | "", sizeof(TOPICO_PUBLISH));
//*/

    file.close();
    log(F("\nLendo configuração Config.json"));
    serializeJsonPretty(jsonConfig, Serial);
    log("");
    return true;
  }
}
boolean configSave() { //Grava configuração
  StaticJsonDocument<JSON_SIZE> jsonConfig;

  File file = SPIFFS.open(F("/Config.json"), "w+");
  if (file) {
    // Atribui valores ao JSON e grava
    jsonConfig["bootCount"]   = bootCount;
    jsonConfig["apID"]        = apID;
    jsonConfig["ssid"]        = ssid;
    jsonConfig["pw"]          = pw;
    jsonConfig["somteclas"]   = bsomteclas;
    jsonConfig["hostnameOTA"] = hostnameOTA;
    jsonConfig["IPfixo"]      = IPfixo;
    //
    jsonConfig["ID_MQTT"]          = ID_MQTT;
    jsonConfig["HOST_MQTT"] = HOST_MQTT;
    jsonConfig["PORT_MQTT"] = PORT_MQTT;
    jsonConfig["USER_MQTT"]        = USER_MQTT;
    jsonConfig["PASS_MQTT"]        = PASS_MQTT;
    jsonConfig["TOPICO_SUBSCRIBE"] = TOPICO_SUBSCRIBE;
    jsonConfig["TOPICO_PUBLISH"]   = TOPICO_PUBLISH;
//*/
    serializeJsonPretty(jsonConfig, file);
    file.close();

    Serial.print(F("apID: "));        Serial.println(apID);
    Serial.print(F("ssid: "));        Serial.println(ssid);
    Serial.print(F("pw: "));          Serial.println(pw);
    Serial.print(F("somteclas: "));   Serial.println(bsomteclas);
    Serial.print(F("hostnameOTA: ")); Serial.println(hostnameOTA);
    Serial.print(F("IPfixo: "));      Serial.println(IPfixo);
    
    Serial.print(F("ID_MQTT: "));          Serial.println(ID_MQTT);
    Serial.print(F("Broker MQTT: "));      Serial.println(HOST_MQTT);
    Serial.print(F("Broker PORT: "));      Serial.println(PORT_MQTT);
    Serial.print(F("USER_MQTT: "));        Serial.println(USER_MQTT);
    Serial.print(F("PASS_MQTT: "));        Serial.println(PASS_MQTT);
    Serial.print(F("TOPICO_SUBSCRIBE: ")); Serial.println(TOPICO_SUBSCRIBE);
    Serial.print(F("TOPICO_PUBLISH: "));   Serial.println(TOPICO_PUBLISH);

    log(F("\nGravando Config.json"));
    serializeJsonPretty(jsonConfig, Serial);
    log("");
    return true;
  }
  return false;
}


long l_time = 0;
void f_atualiza_sensores(){
  long now = millis()/1000;
  if (now - l_time >= TempoAtualizacao){ //se maior q 10segundos
    icont_atualiza_sensores++;
    l_time = now;
    Serial.println(); //Linha em branco
    Serial.println(F("<---------------Atualizando sensores-------------->"));
    Serial.print(F("Número de atualizacoes: ")); Serial.println(icont_atualiza_sensores);
    f_printTempoResetSerial(); //tempo desde o ultimo reset
    Serial.print(F("Número de boot: ")); Serial.println(bootCount); //Número de inicializações
    f_SensorDHT11();
    Serial.println(); //Linha em branco
    f_digitalClockDisplay(); //escreve serial data hora
    f_timeluzportao_atualiza();

    Serial.print(F("Websever: http://"));  Serial.print(ipLocal);     Serial.println(F("/"));
    Serial.println(F("<------------------------------------------------->"));
    Serial.println(F("Aguardando comando IR / Websever: "));
    VerificaConexoesWiFIEMQTT();  //conexões WiFi e ao broker MQTT
    if (MQTT.connect(ID_MQTT)){
//      Serial.println(); //Linha em branco
      Serial.println(F("Aguardando comando MQTT: "));
    }
  }
}


//Definicoes Setup---------------------------------------
void setup(){
  EEPROM.begin(512);
  initSerial();
  initOutput(); //Define a porta status de saida e entrada
  f_WifiAP();
  f_Websever();
  initWiFi();   //1º de tudo connectar na rede
  initOTA();
  initUDP();    //Definicoes Data Hora / WiFiUdp NTP time
  initMQTT();
  iniDHT11();
//  iniSensorIRrecv();

  f_timeluzportao(ON);

  f_AtivaSomTeclas(OFF);
  f_piscaLedESP(3);
  f_buzzbeep(); //1 bip
  delay(10);
}
//Repetição programa principal---------------------------
void loop(){
//  iniTimeReset();   //atualiza variaveis time do reset

  yield(); //WatchDog
  dnsServer.processNextRequest(); //Server DNS
  server.handleClient();          //Webserver

//  f_Controle_Serial();
//  VerificaConexoesWiFIEMQTT();  //conexões WiFi e ao broker MQTT
  MQTT.loop();                  //keep-alive da comunicação com broker MQTT
  ArduinoOTA.handle();          //OTA

  f_digitalClock(); //atualiza variaveis time
  f_atualiza_sensores();
}
