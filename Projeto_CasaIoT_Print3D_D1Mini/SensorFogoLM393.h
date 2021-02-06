/* Author: Caio Elizio - caioelizio@gmail.com 62 98223-9516"
 * Date: 25/11/20
 * Sensor Fogo LM393
 * Versao 1.0 
 */
//Prototypes
void f_SensorFogoLM393();
 
void f_SensorFogoLM393() {
  if (digitalRead(pinSensorFogo) == LOW) {
    Serial.print(F("pinSensorFogo")); Serial.println(pinSensorFogo);
    Serial.println();
    Serial.println(F("Sensor detector de fogo: ON"));
    //f_LCD_SensorFogoLM393();
    f_buzzbeep();
    digitalWrite(pinLedonboard, HIGH); //ACENDE O LED
  } else {
    digitalWrite(pinLedonboard, LOW);  //APAGA O LED
  }
}
