/* Author: Caio Elizio - caioelizio@gmail.com 62 98223-9516"
 * Date: 20/06/20
 * Websever
 * Version 1.0 Print3D
 */ 
//Prototypes
void iniWebsever();
void handleRele1on();
void handleRele1off();
void handleRele2on();
void handleRele2off();
void handleTimeluzon();
void handleTimeluzoff();
void handleTudoff();
void handleDashboard();
void handleInfo();
void handleConfig();
void handleConfigSave();
void handleReconfig();
void handleReboot();
boolean configRead();
boolean configSave();
boolean dhtRead();
boolean dhtSave();
boolean TimeRead();
boolean TimeSave();


//Definicoes websever
void iniWebsever() {
  if (!SPIFFS.begin()) {
    log(F("SPIFFS ERRO"));
    while (true);
  }
  configRead(); //Lê configuração Json
  bootCount++;  //Incrementa contador de inicializações
  configSave(); //Salva configuração Json

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
  log(F("Pronto WebServer"));
}

//Requisições Web --------------------------------------
void handleDashboard() {
  File file = SPIFFS.open(F("/Dashboard.htm"), "r");
  if (file) {
    file.setTimeout(100);
    String s = file.readString();
    file.close();
    log(F("\nAtualiza conteúdo dinâmico Dashboard.htm"));
    //
    s.replace(F("#ID_esp#")        , ID_esp);
    s.replace(F("#cRele1#")        , cRele1);
    s.replace(F("#cSensor#")       , cSensor);
    s.replace(F("#iStatus_Rele1#") , (iStatus_Rele1 ? F("ON") : F("OFF") ));
    s.replace(F("#sStatusTime#")   , sStatusTime);
    s.replace(F("#f_temp#")        , String(f_temp)); //34.01 ºC
    s.replace(F("#f_humid#")       , String(f_humid));
    s.replace(F("#ihrsProgMin#")   , String(ihrsProgMin));
    s.replace(F("#i_horas#")       , String(i_horas));
    s.replace(F("#i_minutos#")     , String(i_minutos));
    
    server.send(200, F("text/html"), s); //Send data //Envia dados
    log("Dashboard: " + ipStr(server.client().remoteIP()));
  } else {
    server.send(500, F("text/plain"), F("Dashboard - ERROR 500"));
    log(F("Dashboard - ERRO lendo arquivo"));
  }
}

//Funções Dashboard ------------------------------------
void handleRele1on(){
  rele1(ON);
  handleDashboard();
  server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>history.back()</script></html>"));
}
void handleRele1off(){
  rele1(OFF);
  handleDashboard();
  server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>history.back()</script></html>"));
}
void handleRele2on(){
  rele2(ON);
  handleDashboard();
  server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>history.back()</script></html>"));
}
void handleRele2off(){
  rele2(OFF);
  handleDashboard();
  server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>history.back()</script></html>"));
}
void handleTudoff(){
  rele1(OFF);
  rele2(OFF);
  iTime = 0;
  sStatusTime    = "OFF";
//somteclas off
  handleDashboard();
  server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>history.back()</script></html>"));
}
void handleTimeluzon(){
  iTime = 1;
  sStatusTime    = "ON";
  handleDashboard();
  server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>history.back()</script></html>"));
}
void handleTimeluzoff(){
  iTime = 0;
  sStatusTime    = "OFF";
  handleDashboard();
  server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>history.back()</script></html>"));
}

void handleInfo() {
  File file = SPIFFS.open(F("/Info.htm"), "r");
  if (file) {
    file.setTimeout(100);
    String s = file.readString();
    file.close();
    log(F("\nAtualiza conteúdo dinâmico Info.htm"));
    s.replace(F("#ID_esp#")     , ID_esp);
    s.replace(F("#bootCount#")  , String(bootCount));
    s.replace(F("#iSomTeclas#") , String(iSomTeclas));
    s.replace(F("#serial#")     , hexStr(ESP.getChipId()));
    s.replace(F("#software#")   , softwareStr());
    s.replace(F("#Version#")    , Version);
    s.replace(F("#sysIP#")      , ipStr(WiFi.status() == WL_CONNECTED ? WiFi.localIP() : WiFi.softAPIP()));
    s.replace(F("#clientIP#")   , ipStr(server.client().remoteIP()));
    s.replace(F("#active#")     , longTimeStr(millis() / 1000));
    s.replace(F("#userAgent#")  , server.header(F("User-Agent")));
    s.replace(F("#autor#")      , autor);
    
    server.send(200, F("text/html"), s); //Send data //Envia dados
    log("Info - Cliente: " + ipStr(server.client().remoteIP()) +
        (server.uri() != "/" ? " [" + server.uri() + "]" : ""));
  } else {
    server.send(500, F("text/plain"), F("ERROR 500"));
    log(F("Info - ERRO lendo arquivo"));
  }
}
void handleConfig() {
  File file = SPIFFS.open(F("/Config.htm"), "r");
  if (file) {
    file.setTimeout(100);
    String s = file.readString();
    file.close();
    log(F("\nAtualiza conteúdo dinâmico Config.htm"));
    s.replace(F("#ID_esp#")           , ID_esp);
    s.replace(F("#ssid#")             , ssid);
    s.replace(F("#pw#")               , pw);
    s.replace(F("#hostnameOTA#")      , hostnameOTA);    
    s.replace(F("#IPfixo#")           , IPfixo);
    s.replace(F("#cRele1#")           , cRele1);
    s.replace(F("#cSensor#")          , cSensor);
    s.replace(F("#ihrsProgMin#")      , String(ihrsProgMin));
    s.replace(F("#i_horas#")          , String(i_horas));
    s.replace(F("#i_minutos#")        , String(i_minutos));

    server.send(200, F("text/html"), s); //Send data //Envia dados
    log("Config - Cliente: " + ipStr(server.client().remoteIP()));
  } else {
    server.send(500, F("text/plain"), F("Config - ERROR 500"));
    log(F("Config - ERRO lendo arquivo"));
  }
}

