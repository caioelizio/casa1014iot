/* Author: Caio Elizio - caioelizio@gmail.com 62 98223-9516"
 * Date: 20/06/20
 * Functions / funções
 * Version 1.0 Servidor
 */
//Prototypes
int  EEPROM_Read(int i_end_EEPROM);
void EEPROM_Write(int end_EEPROM_w,int i_valor);
void EEPROM_Clear();
void f_LedESP(boolean b_valor);
void f_piscaLedESP(int x);
void f_fade_led_esp();
void f_buzz();
void f_buzzbeep();
void f_SensorMovimentoPIR_v1();
void f_AtivaSomTeclas(boolean b_valor);
//
void printDigits_zero(int digits);
void f_digitalClock();
void f_Serial_DigitalClock();
void f_timeClock();
void f_Serial_TempoAtivo();
//
void sendNTPpacket(IPAddress &address);
void log(String s);
String softwareStr();
String longTimeStr();
String hexStr();
String ipStr();
String deviceID();
//
void f_timeClock();
void f_digitalClock();
void f_Serial_digitalClock();
void printDigits_zero(int digits);
void f_Serial_TempoAtivo();
void f_Serial_Sistema();

//Funções Genéricas--------------------------------------
void f_Serial_Sistema(){
  Serial.println();
  Serial.println(F("<----------------Informações Sistema-------------->"));
  Serial.print(F("Autor:        ")); Serial.println(autor);
  Serial.print(F("Sistema:      ")); Serial.print(programa); Serial.println(versao);
  Serial.print(F("Data versao:  ")); Serial.println(dataversao);
  Serial.print(F("Número Reset: ")); Serial.println(bootCount);
//  Serial.print(F("Tempo Ativo: "));    Serial.println(longTimeStr(millis() / 1000)); //tempo desde o ultimo reset
  f_Serial_TempoAtivo(); //ok
  Serial.print(F("Websever: http://")); Serial.print(ipLocal);   Serial.println(F("/"));
  //Serial.print("Websever: http://" + ipStr(WiFi.softAPIP()));  Serial.println(F("/"));
  //Serial.print(F("WiFi AP: "));            Serial.println(String(ID_esp));
  Serial.println(F("<------------------------------------------------->"));
}

void log(String s) {
  // Gera log na Serial
  Serial.println(s);
//erro  Serial.println(F(s));
}
String softwareStr() {
  // Retorna nome do software
  return String(__FILE__).substring(String(__FILE__).lastIndexOf("\\") + 1);
}
String longTimeStr(const time_t &t){
  // Retorna segundos como "d:hh:mm:ss"
  String s = String(t / SECS_PER_DAY) + ":";
  if (hour(t) < 10) {
    s += "0";
  }
  s += String(hour(t)) + ":";
  if (minute(t) < 10) {
    s += "0";
  }
  s += String(minute(t)) + ":";
  if (second(t) < 10) {
    s += "0";
  }
  s += String(second(t));
  return s;
}
String hexStr(const unsigned long &h, const byte &l = 8) {
  // Retorna valor em formato hexadecimal
  String s;
  s= String(h, HEX);
  s.toUpperCase();
  s = ("00000000" + s).substring(s.length() + 8 - l);
  return s;
}

String ipStr(const IPAddress &ip) {
  // Retorna IPAddress em formato "n.n.n.n"
  String sFn = "";
  for (byte bFn = 0; bFn < 3; bFn++) {
    sFn += String((ip >> (8 * bFn)) & 0xFF) + ".";
  }
  sFn += String(((ip >> 8 * 3)) & 0xFF);
  return sFn;
}
String deviceID(){ // Retorna ID padrão
  return ID_esp;
}

