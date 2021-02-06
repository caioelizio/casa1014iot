/* Author: Caio Elizio - caioelizio@gmail.com 62 98223-9516"
 * Date: 01/11/20
 * Definicoes firebase
 * Versao 1.0
 * https://console.firebase.google.com/u/0/project/irrigacao-iot/database/irrigacao-iot/data
 */

//FirebaseESP8266.h must be included before ESP8266WiFi.h
#include "FirebaseESP8266.h"

//casa irrigacao
  //#define FIREBASE_HOST "https://irrigacao-iot.firebaseio.com/" //Without http:// or https:// schemes
  //#define FIREBASE_AUTH "GQf9Y2tO34wMXzLDzeK2hk61GCiohvd3zegrCCKr"

//casa1014
  #define FIREBASE_HOST "https://casa1014-48a5a.firebaseio.com/" //Without http:// or https:// schemes
  #define FIREBASE_AUTH "FRBwJAU6KeQCPM01833Gf3RowEExk1bB3smNcJJu" //casa irrigacao

  //var
  String path    = "/SensorDHT11";
  String jsonStr = "";
  FirebaseJson     json1;
  FirebaseJsonData jsonObj;

//Define Firebase Data object
FirebaseData firebaseData;
void iniFirebase();
void FirebaseSet();
void FirebaseGet();
void FirebaseDHT11();
void printResult(FirebaseData &data);


void iniFirebase(){
  //Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
  firebaseData.setBSSLBufferSize(1024, 1024);
  //Set the size of HTTP response buffers in the case where we want to work with large data.
  firebaseData.setResponseSize(1024);
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
    
  //FirebaseDHT11();
}

void FirebaseSet(){
  Serial.println("------------------------------------");
  Serial.println("Set JSON");
  if (Firebase.set(firebaseData, path, json1)){
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.print("VALUE: ");  printResult(firebaseData);
  }else{
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
  }
  Serial.println("------------------------------------");
  Serial.println();
}
void FirebaseGet(){
  Serial.println("------------------------------------");
  Serial.println("Get JSON");
  if (Firebase.get(firebaseData, path)){
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());

    Serial.print("VALUE: ");
    if (firebaseData.dataType() == "json"){
      printResult(firebaseData);
    }
  }
  else{
      Serial.println("FAILED");
      Serial.println("REASON: " + firebaseData.errorReason());
  }
  Serial.println("------------------------------------");
  Serial.println();  
}


void FirebaseDHT11(){
  json1.set("Temp",   f_temp);
  json1.set("Hum",    f_humid);
  json1.set("test",   "test");
  json1.set("status", true);
  
  FirebaseSet();
  FirebaseGet();

  Serial.println("------------FirebaseDHT11-----------");
  json1 = firebaseData.jsonObject();
  json1.get(jsonObj, "Temp");
  Serial.println("Temp: " + jsonObj.stringValue);
  json1 = firebaseData.jsonObject();
  json1.get(jsonObj, "Hum");
  Serial.println("Hum: " + jsonObj.stringValue);
  json1 = firebaseData.jsonObject();
  json1.get(jsonObj, "test");
  Serial.println("test: " + jsonObj.stringValue);
  json1 = firebaseData.jsonObject();
  json1.get(jsonObj, "status");
  Serial.println("status: " + jsonObj.stringValue);
  Serial.println("------------------------------------");
  Serial.println();
//  Serial.print("Temp: "); Serial.println(String(f_temp));
//  Serial.print("Hum:  "); Serial.println(String(f_humid));
}

void printResult(FirebaseData &data){
  if (data.dataType() == "int")          Serial.println(data.intData());
  else if (data.dataType() == "float")   Serial.println(data.floatData(), 5);
  else if (data.dataType() == "double")  printf("%.9lf\n", data.doubleData());
  else if (data.dataType() == "boolean") Serial.println(data.boolData() == 1 ? "true" : "false");
  else if (data.dataType() == "string")  Serial.println(data.stringData());
  else if (data.dataType() == "json") {
    Serial.println();
    FirebaseJson &json = data.jsonObject();
    //Print all object data
    Serial.println("Pretty printed JSON data:");
    String jsonStr;
    json.toString(jsonStr, true);
    Serial.println(jsonStr);
    Serial.println();
    Serial.println("Iterate JSON data:");
    Serial.println();
    size_t len = json.iteratorBegin();
    String key, value = "";
    int type = 0;
    for (size_t i = 0; i < len; i++){
      json.iteratorGet(i, type, key, value);
      Serial.print(i);
      Serial.print(", ");
      Serial.print("Type: ");
      Serial.print(type == FirebaseJson::JSON_OBJECT ? "object" : "array");
      if (type == FirebaseJson::JSON_OBJECT)
      {
          Serial.print(", Key: ");
          Serial.print(key);
      }
      Serial.print(", Value: ");
      Serial.println(value);
    }
    json.iteratorEnd();
  }
  else if (data.dataType() == "array"){
    Serial.println();
    //get array data from FirebaseData using FirebaseJsonArray object
    FirebaseJsonArray &arr = data.jsonArray();
    //Print all array values
    Serial.println("Pretty printed Array:");
    String arrStr;
    arr.toString(arrStr, true);
    Serial.println(arrStr);
//        Serial.println();
    Serial.println("Iterate array values:");
    for (size_t i = 0; i < arr.size(); i++){
      Serial.print(i);
      Serial.print(", Value: ");
      FirebaseJsonData &jsonData = data.jsonData();
      //Get the result data from FirebaseJsonArray object
      arr.get(jsonData, i);
      if (jsonData.typeNum == FirebaseJson::JSON_BOOL)
          Serial.println(jsonData.boolValue ? "true" : "false");
      else if (jsonData.typeNum == FirebaseJson::JSON_INT)
          Serial.println(jsonData.intValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_FLOAT)
          Serial.println(jsonData.floatValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_DOUBLE)
          printf("%.9lf\n", jsonData.doubleValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_STRING ||
               jsonData.typeNum == FirebaseJson::JSON_NULL ||
               jsonData.typeNum == FirebaseJson::JSON_OBJECT ||
               jsonData.typeNum == FirebaseJson::JSON_ARRAY)
          Serial.println(jsonData.stringValue);
      }
  }
  else if (data.dataType() == "blob"){
    Serial.println();
    for (int i = 0; i < data.blobData().size(); i++){
      if (i > 0 && i % 16 == 0) Serial.println();
      if (i < 16)               Serial.print("0");
      Serial.print(data.blobData()[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
  else if (data.dataType() == "file"){
    Serial.println();
    File file = data.fileStream();
    int i = 0;
    while (file.available()){
      if (i > 0 && i % 16 == 0) Serial.println();
      int v = file.read();
      if (v < 16) Serial.print("0");
      Serial.print(v, HEX);
      Serial.print(" ");
      i++;
    }
    Serial.println();
    file.close();
  }
  else{
    Serial.println(data.payload());
  }
}
