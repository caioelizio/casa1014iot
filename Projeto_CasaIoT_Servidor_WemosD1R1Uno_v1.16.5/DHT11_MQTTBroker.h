/* Author: Caio Elizio - caioelizio@gmail.com 62 98223-9516"
 * Date: 08/08/20
 * Definicoes Sensor DHT11 / Broker
 * Versao 1.0 Servidor
 */  
//Prototypes
void iniDHT11();
void f_SensorDHT11();
void f_Serial_DHT11_v1();
void f_Serial_DHT11_v2();
String getRead_DHT11();
String getRead_DHT11_qrt();
String getRead_DHT11_sala();
String getRead_DHT11_lab();
String getRead_DHT11_3d();

void iniDHT11(){
  Serial.println();
  Serial.println(F("Iniciando DHT11..."));
}
void f_SensorDHT11(){
  dht.begin();  
  if (dht.readTemperature()) f_temp  = dht.readTemperature();
  if (dht.readHumidity())    f_humid = dht.readHumidity();

  if (isnan(f_temp) || isnan(f_humid)){ //isnan() trabalha com números de ponto flutuante, não inteiros.
    Serial.println();
    Serial.println(F("Falha na leitura do sensor DHT11!"));
//    f_LCD_DHT_erro();
//    myBroker.publish(TOPICO_PUBLISH_T, "ERRO");
//    myBroker.publish(TOPICO_PUBLISH_H, "ERRO");
    dhtRead(); //Lê configuração Json
//    return; //interrompe a funcao
  }
  else{ //se ok... 
    Serial.println(F("Sensor DHT11 Atuazizado!"));
    dhtSave(); //Salva configuração Json
    s_temp  = String(f_temp);
    s_humid = String(f_humid);
  }
  f_Serial_DHT11_v2();
}
void f_Serial_DHT11_v1(){
//  if (isnan(f_humid) || isnan(f_temp)){ //isnan() trabalha com números de ponto flutuante, não inteiros.
  if (String(f_temp) == "nan") {
    Serial.println();
    Serial.println(F("<--------------Temperatura e Umidade-------------->"));
//  Serial.print(F("Temperatura: ")); Serial.print(String(f_temp));  Serial.print(F("oC"));
    Serial.print(F("Temperatura: ")); Serial.print(f_temp,0);  Serial.print(F("oC"));
    Serial.print(F(" - Umidade: "));  Serial.print(f_humid,0); Serial.println(F("%"));
    Serial.println(F("<------------------------------------------------->"));
  }else{
    Serial.println(F("Falha na leitura do sensor DHT11"));
  }
}
void f_Serial_DHT11_v2(){
  Serial.println();
  Serial.println(F("<--------------Temperatura e Umidade-------------->"));
  Serial.print(F("Temperatura: ")); Serial.print(f_temp,0);  Serial.print(F("oC"));
  Serial.print(F(" - Umidade: "));  Serial.print(f_humid,0); Serial.println(F("%"));
  Serial.println(F("<------------------------------------------------->"));
}


//sensor temp e hum
String getRead_DHT11(){
  String message  = "Temperatura: " + String(s_temp)   + " ºC \n";
         message += "Umidate: "     + String(s_humid) + " % \n";
  return message;
}
String getRead_DHT11_qrt(){
  String message = "Temperature: " + String(s_temp_qrt) + " ºC \n";
         message += "Humidity: "   + String(s_humid_qrt) + " % \n";
  return message;
}
String getRead_DHT11_sala(){
  String message = "Temperature: " + String(s_temp_sala) + " ºC \n";
         message += "Humidity: "   + String(s_humid_sala) + " % \n";
  return message;
}
String getRead_DHT11_lab(){
  String message = "Temperature: " + String(s_temp_lab) + " ºC \n";
         message += "Humidity: "   + String(s_humid_lab) + " % \n";
  return message;
}
String getRead_DHT11_3d(){
  String message = "Temperature: " + String(s_temp_3d) + " ºC \n";
         message += "Humidity: "   + String(s_humid_3d) + " % \n";
  return message;
}