//Funções de Configuração ------------------------------
void handleConfigSave() {
  log(F("\nSalvando configuração "));
  //
  int iNumArgumentos = 10;
  iNumArgumentos = iNumArgumentos + 1; //ESP8266 gera o campo adicional "plain" via post

  if (server.args() == iNumArgumentos) {  //Verifica número de campos recebidos
    String s;
    s = server.arg("ID_esp"); s.trim();
    if (s == "") { s = ID_esp; }
    strlcpy(ID_esp, s.c_str(), sizeof(ID_esp));
    s = server.arg("ssid"); s.trim();
    if (s != "") { 
      strlcpy(ssid, s.c_str(), sizeof(ssid)); }
    s = server.arg("pw"); s.trim();
    if (s != "") { strlcpy(pw, s.c_str(), sizeof(pw)); }
    s = server.arg("hostnameOTA"); s.trim();
    if (s != "") { strlcpy(hostnameOTA, s.c_str(), sizeof(hostnameOTA)); }
    s = server.arg("IPfixo"); s.trim();
    //5
    if (s != "") { strlcpy(IPfixo, s.c_str(), sizeof(IPfixo)); }
    s = server.arg("cRele1"); s.trim();
    if (s != "") { strlcpy(cRele1, s.c_str(), sizeof(cRele1)); }
    s = server.arg("cSensor"); s.trim();
    if (s != "") { strlcpy(cSensor, s.c_str(), sizeof(cSensor)); }
    s = server.arg("ihrsProgMin"); s.trim();
    if (s != "") { ihrsProgMin = s.toInt(); }
//    ihrsProgMin2 = ihrsProgMin;
    s = server.arg("i_horas"); s.trim();
    if (s != "") { 
      i_horas = s.toInt();
    }
    s = server.arg("i_minutos"); s.trim();
    if (s != "") { 
      i_minutos = s.toInt();
    }

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
boolean configRead() {
//  log(F("Lê configuração JSON"));
  StaticJsonDocument<JSON_SIZE> jsonConfig;

  File file = SPIFFS.open(F("/Config.json"), "r");
  if (deserializeJson(jsonConfig, file)) {
    log(F("Falha lendo Config.json, assumindo valores padrão."));
    configReset();
    return false;
  } else {
//test    log(F("Sucesso na leitura da configuração JSON "));
    strlcpy(ID_esp,           jsonConfig["ID_esp"]      | "", sizeof(ID_esp)); 
    bootCount =               jsonConfig["bootCount"]   | 0;
    strlcpy(ssid,             jsonConfig["ssid"]        | "", sizeof(ssid)); 
    strlcpy(pw,               jsonConfig["pw"]          | "", sizeof(pw));
    strlcpy(hostnameOTA,      jsonConfig["hostnameOTA"] | "", sizeof(hostnameOTA));
    strlcpy(IPfixo,           jsonConfig["IPfixo"]      | "", sizeof(IPfixo));
    strlcpy(cRele1,           jsonConfig["cRele1"]      | "", sizeof(cRele1));
    strlcpy(cSensor,          jsonConfig["cSensor"]   | "", sizeof(cSensor));
    ihrsProgMin =             jsonConfig["ihrsProgMin"] | 0;
    i_horas     =             jsonConfig["i_horas"]     | 0;
    i_minutos   =             jsonConfig["i_minutos"]   | 0;
//    iSomTeclas  =             jsonConfig["iSomTeclas"]       | false;
//    TimeRead();

    file.close();
//    log(F("\nLendo configuração Config.json"));
    log(F("Lendo configuração Config.json"));
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
    jsonConfig["ID_esp"]           = ID_esp;
    jsonConfig["bootCount"]        = bootCount;
    jsonConfig["ssid"]             = ssid;
    jsonConfig["pw"]               = pw;
    jsonConfig["iSomTeclas"]       = iSomTeclas;
    jsonConfig["IPfixo"]           = IPfixo;
    //
    jsonConfig["hostnameOTA"]      = hostnameOTA;
    jsonConfig["cRele1"]           = cRele1;
    jsonConfig["cSensor"]          = cSensor;
    
    jsonConfig["ihrsProgMin"]      = ihrsProgMin;
    jsonConfig["i_horas"]          = i_horas;
    jsonConfig["i_minutos"]        = i_minutos;
    serializeJsonPretty(jsonConfig, file);
    file.close();

    log(F("\nGravando Config.json"));
    serializeJsonPretty(jsonConfig, Serial);
//    log("");
    return true;
  }
  return false;
}

void handleReconfig() {
  configReset(); // Reinicia Config

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

boolean dhtRead() {
  log(F("Lê configuração JSON"));
  StaticJsonDocument<JSON_SIZE> jsonDHT;

  File file = SPIFFS.open(F("/dht.json"), "r");
  if (deserializeJson(jsonDHT, file)) {
    log(F("Falha lendo dht.json, assumindo valores padrão."));
//    configReset();
    return false;
  } else {
    log(F("Sucesso na leitura da configuração JSON "));
    f_temp  =            jsonDHT["f_humid"] | 0;
    f_humid =            jsonDHT["s_humid"] | 0;
//    datahora =            jsonDHT["datahora"] | 0;

    file.close();
    log(F("\nLendo configuração dht.json"));
    serializeJsonPretty(jsonDHT, Serial);
    log("");
    return true;
  }
}
boolean dhtSave() { //Grava configuração
  StaticJsonDocument<JSON_SIZE> jsonDHT;
  File file = SPIFFS.open(F("/dht.json"), "w+");
  if (file) {
    // Atribui valores ao JSON e grava
    jsonDHT["f_temp"]           = f_temp;
    jsonDHT["f_humid"]          = f_humid;
    jsonDHT["datahora"]         = "";
    serializeJsonPretty(jsonDHT, file);
    file.close();

    log(F("\nGravando dht.json"));
    serializeJsonPretty(jsonDHT, Serial);
//    log("");
    return true;
  }
  return false;
}

boolean TimeRead() {
  log(F("Lê configuração JSON"));
  StaticJsonDocument<JSON_SIZE> jsonConfig;

  File file = SPIFFS.open(F("/Time.json"), "r");
  if (deserializeJson(jsonConfig, file)) {
    log(F("Falha lendo Config.json, assumindo valores padrão."));
//    configReset();
    return false;
  } else {
    log(F("Sucesso na leitura da configuração JSON "));
    i_horas   = jsonConfig["i_horas"]   | 0;
    i_minutos = jsonConfig["i_minutos"] | 0;

    file.close();
    log(F("\nLendo configuração Config.json"));
    serializeJsonPretty(jsonConfig, Serial);
    log("");
    return true;
  }
}
boolean TimeSave() { //Grava configuração
  StaticJsonDocument<JSON_SIZE> jsonConfig;
  File file = SPIFFS.open(F("/Config.json"), "w+");
  if (file) {
    // Atribui valores ao JSON e grava
    jsonConfig["i_horas"]   = i_horas;
    jsonConfig["i_minutos"] = i_minutos;
    serializeJsonPretty(jsonConfig, file);
    file.close();

    log(F("\nGravando Config.json"));
    serializeJsonPretty(jsonConfig, Serial);
//    log("");
    return true;
  }
  return false;
}

  /* JSON format for data buffer in the API
   *  This examples uses the relative timestamp as it uses the "delta_t". You can also provide the absolute timestamp using the "created_at" parameter
   *  instead of "delta_t".
   *   "{\"write_api_key\":\"YOUR-CHANNEL-WRITEAPIKEY\",\"updates\":[{\"delta_t\":0,\"field1\":-60},{\"delta_t\":15,\"field1\":200},{\"delta_t\":15,\"field1\":-66}]
   */
// Updates the ThingSpeakchannel with data
/*void httpRequest(char* jsonBuffer) {

  // Format the data buffer as noted above
/*  char data[500] = "{\"write_api_key\":\"YOUR-CHANNEL-WRITEAPIKEY\",\"updates\":"; // Replace YOUR-CHANNEL-WRITEAPIKEY with your ThingSpeak channel write API key
  strcat(data,jsonBuffer);
  strcat(data,"}");
  // Close any connection before sending a new request
  client.stop();
  String data_length = String(strlen(data)+1); //Compute the data buffer length
  Serial.println(data);
  // POST data to ThingSpeak
  if (client.connect(server, 80)) {
    client.println("POST /channels/YOUR-CHANNEL-ID/bulk_update.json HTTP/1.1"); // Replace YOUR-CHANNEL-ID with your ThingSpeak channel ID
    client.println("Host: api.thingspeak.com");
    client.println("User-Agent: mw.doc.bulk-update (Arduino ESP8266)");
    client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.println("Content-Length: "+data_length);
    client.println();
    client.println(data);
  }
  else {
    Serial.println("Failure: Failed to connect to ThingSpeak");
  }
  delay(250); //Wait to receive the response
  client.parseFloat();
  String resp = String(client.parseInt());
  Serial.println("Response code:"+resp); // Print the response code. 202 indicates that the server has accepted the response
  jsonBuffer[0] = '['; //Reinitialize the jsonBuffer for next batch of data
  jsonBuffer[1] = '\0';
  lastConnectionTime = millis(); //Update the last conenction time
}//*/
