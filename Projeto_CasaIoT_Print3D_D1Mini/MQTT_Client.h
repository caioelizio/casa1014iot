/* Author: Caio Elizio - caioelizio@gmail.com 62 98223-9516"
 * Date: 08/08/20
 */
//Prototypes
void initMQTT();
void reconnectMQTT_v1();
void reconnectMQTT_v2();
void VerificaConexoesWiFIEMQTT(void);

//Definicoes mqtt
//Função: inicializa conexão MQTT(endereço do broker, porta e seta função de callback)
void initMQTT(){
  MQTT.setServer(HOST_MQTT, PORT_MQTT); //informa qual broker e porta deve ser conectado
//  MQTT.setServer(HOST_MQTT, 1883);          //informa qual broker e porta deve ser conectado
  MQTT.setCallback(mqtt_callback);          //atribui função de callback (função chamada quando qualquer 
                                              //informação de um dos tópicos subescritos chega)
  VerificaConexoesWiFIEMQTT();
}
//Função: reconecta-se ao broker MQTT (caso ainda não esteja conectado ou em caso de a conexão cair)
//        em caso de sucesso na conexão ou reconexão, o subscribe dos tópicos é refeito.
char msg[50];
long l_timeMQTT = 0;
int  i_contTentMQTT = 0;
void reconnectMQTT_v1() {
    while (!MQTT.connected()) {
        Serial.println("");
        Serial.print("Broker MQTT: ");  Serial.println(HOST_MQTT);
        Serial.print("Broker PORT: ");  Serial.println(PORT_MQTT);
        if (MQTT.connect(ID_MQTT)) {
            Serial.println("Cotectado Broker MQTT");
            Serial.println("");
            Serial.print(F("Broker MQTT: "));      Serial.println(HOST_MQTT);
            Serial.print(F("Broker PORT: "));      Serial.println(PORT_MQTT);
            Serial.print("TOPICO_SUBSCRIBE: ");  Serial.println(TOPICO_SUBSCRIBE);
            Serial.print("TOPICO_PUBLISH: ");    Serial.println(TOPICO_PUBLISH);
            Serial.println("");
            MQTT.subscribe(TOPICO_SUBSCRIBE); 
        }
        else {
//            Serial.println("Falha ao reconectar no broker.");
//            Serial.println("Havera nova tentatica de conexao em 2s");

            long now = millis()/1000;
            if (now - l_timeMQTT > 3) { // delay(2000);
              l_timeMQTT = now;
              ++i_contTentMQTT;
              snprintf (msg, 75, "Falha ao reconectar no broker... recnectando em 3 seg. #%ld", i_contTentMQTT);
              Serial.print("Publish message: "); Serial.println(msg);
//              Serial.println("Falha ao reconectar no broker... recnectando em 3 seg.");
              MQTT.publish(TOPICO_PUBLISH, msg);
            }//*/   
        }
    }
}
void reconnectMQTT_v2(){
//    if (MQTT.connect(ID_MQTT,USER_MQTT,PASS_MQTT)){
    if (MQTT.connect(ID_MQTT)){
      Serial.println(); //Linha em branco
      Serial.println(F("Conectado Broker MQTT"));
      Serial.print(F("Broker MQTT: "));      Serial.println(HOST_MQTT);
      Serial.print(F("Broker PORT: "));      Serial.println(PORT_MQTT);
      Serial.print(F("TOPICO_SUBSCRIBE: ")); Serial.println(TOPICO_SUBSCRIBE);
      Serial.print(F("TOPICO_PUBLISH: "));   Serial.println(TOPICO_PUBLISH);
      MQTT.subscribe(TOPICO_SUBSCRIBE);
//    MQTT.publish(TOPICO_PUBLISH, "Conectado Broker MQTT");
    } else {
      long now = millis()/1000;
      if (now - l_timeMQTT > 3){
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
void VerificaConexoesWiFIEMQTT(void) {
//    if (!MQTT.connected()) reconnectMQTT_v1(); //se não há conexão com o Broker, a conexão é refeita
    if (!MQTT.connected()) reconnectMQTT_v2(); //se não há conexão com o Broker, a conexão é refeita
    reconectWiFi();                         //se não há conexão com o WiFI, a conexão é refeita
}
//Função: função de callback é chamada toda vez que uma informação de um dos tópicos subescritos chega
