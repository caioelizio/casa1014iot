/* Author: Caio Elizio - caioelizio@gmail.com 62 98223-9516"
 * Date: 20/06/20
 */
void f_Estatistica();
int  EEPROM_Read(int i_end_EEPROM);
void EEPROM_Clear();
void EEPROM_Write(int end_EEPROM_w,int i_valor);
void f_LedESP(boolean b_valor);
void f_piscaLedESP(int x);
void f_fade_led_esp();
void iniTimeReset();
void f_printTempoResetSerial();
void f_tempo1seg();
void iniTimeReset();
void f_printTempoResetSerial();
void f_tempo1seg();
void f_somteclas();
void f_AtivaSomTeclas(boolean b_valor);
void sendNTPpacket(IPAddress &address);

  
//Funções Genéricas--------------------------------------
void log(String s) {
  // Gera log na Serial
  Serial.println(s);
}
String softwareStr() {
  // Retorna nome do software
  return String(__FILE__).substring(String(__FILE__).lastIndexOf("\\") + 1);
}
String longTimeStr(const time_t &t){
  // Retorna segundos como "d:hh:mm:ss"
  String s = String(t / SECS_PER_DAY) + ':';
  if (hour(t) < 10) {
    s += '0';
  }
  s += String(hour(t)) + ':';
  if (minute(t) < 10) {
    s += '0';
  }
  s += String(minute(t)) + ':';
  if (second(t) < 10) {
    s += '0';
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
  return apID;
}
void SetSomteclas() {
  // Define estado do LED
  if (bsomteclas == ON) {  
    f_AtivaSomTeclas(ON);
  } else {
    f_AtivaSomTeclas(OFF);
  }  
}


void f_LedESP(boolean b_valor){ //acende e apaga o led da ESP
  if (b_valor == ON) {
    digitalWrite(pinLedESP, ON);  //LED 
    Serial.print(F("Led ESP ")); Serial.println(F("ON"));
//    EEPROM_Write(13,1);  // EEPROM_Write(0-512, 1-0)
  } else {
    digitalWrite(pinLedESP, OFF); //LED 
    Serial.print(F("Led ESP ")); Serial.println(F("OFF"));  
//    EEPROM_Write(13,0);  // EEPROM_Write(0-512, 1-0)
  }
}
void f_piscaLedESP(int x) { //Pisca o led ao ligar o arduino
  Serial.print("Pisca o led: "); Serial.print(x); Serial.println("x");
  for (int i=0; i<=x; i++){
    digitalWrite(pinLedESP, 0);    // LED off:
    delay(500);
    digitalWrite(pinLedESP, 1);    // LED on:   
    delay(500);
  }
}
void f_fade_led_esp(){    // set the LED brightness
  analogWrite(pinLedESP, brightness);       //increment/decrement the brightness for the next loop
  brightness = brightness + fadeAmount;      //limit to 10-bit (0-1023)
  if (brightness < 0) brightness = 0;
  if (brightness > 1023) brightness = 1023;  //reverse the direction of the fading at each end
  if (brightness == 0 || brightness == 1023){
    fadeAmount = -fadeAmount;
  }
  delay(delayMillis);
}

void iniTimeReset(){
  i_valor = millis()/1000;
  i_seg   = i_valor % 60;
  i_valor = i_valor / 60;
  i_min   = i_valor % 60;
  i_valor = i_valor / 60;
  i_horas = i_valor;
}
void f_printTempoResetSerial(){
    Serial.print(F("Tempo Ativo: "));
    Serial.println(longTimeStr(millis() / 1000));
//    if (i_horas < 10){ Serial.print(F("0")); } Serial.print(i_horas); Serial.print(F(":"));
//    if (i_min   < 10){ Serial.print(F("0")); } Serial.print(i_min);   Serial.print(F(":"));
//    if (i_seg   < 10){ Serial.print(F("0")); } Serial.println(i_seg);
}


void f_digitalClock(){
  segundos    = second();
  minutos     = minute();
  horas       = hour();
//  horas       = hour()/2.5; //altera em 1 hora a cada segundo
  diadasemana = day(); 
  diadomes    = day(); 
  mes         = month();
  ano         = year();
}
void f_digitalClockDisplay(){
  i_minutos = minutos;
  Serial.print(F("Data - Hora: "));  
  if (diadomes < 10)      Serial.print(F("0"));  //Acrescenta o 0 (zero) se menor do que 10
  Serial.print(diadomes); Serial.print(F("/"));
  if (mes < 10)           Serial.print(F("0"));  //Acrescenta o 0 (zero) se menor do que 10
  Serial.print(mes);      Serial.print(F("/"));  Serial.print(ano);
  Serial.print(F(" - "));
  Serial.print(horas); Serial.print(F(":"));
  if (minutos < 10)   Serial.print(F("0"));  //Acrescenta o 0 (zero) se menor do que 10
  Serial.println(minutos);
}
void printDigits(int digits){
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(F(":"));
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}


/*-------- NTP code ----------*/
const int NTP_PACKET_SIZE = 48;     //NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets
time_t getNtpTime(){
  IPAddress ntpServerIP;          //NTP server's ip address
  while (Udp.parsePacket() > 0) ; //discard any previously received packets
  Serial.println(); //Linha em branco
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


//Definicoes Sensor DHT11
void iniDHT11(){
    Serial.println(); //Linha em branco
    Serial.println(F("Iniciando DHT11..."));
//  dht.begin();

    if (b_debug == true) {
    Serial.println(F("Leitura das estatistica temperaruta e umidade EEPROM"));
    if ((EEPROM_Read(80) != 0) ){
        f_tempMin  = EEPROM_Read(80);}  //ler status salvo na memoria
    if ((EEPROM_Read(81) != 0) ){
        f_tempMax  = EEPROM_Read(81);}  //ler status salvo na memoria
    if ((EEPROM_Read(90) != 0) ){
        f_humidMin = EEPROM_Read(90);}  //ler status salvo na memoria
    if ((EEPROM_Read(91) != 0) ){
        f_humidMax = EEPROM_Read(91);}  //ler status salvo na memoria
    }//*/


//  f_SensorDHT11();
    char buffer[2]; //websever valores de temp. e hum.
    MQTT.publish("YOUR/TOPIC/t",dtostrf(0,0, 0, buffer));
    MQTT.publish("YOUR/TOPIC/h",dtostrf(0,0, 0, buffer));
}

void f_SensorDHT11(){
  dht.begin();  
  f_temp  = 0;
  f_humid = 0;
  if (dht.readTemperature()) f_temp  = dht.readTemperature();
  if (dht.readHumidity())    f_humid = dht.readHumidity();
  if (isnan(f_humid) || isnan(f_temp)){
    Serial.println(); //Linha em branco
    Serial.println(F("Falha na leitura do sensor DHT!"));
    MQTT.publish("YOUR/TOPIC/t", "ERRO");
    MQTT.publish("YOUR/TOPIC/h", "ERRO");
//    return; //interrompe a funcao
  }else{ //se ok... 
    f_Estatistica();
  }
  Serial.print(F("Temperatura: ")); Serial.print(f_temp,0);  Serial.print(F("oC"));
  Serial.print(F(" - Umidade: "));  Serial.print(f_humid,0); Serial.println(F("%"));
  if (bEstatisticaDHT11 == true) {
    Serial.println(F("-----------------------------------"));
    Serial.println(F("Estatistica Temperatura e Umidade"));
    Serial.print(F("Temperatura Min. X Max.: "));
    Serial.print(f_tempMin,0);     Serial.print(F("oC")); Serial.print(F(" X ")); Serial.print(f_tempMax,0); Serial.println(F("oC"));
    Serial.print(F("Umidade Min: X Max.    : "));
    Serial.print(f_humidMin,0);    Serial.print(F("%")); Serial.print(F(" X ")); Serial.print(f_humidMax,0); Serial.println(F("%"));
  }
  char buffer[2];
  MQTT.publish("YOUR/TOPIC/t",dtostrf(f_temp, 0, 0, buffer));
  MQTT.publish("YOUR/TOPIC/h",dtostrf(f_humid,0, 0, buffer));
}


void f_Estatistica() {
    if ((f_tempMin  == 0) ){ f_tempMin  = 100; }
    if ((f_humidMin == 0) ){ f_humidMin = 100; }
    if ((f_temp != 0) ){
      if ((f_temp < f_tempMin) ){
        f_tempMin = f_temp;
        EEPROM_Write(80,f_tempMin); //erro //EEPROM_Write(0-511,0-511)
//        EEPROM_Write(80,intflout(f_tempMin)); //erro //EEPROM_Write(0-511,0-511)
        if (b_debug == true) {
          Serial.print(F("Temperatura Minima: ")); Serial.print(f_tempMin,0);   Serial.println(F("oC"));
        }
      }
      if ((f_temp > f_tempMax) ){
        f_tempMax = f_temp;
       EEPROM_Write(81,f_tempMax);
        if (b_debug == true) {
          Serial.print(F("Temperatura Maxima: ")); Serial.print(f_tempMax,0);   Serial.println(F("oC"));
        }
      }
    }
    if ((f_humid != 0) ){
      if ((f_humid < f_humidMin) ){
        f_humidMin = f_humid;
        EEPROM_Write(90,f_humidMin);
        if (b_debug == true) {
          Serial.print(F("Umidade Minima: ")); Serial.print(f_humidMin,0);  Serial.println(F("%"));
        }
      }
      if ((f_humid > f_humidMax) ){
        f_humidMax = f_humid;
        EEPROM_Write(91,f_humidMax);
        if (b_debug == true) {
          Serial.print(F("Umidade Maxima: ")); Serial.print(f_humidMax,0);  Serial.println(F("%"));
        }
      }
    }
}


//Função: leitura da memoria da ESP
int EEPROM_Read(int i_end_EEPROM){
  byte b_value = EEPROM.read(i_end_EEPROM);  //read a byte from the current address 0-511
  if (b_debug == true){
//    Serial.println(); //Linha em branco
    Serial.print(F("Endereco memoria EEPROM: "));  Serial.print(i_end_EEPROM);
    Serial.print(F("\t")); //espaço
    Serial.print(F(" Valor read: "));  Serial.print(b_value, DEC);
    Serial.println(); //Linha em branco
  }
  if (i_end_EEPROM >= 512){ //EEPROM_Read(0-511)
    Serial.print(F("Erro endereco de memoria! Valor maior que 512"));
    return 512;
  }
  delay(500);
  return b_value;
}
void EEPROM_Write(int end_EEPROM_w,int i_valor){ //EEPROM_Write(0-511,0-511)
  //write the value to the appropriate byte of the EEPROM.
  //these values will remain there when the board is turned off.
  EEPROM.write(end_EEPROM_w, i_valor);
  if (b_debug == true){
    Serial.println(); //Linha em branco
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
//    Serial.println("EEPROM successfully committed");
  } else {
    Serial.println("ERROR! EEPROM commit failed");
  }
  delay(100);
}
void EEPROM_Clear(){ // write a 0 to all 512 bytes of the EEPROM
  EEPROM.begin(512);
    for (int i = 0; i < 512; i++) EEPROM.write(i, 0);
  EEPROM.end();
}


void f_buzz(int targetPin, long frequency, long length){
  long delayValue = 1000000 / frequency / 2; // calculate the delay value between transitions
  // 1 second's worth of microseconds, divided by the frequency, then split in half since
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
  Serial.println(F("1 bip"));
  f_buzz(pinBuzzer, 300, 50);    //1 bip inicial (pin,frequencia,Duração) //bom baixo
//  f_buzz(pinBuzzer, 800, 50);  //1 bip inicial (pin,frequencia,Duração) //bom 
//  f_buzz(pinBuzzer, 1800, 50); //1 bip inicial (pin,frequencia,Duração) //alto
}


void f_somteclas(){ //som teclas
  if (bsomteclas == true){
    f_buzzbeep();
  }
}
void f_AtivaSomTeclas(boolean b_valor) {
  if (b_valor == ON) {  
    bsomteclas = true;
    Serial.println(); //Linha em branco
    Serial.println(F("Som teclas on"));
    MQTT.publish(TOPICO_PUBLISH, "somteclason");
  } else {
    bsomteclas = false;
    Serial.println(); //Linha em branco
    Serial.println(F("Som teclas off"));
  }
}


void f_SensorMovimentoPIR_v1(){
  iSensorPIR = digitalRead(pinSensorPIR);
  if (iSensorPIR == 1)
  {
    i_ttl_movimento++;
    f_LedESP(ON);
  }else{
    f_LedESP(OFF);
  }
//  Serial.print(F(" *"));
//  Serial.print(F("iSensorPIR: ")); Serial.println(iSensorPIR);
//    Serial.print(F("i_ttl_movimento: ")); Serial.println(i_ttl_movimento);

  if ((i_ttl_movimento == 10) 
  or (i_ttl_movimento == 30)
  or (i_ttl_movimento == 100)
  or (i_ttl_movimento == 500)
  ){
    Serial.println(); //Linha em branco
    Serial.print(F("i_ttl_movimento: ")); Serial.println(i_ttl_movimento);
  }//*/
}
void f_SensorMovimentoPIR_v2(){
  int valorSensorPIR = digitalRead(pinSensorPIR);  //Lendo o valor do sensor PIR. 1 quando detecta algum movimento e 0 quando não detecta.
  Serial.print(F("Valor do Sensor PIR: ")); Serial.println(valorSensorPIR);
  //Verificando se ocorreu detecção de movimentos
  if (valorSensorPIR == 1 && movimento == false){
    if (valorSensorPIR < 100){
      Serial.print(F("Valor do Sensor PIR: ")); Serial.println(valorSensorPIR);
      movimento = true;
//      acendeled9();
    }
  }
  if (valorSensorPIR == 1 && movimento == true){
    //Serial.println(F("pir 1 mov t"));
    //Serial.print(F("ValorSensorPIR: ")); Serial.println(valorSensorPIR);
  }
  if (movimento == true){
    if (minutos != i_minutos){ //a cada 1 minuto
      Serial.println(F("a cada 1 minuto - led off"));
      i_minutos = minutos;
      movimento = false;
//      apagaled9();
    }
  }
}
