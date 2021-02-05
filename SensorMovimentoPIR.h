/* Author: Caio Elizio - caioelizio@gmail.com 62 98223-9516"
 * Date: 08/08/20
 * MQTT Client
 * Version 1.0 Print3D
 */
//Prototypes
void f_SensorMovimentoPIR_v1();
void f_SensorMovimentoPIR_v2();
void f_SensorMovimentoPIR_v3();


void f_SensorMovimentoPIR_v1(){
  iSensorPIR = digitalRead(pinSensorPIR);
  if (iSensorPIR == 1)
  {
    i_ttl_movimento++;
    //f_LedESP(OFF);
    Serial.println(F("Movimento detectado!"));
  }else{
    //f_LedESP(OFF);
  }
//  Serial.print(F(" *"));
//  Serial.print(F("iSensorPIR: ")); Serial.println(iSensorPIR);
//    Serial.print(F("i_ttl_movimento: ")); Serial.println(i_ttl_movimento);

  if ((i_ttl_movimento == 10) 
  or (i_ttl_movimento == 30)
  or (i_ttl_movimento == 100)
  or (i_ttl_movimento == 500)
  ){
    Serial.println();
    Serial.print(F("i_ttl_movimento: ")); Serial.println(i_ttl_movimento);
  }//*/
}
void f_SensorMovimentoPIR_v2(){
  int valorSensorPIR = digitalRead(pinSensorPIR);  //Lendo o valor do sensor PIR. 1 quando detecta algum movimento e 0 quando não detecta.
  Serial.print(F("Valor do Sensor PIR: ")); Serial.println(valorSensorPIR);
  //Verificando se ocorreu detecção de movimentos
  if (valorSensorPIR == 1 && bMovimento == false){
    if (valorSensorPIR < 100){
      Serial.print(F("Valor do Sensor PIR: ")); Serial.println(valorSensorPIR);
      bMovimento = true;
      //f_LedESP(ON);
    }
  }
  if (valorSensorPIR == 1 && bMovimento == true){
    //Serial.println(F("pir 1 mov t"));
    //Serial.print(F("ValorSensorPIR: ")); Serial.println(valorSensorPIR);
  }
  if (bMovimento == true){
    bMovimento = false;
    //f_LedESP(OFF);
  }
}

void f_SensorMovimentoPIR_v3(){
  int valorSensorPIR = digitalRead(pinSensorPIR);  //Lendo o valor do sensor PIR. 1 quando detecta algum movimento e 0 quando não detecta.
  //Serial.print(F("Valor do Sensor PIR: ")); Serial.println(valorSensorPIR);
  Serial.print(F("valorSensorPIR: ")); Serial.println(valorSensorPIR);
  if (valorSensorPIR == 1){
    i_ttl_movimento++;
    if (i_ttl_movimento == 20){
      Serial.print(F("Movimentos: ")); Serial.println(i_ttl_movimento);
//      bMovimento = True;
      i_ttl_movimento = 0;
      f_LCD_Backlight(ON);
      //f_LedESP(ON);
    }
  }else{
//    long now = millis()/1000;
//    if (now - lastMsg > 1000){
//      lastMsg = now;
      Serial.println();
      Serial.println(F("LCD luz off"));
      Serial.print(F("lastMsg: ")); Serial.println(lastMsg);
      Serial.println(F("No motion"));
      Serial.println(F("<------------------------------------------------->"));
      f_LCD_Backlight(OFF);
      bMovimento = false;
//    f_LedESP(OFF);
//    }
  }
}
