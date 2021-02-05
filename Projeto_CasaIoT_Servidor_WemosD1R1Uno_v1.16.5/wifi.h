/* Author: Caio Elizio - caioelizio@gmail.com 62 98223-9516"
 * Date: 20/06/2020 - Ultima Alteração: 19/09/2020
 * Wi-fi
 * Versao Servidor
 */
//Prototypes
void f_WifiAP();
void initWiFi();
void reconectWiFi_v1();
void reconectWiFi_v2();
void printWifiStatus();
void f_Serial_Wifi();
void initOTA();
void initUDP();


void f_WifiAP() {
  WiFi.mode(WIFI_AP);             //AP
//erro  WiFi.softAP(ID_esp, pw); //WiFi AP Access Point
  WiFi.softAP(ssid, pw);
  WiFi.softAP("casa.IoT.Servidor", pw); //WiFi AP Access Point
  WiFi.softAPConfig(apIP, gateway, IPAddress(subnet));
  Serial.println();
  Serial.println(F("<-------------------------------------------------------->"));
  Serial.println("WiFi AP: " + String(ID_esp) + " - IP " + ipStr(WiFi.softAPIP()));
  Serial.println(F("Conecte-se nesta rede para acessar a configuração da ESP"));
  Serial.println(F("Password igual da rede Wifi"));
  Serial.println(F("<-------------------------------------------------------->"));
  Serial.println();
  // Habilita roteamento DNS
  dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
  dnsServer.start(DNSSERVER_PORT, "*", apIP);
}


void initWiFi(){
  delay(10);
  Serial.println();
  Serial.print  (F("Conectando a rede WI-FI: "));  Serial.println(ssid);
  Serial.println(F("Aguarde..."));
  WiFi.mode(WIFI_STA);              //OTA
  WiFi.begin(ssid, pw);             //Conecta na rede WI-FI
  WiFi.config(ip, gateway, subnet); //PASSA OS PARÂMETROS PARA A FUNÇÃO QUE VAI SETAR O IP FIXO NO NODEMCU
  WiFi.hostname(hostnameOTA);
  wifi_id = WiFi.SSID();
  l_rssi  = WiFi.RSSI();
  ipLocal = WiFi.localIP();
  reconectWiFi_v2();
  printWifiStatus();
}
int  i_contTentWifi = 0;
void reconectWiFi_v1(){
  int cont = 0;
  if (WiFi.status() == WL_CONNECTED) return;
  Serial.print(F("ERRO conectando WI-FI: ")); Serial.println(i_contTentWifi);
  Serial.print(F("Força do sinal (RSSI): ")); Serial.print(l_rssi); Serial.println(F(" dBm"));
//  f_LCD_Wifi_erro();
  while (WiFi.status() != WL_CONNECTED) {
    if (i_contTentWifi > 50){
//      i_contTentWifi = 0;
//      break;
    }  
    ++cont;
    Serial.print(F("."));
    if (cont == 50){
      cont = 0;
      Serial.println();
      Serial.print(F("Numero de Tentativas: ")); Serial.println(i_contTentWifi);
      initWiFi();
    }
    if (i_contTentWifi == 10){
      i_contTentWifi = 0;
//      WiFi.mode(WIFI_AP);             //AP
//      f_WifiAP();
//    i_contTentWifi = EEPROM_Read(400);
      //ESP.restart();
    //}else{
    }
    delay(100);
  }
}
void reconectWiFi_v2(){
  if (WiFi.status() == WL_CONNECTED) return;
  Serial.print(F("ERRO conectando WI-FI: ")); Serial.println(i_contTentWifi);
  Serial.print(F("Força do sinal (RSSI):       ")); Serial.print(l_rssi);         Serial.println(F(" dBm"));
  if (WiFi.status() != WL_CONNECTED) {
    i_contTentWifi++;
    if (i_contTentWifi == 10){
      i_contTentWifi = 0;
      Serial.println();
      Serial.print(F("Numero de Tentativas: ")); Serial.println(i_contTentWifi);
      initWiFi();
    }      
    delay(100);
  }
}
void printWifiStatus(){
  if (WiFi.status() == WL_CONNECTED){
    WiFi.mode(WIFI_STA);              //OTA
    f_Serial_Wifi();
    //delay(50);
  }
}
void f_Serial_Wifi(){
    Serial.println();
    Serial.println(F("<----------------------WI-FI---------------------->"));
    Serial.print(F("Conectado a rede WI-FI: ")); Serial.println(wifi_id);
    Serial.print(F("IP Local: "));               Serial.println(ipLocal);
//    Serial.print(F("IP Local: "));               Serial.println(ipStr(WiFi.localIP()));
    Serial.print(F("Força do sinal (RSSI):  ")); Serial.print(l_rssi); Serial.println(F(" dBm"));
    Serial.println(F("<------------------------------------------------->"));
}

void initOTA(){
  WiFi.hostname(hostnameOTA);
  ArduinoOTA.setPort(8266); //Port defaults to 8266
  ArduinoOTA.setHostname(hostnameOTA);
//  ArduinoOTA.setPassword((const char *)pw);  //OPTIONAL
  Serial.println();
  ArduinoOTA.onStart([]() { Serial.println(F("Start")); });
  ArduinoOTA.onEnd([]()   { Serial.println(F("\nEnd")); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total){
    Serial.println();
    Serial.printf("ProgFailed to read from DHT senress: %u%%\r"), (progress / (total / 100));
  });
  ArduinoOTA.onError([](ota_error_t error){
    Serial.println();
    Serial.printf("Error[%u]: ", error); //erro printf(F(
    Serial.println(F("erro - 2 bip na sirene"));
    if      (error == OTA_AUTH_ERROR)    Serial.println(F("Auth Failed"));
    else if (error == OTA_BEGIN_ERROR)   Serial.println(F("Begin Failed"));
    else if (error == OTA_CONNECT_ERROR) Serial.println(F("Connect Failed"));
    else if (error == OTA_RECEIVE_ERROR) Serial.println(F("Receive Failed"));
    else if (error == OTA_END_ERROR)     Serial.println(F("End Failed"));
  });
  ArduinoOTA.begin();
  Serial.println();
  Serial.println(F("Iniciando OTA..."));
  Serial.print(F("Hostname OTA: ")); Serial.println(hostnameOTA);
}
void initUDP(){
  Serial.println();
  Serial.println(F("Iniciando UDP...")); 
  Udp.begin(localPort);
  Serial.print(F("Local port: "));  Serial.println(Udp.localPort());
  Serial.println(F("waiting for sync"));
  setSyncProvider(getNtpTime);
  setSyncInterval(300);
}