void f_LedESP(boolean b_valor){ //acende e apaga o led da ESP
  if (b_valor == ON) {
    digitalWrite(pinLedonboard, ON);  //LED 
//    Serial.print(F("Led ESP ")); Serial.println(F("ON"));
//    EEPROM_Write(13,1);  // EEPROM_Write(0-512, 1-0)
  } else {
    digitalWrite(pinLedonboard, OFF); //LED 
//    Serial.print(F("Led ESP ")); Serial.println(F("OFF"));  
//    EEPROM_Write(13,0);  // EEPROM_Write(0-512, 1-0)
  }
}
int lastTimeLedESP = 0;
#define lesESP_Time 500
void f_piscaLedESP(int x) {     //Pisca o led ao ligar o arduino
  Serial.print(F("Pisca o led: ")); Serial.print(x); Serial.println(F("x"));
  if (millis() - lastTimeLedESP >= lesESP_Time){
    digitalWrite(pinLedonboard, 0);    // LED off:
  }else{
    digitalWrite(pinLedonboard, 1);    // LED on:    
  }
/*  for (int i=0; i<=x; i++){
    digitalWrite(pinLedonboard, 0);    // LED off:
    delay(500);
    digitalWrite(pinLedonboard, 1);    // LED on:   
    delay(500);
  }//*/
}
void f_fade_led_esp(){          // set the LED brightness
  analogWrite(pinLedonboard, brightness);       //increment/decrement the brightness for the next loop
  brightness = brightness + fadeAmount;      //limit to 10-bit (0-1023)
  if (brightness < 0) brightness = 0;
  if (brightness > 1023) brightness = 1023;  //reverse the direction of the fading at each end
  if (brightness == 0 || brightness == 1023){
    fadeAmount = -fadeAmount;
  }
  delay(delayMillis);
}


void f_timeClock(){
  i_valor = millis()/1000;
  i_segAtivo   = i_valor % 60;
  i_valor = i_valor / 60;
  i_minAtivo   = i_valor % 60;
  i_valor = i_valor / 60;
  i_horasAtivo = i_valor;
}
void f_Serial_TempoAtivo(){
//  Serial.print(F("Tempo Ativo: ")); Serial.println(longTimeStr(millis() / 1000)); //tempo desde o ultimo reset
//erro  Serial.println();
  Serial.print(F("Tempo Ativo: "));
  if (i_horasAtivo < 10){ Serial.print(F("0")); } Serial.print(i_horasAtivo); Serial.print(F(":"));
  if (i_minAtivo   < 10){ Serial.print(F("0")); } Serial.print(i_minAtivo);   Serial.print(F(":"));
  if (i_segAtivo   < 10){ Serial.print(F("0")); } Serial.println(i_segAtivo);
}

