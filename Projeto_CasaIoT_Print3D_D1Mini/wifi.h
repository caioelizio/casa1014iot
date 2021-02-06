/* Author: Caio Elizio - caioelizio@gmail.com 62 98223-9516"
 * Date: 20/06/20
 * Wi-fi
 * Version 1.0 Print3D
 */
//Prototypes
void f_WifiAP();
void initWiFi();
void reconectWiFi();
void reconectWiFi_v2();
void printWifiStatus();
void f_Serial_Wifi();
void initOTA();
void initUDP();


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
//  ++i_contTentWifi;
//  if (i_contTentWifi == 30){
//    WiFi.mode(WIFI_AP);             //AP
//    f_WifiAP();
//    i_contTentWifi = 0;
//    ESP.restart();
//  }else{
//    reconectWiFi_v1();
    reconectWiFi();
    printWifiStatus();
//  }
}
int  i_contTentWifi = 0;
void reconectWiFi(){
  if (WiFi.status() == WL_CONNECTED) return;
  Serial.print(F("ERRO conectando WI-FI: ")); Serial.println(i_contTentWifi);
  Serial.print(F("Força do sinal (RSSI): ")); Serial.print(l_rssi); Serial.println(F(" dBm"));
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
//    iniThingspeak();
    //delay(500);
  }
}

void reconectWiFi_v2(){
  int cont = 0;
  if (WiFi.status() == WL_CONNECTED) return;
  Serial.println(F("ERRO conectando WI-FI"));
  Serial.print(F("Força do sinal (RSSI):       ")); Serial.print(l_rssi);         Serial.println(F(" dBm"));
//  f_LCD_Wifi_erro();
  while (WiFi.status() != WL_CONNECTED) {
    if (i_contTentWifi > 50){
//      i_contTentWifi = 0;
//      break;
    }  
    ++cont;
    Serial.print(F("."));
    if (cont == 50){
      Serial.println();
      Serial.print(F("Numero de Tentativas: ")); Serial.println(i_contTentWifi);
      initWiFi();
    }
//    ++i_contTentWifi;
    if (i_contTentWifi == 10){
      i_contTentWifi = 0;
//      WiFi.mode(WIFI_AP);             //AP
//      f_WifiAP();
      //ESP.restart();
    //}else{
    }
    delay(100);
  }
}


void f_Serial_Wifi(){
  Serial.println();
  Serial.println(F("<----------------------WI-FI---------------------->"));
  Serial.print  (F("Conectado a rede WI-FI: ")); Serial.println(wifi_id);
  Serial.print  (F("IP Local: "));               Serial.println(ipStr(WiFi.localIP()));
  Serial.print  (F("Força do sinal (RSSI):  ")); Serial.print(l_rssi); Serial.println(F(" dBm"));
  Serial.println(F("<------------------------------------------------->"));
}

void initOTA(){
//  WiFi.hostname(hostnameOTA);
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


void f_WifiAP() {
//erro  WiFi.softAP(ID_esp, pw); //WiFi AP Access Point
  WiFi.softAP("casa.IoT.Print3D", pw); //WiFi AP Access Point
  WiFi.softAPConfig(apIP, gateway, IPAddress(subnet));
  log("");
//log("*********************************************************");
  log("<-------------------------------------------------------->");
  log("WiFi AP: " + String(ID_esp) + " - IP " + ipStr(WiFi.softAPIP()));
  log("Conecte-se nesta rede para acessar a configuração da ESP");
  log("Password igual da rede Wifi");
  log("<-------------------------------------------------------->");
  log("");
  // Habilita roteamento DNS
  dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
  dnsServer.start(DNSSERVER_PORT, "*", apIP);
}

/*-------- NTP code ----------*/
const int NTP_PACKET_SIZE = 48;     //NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets
time_t getNtpTime(){
  IPAddress ntpServerIP;          //NTP server"s ip address
  while (Udp.parsePacket() > 0) ; //discard any previously received packets
  Serial.println();
  Serial.println(F("Transmit NTP Request"));
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(F("Server NTP: ")); Serial.print(ntpServerName); 
  Serial.print(F(": "));           Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500){
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE){
      Serial.println(F("Receive NTP Response OK"));
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println(F("No NTP Response :-("));
  return 0; // return 0 if unable to get the time
}
// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address){
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
