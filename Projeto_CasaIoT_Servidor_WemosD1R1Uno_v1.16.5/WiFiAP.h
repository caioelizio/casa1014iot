/*
testar se nao conectar entra modo AP...
*/

#include <ESP8266WiFi.h>
#include "uMQTTBroker.h"
uMQTTBroker myBroker;

/*
 * Your WiFi config here
 */
char ssid[] = "ssid";     // your network SSID (name)
char pass[] = "password"; // your network password
bool WiFiAP = false;      // Do yo want the ESP as AP?

/*
 * WiFi init stuff
 */
void initWiFi(){
  Serial.println("Connecting to "+(String)ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }

  Serial.println();
  Serial.println(F("WiFi connected"));
  Serial.println("IP address: " + WiFi.localIP().toString());
}

void startWiFiAP(){
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, pass);
  Serial.println(F("AP started"));
  Serial.println("IP address: " + WiFi.softAPIP().toString());
}

void setup(){
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  if (WiFiAP) startWiFiAP(); else initWiFi();

  // Start the broker
  Serial.println(F("Starting MQTT broker"));
  myBroker.init();
}

/*void loop(){   
  //do anything here
  delay(1000);
}//*/
