/* Author: Caio Elizio - caioelizio@gmail.com 62 98223-9516"
 * Date: 20/06/20
 * Websever
 * Version 1.0 Servidor
 */
//Prototypes
void iniWebsever();
void handleDashboard();
void handleRele1on();
void handleRele1off();
void handleRele2on();
void handleRele2off();
void handleTimeon();
void handleTimeoff();
void handleTudoff();
void handleInfo();
void handleConfig();
void handleConfigSave();
void handleReconfig();
void handleReboot();
boolean configRead();
boolean configSave();
boolean dhtRead();
boolean dhtSave();

void iniWebsever() {
  if (!SPIFFS.begin()) {
    Serial.println(F("SPIFFS ERRO"));
    while (true);
  }
  configRead(); //Lê configuração Json
  bootCount++;  //Incrementa contador de inicializações
  configSave(); //Salva configuração Json

  // WebServer
  server.on("/dashboard"  , handleDashboard);
  server.on("/info"       , handleInfo);
  server.on("/config"     , handleConfig);
  server.on("/configSave" , handleConfigSave);
  server.on("/rele1on"    , handleRele1on);
  server.on("/rele1off"   , handleRele1off);
  server.on("/rele2on"    , handleRele2on);
  server.on("/rele2off"   , handleRele2off);
  server.on("/tudoff"     , handleTudoff);
  server.on("/timeon"     , handleTimeon);
  server.on("/timeoff"    , handleTimeoff);
  server.on("/reconfig"   , handleReconfig);
  server.on("/reboot"     , handleReboot);
  server.onNotFound(handleDashboard); //pagina default
  server.collectHeaders(WEBSERVER_HEADER_KEYS, 1);
  server.begin();
  Serial.println(F("WebServer Pronto"));
}


//Requisições Web --------------------------------------
void handleDashboard() {
  File file = SPIFFS.open("/Dashboard.htm", "r");
  if (file) {
    file.setTimeout(100);
    String s = file.readString();
    file.close();
    Serial.println(F("\nAtualiza conteúdo dinâmico Dashboard.htm"));
    //
    s.replace("#ID_esp#"        , ID_esp);
    s.replace("#cRele1#"        , cRele1);
    s.replace("#cRele2#"        , cRele2);
//    s.replace(F("#iStatus_Rele1#") , (iStatus_Rele1 ? F("ON") : F("OFF") ));
//    s.replace(F("#iStatus_Rele2#") , (iStatus_Rele2 ? F("ON") : F("OFF") ));
    s.replace("#sStatus_Rele1#" , sStatus_Rele1);
    s.replace("#sStatus_Rele2#" , sStatus_Rele2);
    s.replace("#sStatusTime#"   , sStatusTime);
//
    s.replace("#s_temp#"        , String(f_temp));    //34.01 ºC
    s.replace("#s_humid#"       , String(f_humid));
//    s.replace("#s_temp#"        , s_temp);          //34 ºC
//    s.replace("#s_humid#"       , s_humid);
    s.replace("#s_temp_sala#"   , s_temp_sala);
    s.replace("#s_humid_sala#"  , s_humid_sala);
    s.replace("#s_temp_qrt#"    , s_temp_qrt);
    s.replace("#s_humid_qrt#"   , s_humid_qrt);
    s.replace("#s_temp_lab#"    , s_temp_lab);
    s.replace("#s_humid_lab#"   , s_humid_lab);
    s.replace("#s_temp_3d#"     , s_temp_3d);
    s.replace("#s_humid_3d#"    , s_humid_3d);
//
    s.replace("#iSomTeclas#"    , iSomTeclas ? ("ON") : ("OFF"));
//  s.replace("#iSomTeclas#"    , String(iSomTeclas));
    s.replace("#i_horas#"       , String(i_horas));
    s.replace("#i_minutos#"     , String(i_minutos));
    s.replace("#ihrsProgIni#"   , String(ihrsProgIni));
    s.replace("#ihrsProgFin#"   , String(ihrsProgFin));

    server.send(200, "text/html", s); //Send data //Envia dados
    Serial.println("Dashboard: " + ipStr(server.client().remoteIP()));
  } else {
    server.send(500, "text/plain", "Dashboard - ERROR 500");
    Serial.println(F("Dashboard - ERRO lendo arquivo"));
  }
}