void f_digitalClock(){
  diadomes    = day();
  mes         = month();
  ano         = year();
  segundos    = second();
  minutos     = minute();
  horas       = hour();
  //horas       = hour()/2.5; //altera em 1 hora a cada segundo
}
void f_Serial_digitalClock(){
//  Serial.print(F("i_horas: ")); Serial.println(i_horas);
  if (ano != 1970) {
    Serial.println(F("Time OK!"));
    i_horas   = horas;
    i_minutos = minutos;
  }else{
    Serial.println(F("ERRO Time!"));
    configRead(); //Lê configuração Json
    if (i_minutos == 59){
      i_minutos = 0;
      if (i_horas == 23){
        i_horas = 0;
      }else{
        i_horas++;
      }
    }else{
      i_minutos++;
    }
    configSave(); //Salva configuração Json
  }
  Serial.println();
  Serial.print(F("Data: ")); printDigits_zero(diadomes); Serial.print(F("/")); printDigits_zero(mes);
//  Serial.print(F("/")); Serial.print(ano);
  Serial.print(F("/"));  printDigits_zero(ano);
  Serial.print(F(" - "));
  Serial.print(F("Hora: ")); printDigits_zero(i_horas); 
  Serial.print(F(":")); printDigits_zero(i_minutos);
  Serial.println();
}
void f_Serial_digitalClock_v1() {
  //Serial.print(F("i_horas: ")); Serial.println(i_horas);
  if (ano != 1970) {
    Serial.println(F("Time OK!"));
    i_horas   = horas;
    i_minutos = minutos;
  }else{
    Serial.println(F("ERRO Time!"));
    configRead(); //Lê configuração Json
    if (i_minutos == 59){
      i_minutos = 0;
      if (i_horas == 23){
        i_horas = 0;
      }else{
        i_horas++;
      }
    }else{
      i_minutos++;
    }
    configSave(); //Salva configuração Json
  }
  Serial.println();
  Serial.print(F("Data - Hora: "));
  printDigits_zero(diadomes); Serial.print(F("/")); printDigits_zero(mes); Serial.print(F("/")); Serial.print(ano);
  Serial.print(F(" - "));
  printDigits_zero(i_horas); Serial.print(F(":")); printDigits_zero(i_minutos);
  Serial.println();
}
void printDigits_zero(int digits) { //utility for digital clock display: prints preceding colon and leading 0
  if (digits < 10) Serial.print("0"); Serial.print(digits);
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
//teste 
//  ntpServerName[] = "a.st1.ntp.br";   //ok
//  const char ntpServerName[] = "time.nist.gov";  //ok
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


//Função: leitura da memoria da ESP
int EEPROM_Read(int i_end_EEPROM){
  byte b_value = EEPROM.read(i_end_EEPROM);  //read a byte from the current address 0-511
  if (b_debug == true){
//    Serial.println();
    Serial.print(F("Endereco memoria EEPROM: "));  Serial.print(i_end_EEPROM);
    Serial.print(F("\t")); //espaço
    Serial.print(F(" Valor read: "));  Serial.print(b_value, DEC);
    Serial.println();
  }
  if (i_end_EEPROM >= 512){ //EEPROM_Read(0-511)
    Serial.print(F("Erro endereco de memoria! Valor maior que 512"));
    return 512;
  }
  //delay(500);
  return b_value;
}
void EEPROM_Write(int end_EEPROM_w,int i_valor){ //EEPROM_Write(0-511,0-511)
  //write the value to the appropriate byte of the EEPROM.
  //these values will remain there when the board is turned off.
  EEPROM.write(end_EEPROM_w, i_valor);
  if (b_debug == true){
    Serial.println();
    Serial.print(F("Endereco memoria: ")); Serial.print(end_EEPROM_w);
    Serial.print(F("\t")); //espaço
    Serial.print(F("Valor write: "));      Serial.println(i_valor, DEC);
  }
  if (i_valor >= 512){
    Serial.print(F("Valor maior que 512, erro endereco de memoria!"));
  }else{
    EEPROM.commit();
  };
  if (EEPROM.commit()) {
//    Serial.println(F("EEPROM successfully committed"));
  } else {
    Serial.println(F("ERROR! EEPROM commit failed"));
  }
  //delay(100);
}
void EEPROM_Clear(){ // write a 0 to all 512 bytes of the EEPROM
  EEPROM.begin(512);
    for (int i = 0; i < 512; i++) EEPROM.write(i, 0);
  EEPROM.end();
}


void f_buzz(int targetPin, long frequency, long length){
  long delayValue = 1000000 / frequency / 2; // calculate the delay value between transitions
  // 1 second"s worth of microseconds, divided by the frequency, then split in half since
  // there are two phases to each cycle
  long numCycles = frequency * length / 1000; // calculate the number of cycles for proper timing
  // multiply frequency, which is really cycles per second, by the number of seconds to
  // get the total number of cycles to produce
  for (long i = 0; i < numCycles; i++){ // for the calculated length of time...
    digitalWrite(targetPin, HIGH); // write the buzzer pin high to push out the diaphram
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(targetPin, LOW);  // write the buzzer pin low to pull back the diaphram
    delayMicroseconds(delayValue); // wait again or the calculated delay value
  }
  //frequências do ouvido humano pode entender: 20Hz e 20.000Hz ou 20 kHz
  //tone(pin, frequencia, duração)  //frequencia do tom é setada em hertz, e a duração em milisegundos.
//  f_buzz(pinBuzzer, 0, 0);      //stop the tone playing:
//  f_buzz(pinBuzzer, 0, 20);     //Min
//  f_buzz(pinBuzzer, 0, 20000);  //Max
}
void f_buzzbeep(){
  if (iSomTeclas == true){
    Serial.println(F("1 bip"));
    f_buzz(pinBuzzer, 300, 50);    //1 bip inicial (pin,frequencia,Duração) //bom baixo
//  f_buzz(pinBuzzer, 800, 50);  //1 bip inicial (pin,frequencia,Duração) //bom 
//  f_buzz(pinBuzzer, 1800, 50); //1 bip inicial (pin,frequencia,Duração) //alto
  }
}

void f_AtivaSomTeclas(boolean b_valor) {
  if (b_valor == ON) {  
    iSomTeclas = 1;
    Serial.println();
    Serial.println(F("Som teclas on"));
//    myBroker.publish(TOPICO_PUBLISH, "somteclason");
  } else {
    iSomTeclas = 0;
    Serial.println();
    Serial.println(F("Som teclas off"));
  }
}
