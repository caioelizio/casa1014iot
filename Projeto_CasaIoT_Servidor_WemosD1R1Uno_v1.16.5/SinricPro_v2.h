/* Author: Caio Elizio - caioelizio@gmail.com 62 98223-9516"
 * Date: 08/08/20
 * Definicoes SinricPro  
 * Version 1.0 
 */

#include "SinricPro.h"
#include "SinricProSwitch.h"

#define APP_KEY           "83e1a7c2-e44e-4d57-83a1-ad2dacb8f567"
#define APP_SECRET        "f25b1dfb-001c-480e-9db3-60808da740e0-08ccc827-e765-4a6d-98c8-c75c76441ea5"

#define LuzServidor_ID       "5ffe5b9c3de8d156e7d53687"    // Algo como "5dc1564130xxxxxxxxxxxxxx"
#define VentiladorSala_ID    "5ffd08fadfd58f56bc43c322"    // Algo como "5dc1564130xxxxxxxxxxxxxx"
#define VentiladorQuarto_ID  "5ffd0b3c3de8d156e7d51999"    // Algo como "5dc1564130xxxxxxxxxxxxxx"
#define LuzQuarto_ID         ""    // Algo como "5dc1564130xxxxxxxxxxxxxx"

#define LuzServidor_Pin pinRele1  // O pino fisico onde está ligado

void setupSinricPro();
bool VentiladorState(const String &deviceId, bool &state);

// setup das funções para o SinricPro
void setupSinricPro() {
  // add devices and callbacks to SinricPro
 
  SinricProSwitch &mySwitch4 = SinricPro[LuzServidor_ID];
  mySwitch4.onPowerState(VentiladorState);
 
  // setup SinricPro
  SinricPro.onConnected([](){ Serial.printf("Conectado a nuvem SinricPro\r\n"); }); 
  SinricPro.onDisconnected([](){ Serial.printf("Desconectado a nuvem SinricPro\r\n"); });
  SinricPro.begin(APP_KEY, APP_SECRET);
}
bool VentiladorState(const String &deviceId, bool &state) {
  Serial.printf("O ventilador foi %s\r\n", state?"ligado":"desligado");
  digitalWrite(pinRele1, state);
//  digitalWrite(LuzServidor_Pin, state);
  return true; // request handled properly
}