//Funções Dashboard ------------------------------------
void handleRele1on(){
  rele1(ON);
  handleDashboard();
  server.send(200, "text/html", "<html><meta charset='UTF-8'><script>history.back()</script></html>");
}
void handleRele1off(){
  rele1(OFF);
  handleDashboard();
  server.send(200, "text/html", "<html><meta charset='UTF-8'><script>history.back()</script></html>");
}
void handleRele2on(){
  rele2(ON);
  handleDashboard();
  server.send(200, "text/html", "<html><meta charset='UTF-8'><script>history.back()</script></html>");
}
void handleRele2off(){
  rele2(OFF);
  handleDashboard();
  server.send(200, "text/html", "<html><meta charset='UTF-8'><script>history.back()</script></html>");
}

void handleTudoff(){
  tudoff();
  handleDashboard();
  server.send(200, "text/html", "<html><meta charset='UTF-8'><script>history.back()</script></html>");
}
void handleTimeon(){
  f_TimeRele1(ON);
  handleDashboard();
  server.send(200, "text/html", "<html><meta charset='UTF-8'><script>history.back()</script></html>");
}
void handleTimeoff(){
  f_TimeRele1(OFF);
  handleDashboard();
  server.send(200, "text/html", "<html><meta charset='UTF-8'><script>history.back()</script></html>");
}


void handleInfo(){
  File file = SPIFFS.open("/Info.htm", "r");
  if (file) {
    file.setTimeout(100);
    String s = file.readString();
    file.close();
    Serial.println(F("\nAtualiza conteúdo dinâmico Info.htm"));
    s.replace("#ID_esp#"     , ID_esp);
    s.replace("#iSomTeclas#" , iSomTeclas ? "Ligado" : "Desligado");
    s.replace("#bootCount#"  , String(bootCount));
    s.replace("#serial#"     , hexStr(ESP.getChipId()));
    s.replace("#software#"   , softwareStr());
    s.replace("#versao#"     , versao);
    s.replace("#sysIP#"      , ipStr(WiFi.status() == WL_CONNECTED ? WiFi.localIP() : WiFi.softAPIP()));
    s.replace("#clientIP#"   , ipStr(server.client().remoteIP()));
    s.replace("#active#"     , longTimeStr(millis() / 1000));
    s.replace("#userAgent#"  , server.header("User-Agent"));
    s.replace("#autor#"      , autor);
    
    server.send(200, "text/html", s); //Send data //Envia dados
    Serial.println("Info - Cliente: " + ipStr(server.client().remoteIP()) +
        (server.uri() != "/" ? " [" + server.uri() + "]" : ""));
  } else {
    server.send(500, "text/plain", "ERROR 500");
    Serial.println("Info - ERRO lendo arquivo");
  }
}


