/* Author: Caio Elizio - caioelizio@gmail.com 62 98223-9516"
 * Date: 07/08/20
 * MQTTBroker uSeverMQTTBroker.h 
 * Versao 1.0 Servidor
 */
#include "uMQTTBroker.h"  

//Prototypes
void initMQTT();
void Broker_pub(String s);
void EnviaEstadoOutputMQTT();
void EnviaMnsMQTT();

//Custom broker class with overwritten callback functions
class myMQTTBroker: public uMQTTBroker{
public:
    virtual bool onConnect(IPAddress addr, uint16_t client_count) {
//      Serial.print(F("Novo Cliente MQTT Broker: "));
      Serial.print("Novo Cliente MQTT Broker: ");
      Serial.print(addr.toString());
      Serial.println(" connected");
      return true;
    }

    virtual bool onAuth(String username, String password) {
//    virtual bool onAuth(String(USER_MQTT), String(PASS_MQTT)) {
//      Serial.println(F("Username/Password: "+USER_MQTT+"/"+PASS_MQTT));
      return true;
    }
    
    virtual void onData(String topic, const char *data, uint32_t length) {
      char data_str[length+1];
      os_memcpy(data_str, data, length);
      data_str[length] = '\0';
      String msgRecebida = (String)data_str;

      if (topic == "casa1014/serv/t"){
        //s_temp  = msgRecebida;
      }
      if (topic == "casa1014/serv/h"){
        //s_humid = msgRecebida;
      }
      if (topic == "casa1014/qrt/t"){
        s_temp_qrt  = msgRecebida;
      }
      if (topic == "casa1014/qrt/h"){
        s_humid_qrt = msgRecebida;
      }
      if (topic == "casa1014/lab/t"){
        s_temp_lab  = msgRecebida;
      }
      if (topic == "casa1014/lab/h"){
        s_humid_lab = msgRecebida;
      }
      if (topic == "casa1014/sala/t"){
        s_temp_sala  = msgRecebida;
      }
      if (topic == "casa1014/sala/h"){
        s_humid_sala = msgRecebida;
      }
      if (topic == "casa1014/3d/t"){
        s_temp_3d  = msgRecebida;
      }
      if (topic == "casa1014/3d/h"){
        s_humid_3d = msgRecebida;
      }
      //
      if (topic == "casa1014/serv/sub"){
        if (msgRecebida == "somteclasalaon"){
          f_AtivaSomTeclas(ON);
        }
        if (msgRecebida == "somteclasalaoff"){
          f_AtivaSomTeclas(OFF);
        }
        if (msgRecebida == "r1on"){
          rele1(ON);
          f_buzzbeep();
        }
        if (msgRecebida == "r1off"){
          rele1(OFF);
          f_buzzbeep();
        }
        if (msgRecebida == "tudoff"){
          tudoff();
          f_buzzbeep();
        }
      }
      
/*      Serial.println();
      Serial.println(F("<----------------------MQTT----------------------->"));  
      Serial.print(  "topic:       "); Serial.println(topic);
      Serial.print(  "msgRecebida: "); Serial.println(msgRecebida);
      Serial.println(F("<------------------------------------------------->"));
//*/
    }

};
  myMQTTBroker myBroker;
void initMQTT(){ //Função: inicializa conexão MQTT
  myBroker.init();
  myBroker.subscribe("#");
}
void Broker_pub(String s) { // Gera log myBroker
  myBroker.publish(TOPICO_PUBLISH, s);
}
//Função: envia ao Broker o estado atual do output 
void EnviaEstadoOutputMQTT(void) {
}
void EnviaMnsMQTT() {
  myBroker.publish("casa1014/serv/horas"      , String(i_horas));
  myBroker.publish("casa1014/serv/minutos"    , String(i_minutos));
  myBroker.publish("casa1014/serv/sistema"    , programa);
  myBroker.publish("casa1014/serv/versao"     , versao);
//myBroker.publish("casa1014/serv/bootCount"  , bootCount);
//myBroker.publish("casa1014/serv/TempoAtivo" , longTimeStr(millis() / 1000));
//myBroker.publish("casa1014/serv/ipLocal"    , ipStr(ipLocal)); 
  char buffer[2];
//myBroker.publish(TOPICO_PUBLISH_T, dtostrf(f_temp, 0, 0, buffer));
  myBroker.publish(TOPICO_PUBLISH_T, s_temp);
//myBroker.publish(TOPICO_PUBLISH_H, dtostrf(f_humid,0, 0, buffer));
  myBroker.publish(TOPICO_PUBLISH_H, s_humid);
}
