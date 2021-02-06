/* Author: Caio Elizio - caioelizio@gmail.com 62 98223-9516"
 * Date: 08/08/20
 * Definicoes Sensor DHT11
 * Version 1.0 Print3D
 */  
//Prototypes
void iniDHT11();
void f_SensorDHT11();
void f_Serial_DHT11();

void iniDHT11() {
  Serial.println();
  Serial.println(F("Iniciando DHT11..."));
}
void f_SensorDHT11(){
  dht.begin();  
  if (dht.readTemperature()) f_temp  = dht.readTemperature();
  if (dht.readHumidity())    f_humid = dht.readHumidity();
  if (isnan(f_humid) || isnan(f_temp)){ //isnan() trabalha com números de ponto flutuante, não inteiros.
    Serial.println();
    Serial.println(F("Falha na leitura do sensor DHT11"));
    dhtRead(); //Lê configuração Json
//    f_LCD_DHT_erro();
//    return; //interrompe a funcao
    MQTT.publish(TOPICO_PUBLISH_T, "ERRO");
    MQTT.publish(TOPICO_PUBLISH_H, "ERRO");
  }
  else{ //se ok...
    dhtSave(); //Salva configuração Json
    Serial.println();
    Serial.println(F("Sensor DHT11 Atuazizado!"));
    char buffer[2];
    MQTT.publish(TOPICO_PUBLISH_T,dtostrf(f_temp, 0, 0, buffer));
    MQTT.publish(TOPICO_PUBLISH_H,dtostrf(f_humid,0, 0, buffer));
  }
//  f_Serial_DHT11();
}
void f_Serial_DHT11(){
  Serial.println();
  Serial.println(F("<--------------Temperatura e Umidade-------------->"));
  Serial.print(F("Temperatura: ")); Serial.print(f_temp,0);  Serial.print(F("oC"));
  Serial.print(F(" - Umidade: "));  Serial.print(f_humid,0); Serial.println(F("%"));
  Serial.println(F("<------------------------------------------------->"));
}