//Funções de Configuração ------------------------------
void handleConfigSave() {
  Serial.println(F("\nSalvando configuração "));
  //
  int iNumArgumentos = 21;
  iNumArgumentos = iNumArgumentos + 1; //ESP8266 gera o campo adicional "plain" via post

  if (server.args() == iNumArgumentos) {  //Verifica número de campos recebidos
    String s;
    s = server.arg("ID_esp"); s.trim();
    if (s == "") { s = ID_esp; }
    strlcpy(ID_esp, s.c_str(), sizeof(ID_esp));
    s = server.arg("ssid"); s.trim();
    if (s != "") { strlcpy(ssid, s.c_str(), sizeof(ssid)); }
    s = server.arg("pw"); s.trim();
    if (s != "") { strlcpy(pw, s.c_str(), sizeof(pw)); }
    s = server.arg("hostnameOTA"); s.trim();
    if (s != "") { strlcpy(hostnameOTA, s.c_str(), sizeof(hostnameOTA)); }
    s = server.arg("IPfixo"); s.trim();
//5
    if (s != "") { strlcpy(IPfixo, s.c_str(), sizeof(IPfixo)); }
    //Grava dados do broker
    s = server.arg("ID_MQTT"); s.trim();
    if (s != "") { strlcpy(ID_MQTT, s.c_str(), sizeof(ID_MQTT)); }
    s = server.arg("HOST_MQTT"); s.trim();
    if (s != "") { strlcpy(HOST_MQTT, s.c_str(), sizeof(HOST_MQTT)); }
    s = server.arg("PORT_MQTT"); s.trim();
    if (s != "") { PORT_MQTT = PORT_MQTT; }
//    if (s != "") { strlcpy(PORT_MQTT, s.c_str(), sizeof(PORT_MQTT)); }
    s = server.arg("USER_MQTT"); s.trim();
    if (s != "") { strlcpy(USER_MQTT, s.c_str(), sizeof(USER_MQTT)); }
    s = server.arg("PASS_MQTT"); s.trim();
//10
    if (s != "") { strlcpy(PASS_MQTT, s.c_str(), sizeof(PASS_MQTT)); }
    s = server.arg("TOPICO_SUBSCRIBE"); s.trim();
    if (s != "") { strlcpy(TOPICO_SUBSCRIBE, s.c_str(), sizeof(TOPICO_SUBSCRIBE)); }
    s = server.arg("TOPICO_PUBLISH"); s.trim();
    if (s != "") { strlcpy(TOPICO_PUBLISH, s.c_str(), sizeof(TOPICO_PUBLISH)); }
    s = server.arg("TOPICO_PUBLISH_T"); s.trim();
    if (s != "") { strlcpy(TOPICO_PUBLISH_T, s.c_str(), sizeof(TOPICO_PUBLISH_T)); }
    s = server.arg("TOPICO_PUBLISH_H"); s.trim();
    if (s != "") { strlcpy(TOPICO_PUBLISH_H, s.c_str(), sizeof(TOPICO_PUBLISH_H)); }
    s = server.arg("cRele1"); s.trim();
    if (s != "") { strlcpy(cRele1, s.c_str(), sizeof(cRele2)); }
//15
    s = server.arg("cRele2"); s.trim();
    if (s != "") { strlcpy(cRele2, s.c_str(), sizeof(cRele2)); }
    s = server.arg("i_horas");   s.trim();
    if (s != "") { i_horas     = s.toInt(); }
    s = server.arg("i_minutos"); s.trim();
    if (s != "") { i_minutos   = s.toInt(); }
    s = server.arg("ihrsProgIni"); s.trim();
    if (s != "") { ihrsProgIni = s.toInt(); }
    s = server.arg("ihrsProgFin"); s.trim();
    if (s != "") { ihrsProgFin = s.toInt(); }
//20
    iSomTeclas = server.arg("iSomTeclas").toInt();
    if (s != "") { iSomTeclas = s.toInt(); }

    if (configSave()) { // Grava configuração
      server.send(200, "text/html", "<html><meta charset='UTF-8'><script>alert('Configuração salva.');history.back()</script></html>");
      Serial.println("ConfigSave - Cliente: " + ipStr(server.client().remoteIP()));
    } else {
      server.send(200, "text/html", "<html><meta charset='UTF-8'><script>alert('Falha salvando configuração.');history.back()</script></html>");
      Serial.println(F("ConfigSave - ERRO salvando Config"));
    }
  } else {
    server.send(200, "text/html", "<html><meta charset='UTF-8'><script>alert('Erro de parâmetros.');history.back()</script></html>");
  }
}
void handleConfig() {
  File file = SPIFFS.open("/Config.htm", "r");
  if (file) {
    file.setTimeout(100);
    String s = file.readString();
    file.close();
    Serial.println(F("Atualiza conteúdo dinâmico Config.htm"));
//    s.replace("#bootCount#"        , "");
//    s.replace("#bootCount#"        , String(bootCount));
    s.replace("#ID_esp#"           , ID_esp);
    s.replace("#ssid#"             , ssid);
    s.replace("#pw#"               , pw);
    s.replace("#hostnameOTA#"      , hostnameOTA);    
    s.replace("#IPfixo#"           , IPfixo);
//
    s.replace("#ID_MQTT#"          , ID_MQTT);
    s.replace("#HOST_MQTT#"        , HOST_MQTT);
    s.replace("#PORT_MQTT#"        , String(PORT_MQTT));
    s.replace("#USER_MQTT#"        , USER_MQTT);
    s.replace("#PASS_MQTT#"        , PASS_MQTT);
    s.replace("#TOPICO_SUBSCRIBE#" , TOPICO_SUBSCRIBE);
    s.replace("#TOPICO_PUBLISH#"   , TOPICO_PUBLISH);
    s.replace("#TOPICO_PUBLISH_T#" , TOPICO_PUBLISH_T);
    s.replace("#TOPICO_PUBLISH_H#" , TOPICO_PUBLISH_H);
//
    s.replace("#cRele1#"           , cRele1);
    s.replace("#cRele2#"           , cRele2);
    s.replace(F("#i_horas#")       , String(i_horas));
    s.replace(F("#i_minutos#")     , String(i_minutos));
    s.replace(F("#ihrsProgIni#")   , String(ihrsProgIni));
    s.replace(F("#ihrsProgFin#")   , String(ihrsProgFin));
    s.replace(F("#iSomTeclas#")    ,  iSomTeclas ? " checked" : "");
//    s.replace(F("#iSomTeclas#")    ,  !iSomTeclas ? " checked" : "");
    s.replace(F("#somteclason#")   ,  iSomTeclas ? " checked" : "");
    s.replace(F("#somteclasoff#")  , !iSomTeclas ? " checked" : "");

    server.send(200, "text/html", s); //Send data //Envia dados
    Serial.println("Config - Cliente: " + ipStr(server.client().remoteIP()));
  } else {
    server.send(500, "text/plain", "Config - ERROR 500");
    Serial.println(F("Config - ERRO lendo arquivo"));
  }
}
boolean configRead() {
//test  Serial.println(F("Lê configuração JSON"));
  StaticJsonDocument<JSON_SIZE> jsonConfig;

  File file = SPIFFS.open("/Config.json", "r");
  if (deserializeJson(jsonConfig, file)) {
    Serial.println(F("Falha lendo Config.json, assumindo valores padrão."));
    configReset();
    return false;
  } else {
//test    Serial.println(F("Sucesso na leitura da configuração JSON "));
    bootCount =               jsonConfig["bootCount"]   | 0;
    strlcpy(ID_esp,           jsonConfig["ID_esp"]      | "", sizeof(ID_esp)); 
    strlcpy(ssid,             jsonConfig["ssid"]        | "", sizeof(ssid)); 
    strlcpy(pw,               jsonConfig["pw"]          | "", sizeof(pw));
    strlcpy(IPfixo,           jsonConfig["IPfixo"]      | "", sizeof(IPfixo));
    strlcpy(hostnameOTA,      jsonConfig["hostnameOTA"] | "", sizeof(hostnameOTA));
//
    strlcpy(ID_MQTT,          jsonConfig["ID_MQTT"]          | "", sizeof(ID_MQTT)); 
    strlcpy(HOST_MQTT,        jsonConfig["HOST_MQTT"]        | "", sizeof(HOST_MQTT)); 
    PORT_MQTT =               jsonConfig["PORT_MQTT"]        | 0;
    strlcpy(USER_MQTT,        jsonConfig["USER_MQTT"]        | "", sizeof(USER_MQTT)); 
    strlcpy(PASS_MQTT,        jsonConfig["PASS_MQTT"]        | "", sizeof(PASS_MQTT)); 
    strlcpy(TOPICO_SUBSCRIBE, jsonConfig["TOPICO_SUBSCRIBE"] | "", sizeof(TOPICO_SUBSCRIBE)); 
    strlcpy(TOPICO_PUBLISH,   jsonConfig["TOPICO_PUBLISH"]   | "", sizeof(TOPICO_PUBLISH));
    strlcpy(TOPICO_PUBLISH_T, jsonConfig["TOPICO_PUBLISH_T"] | "", sizeof(TOPICO_PUBLISH_T)); 
    strlcpy(TOPICO_PUBLISH_H, jsonConfig["TOPICO_PUBLISH_H"] | "", sizeof(TOPICO_PUBLISH_H));
//
    strlcpy(cRele1,           jsonConfig["cRele1"]           | "", sizeof(TOPICO_PUBLISH_H));
    strlcpy(cRele2,           jsonConfig["cRele2"]           | "", sizeof(TOPICO_PUBLISH_H));
    i_horas     =             jsonConfig["i_horas"]          | 0;
    i_minutos   =             jsonConfig["i_minutos"]        | 0;
    ihrsProgIni =             jsonConfig["ihrsProgIni"]      | 0;
    ihrsProgFin =             jsonConfig["ihrsProgFin"]      | 0;    
    iSomTeclas  =             jsonConfig["iSomTeclas"]       | 0;

    file.close();
    Serial.println(F("\nLendo configuração Config.json"));
    serializeJsonPretty(jsonConfig, Serial);
    Serial.println("");
    return true;
  }
}
boolean configSave() { //Grava configuração  
  StaticJsonDocument<JSON_SIZE> jsonConfig;
  File file = SPIFFS.open("/Config.json", "w+");
  if (file) {
    // Atribui valores ao JSON e grava
    jsonConfig["bootCount"]        = bootCount;
    jsonConfig["ID_esp"]           = ID_esp;
    jsonConfig["ssid"]             = ssid;
    jsonConfig["pw"]               = pw;
    jsonConfig["hostnameOTA"]      = hostnameOTA;
    jsonConfig["IPfixo"]           = IPfixo;
    //
    jsonConfig["ID_MQTT"]          = ID_MQTT;
    jsonConfig["HOST_MQTT"]        = HOST_MQTT;
    jsonConfig["PORT_MQTT"]        = PORT_MQTT;
    jsonConfig["USER_MQTT"]        = USER_MQTT;
    jsonConfig["PASS_MQTT"]        = PASS_MQTT;
    jsonConfig["TOPICO_SUBSCRIBE"] = TOPICO_SUBSCRIBE;
    jsonConfig["TOPICO_PUBLISH"]   = TOPICO_PUBLISH;
    jsonConfig["TOPICO_PUBLISH_T"] = TOPICO_PUBLISH_T;
    jsonConfig["TOPICO_PUBLISH_H"] = TOPICO_PUBLISH_H;
//
    jsonConfig["cRele1"]           = cRele1;
    jsonConfig["cRele2"]           = cRele2;
    jsonConfig["i_horas"]          = i_horas;
    jsonConfig["i_minutos"]        = i_minutos;
    jsonConfig["ihrsProgIni"]      = ihrsProgIni;
    jsonConfig["ihrsProgFin"]      = ihrsProgFin;
    jsonConfig["iSomTeclas"]       = iSomTeclas;
    
    serializeJsonPretty(jsonConfig, file);
    file.close();

    Serial.println(F("\nGravando Config.json"));
    serializeJsonPretty(jsonConfig, Serial);
//    Serial.println("");
    return true;
  }
  return false;
}


void handleReconfig() {
  configReset(); // Reinicia Config

  // Grava configuração
  if (configSave()) {
    server.send(200, "text/html", "<html><meta charset='UTF-8'><script>alert('Configuração reiniciada.');window.location = '/'</script></html>");
    Serial.println("Reconfig - Cliente: " + ipStr(server.client().remoteIP()));
  } else {
    server.send(200, "text/html", "<html><meta charset='UTF-8'><script>alert('Falha reiniciando configuração.');history.back()</script></html>");
    Serial.println(F("Reconfig - ERRO reiniciando Config"));
  }
}
void handleReboot() {
  // Reboot
  File file = SPIFFS.open("/Reboot.htm", "r");
  if (file) {
    server.streamFile(file, "text/html");
    file.close();
    Serial.println("Reboot - Cliente: " + ipStr(server.client().remoteIP()));
    delay(100);
    ESP.restart();
  } else {
    server.send(500, "text/plain", "Reboot - ERROR 500");
    Serial.println("Reboot - ERRO lendo arquivo");
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
    f_temp =             jsonDHT["f_temp"] | 0;
    f_humid =            jsonDHT["f_humid"] | 0;
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
