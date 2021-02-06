#include <ThingSpeak.h>
//#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

//aba wifi //WiFiClient espClient;
int writeTSData( long TSChannel, unsigned int TSField, float data );


// ThingSpeak information
//char serverThingSpeak[] = "api.thingspeak.com";
char* serverThingSpeak = "api.thingspeak.com";
char serverT[] = "api.thingspeak.com"; // ThingSpeak Server

unsigned long channelID = 281709;
char* readAPIKey = "J69O76WZLGRJ4V7H";
char* writeAPIKey = "UCLDTHE1HRMCM8VU";
unsigned int dataFieldOne = 1;  //Field to write temperature data
unsigned int dataFieldTwo = 2;  //Field to write temperature data
unsigned int aField = 1;        //Field to hold first constant of the thermistor calibration                
unsigned int bField = 2;        //Field to hold second constant of the thermistor calibration

int iTestRadom = random(0,100);
String myStatus = "";

void iniThingspeak() {
  ThingSpeak.begin(client);  // Initialize ThingSpeak

// Read the constants at startup.
//aConst = readTSData( channelID, aField );
}
int connectWifi(){
    while (WiFi.status() != WL_CONNECTED) {
       WiFi.begin(ssid, pw);
       delay(2500);
       Serial.println("Connecting to WiFi");
    }
    Serial.println("Connected");
    ThingSpeak.begin(client);
}

void Thingspeak(){

  connectWifi();  
      
  if (client.connect(serverT, 80)) {

  iTestRadom = random(0,100);
  // set the fields with the values
//  ThingSpeak.setField(1, iTestRadom);

  myStatus = String("iTestRadom ok");
  // set the status
//  ThingSpeak.setStatus(myStatus);

//  writeTSData( channelID , dataFieldOne , iTestRadom , dataFieldTwo , iTestRadom , iTestRadom , millis() );      // Write the temperature in F, C, and time since starting.
//  writeTSData( channelID , dataFieldOne , millis() );      // Write the temperature in F, C, and time since starting.
  writeTSData( channelID , dataFieldOne , iTestRadom );      // Write the temperature in F, C, and time since starting.
//  ThingSpeak.writeField( channelID, dataFieldOne, "40", writeAPIKey ); // Write the data to the channel
//  ThingSpeak.writeField( channelID, dataFieldTwo, "40", writeAPIKey ); // Write the data to the channel
  
/*  Serial.print("Temperature: "); Serial.print(f_temp);
    Serial.print(" degrees Celsius Humidity: "); Serial.print(f_humid);
//*/

    Serial.println("Sending data to Thingspeak");
  }else{
    Serial.println("Thingspeak erro!");    
  }//*/
//  client.stop();
}
 
float readTSData( long TSChannel,unsigned int TSField ){
  float data =  ThingSpeak.readFloatField( TSChannel, TSField, readAPIKey );
  Serial.println( " Data read from ThingSpeak: " + String( data, 9 ) );
  return data;

}

// Use this function if you want to write a single field.
int writeTSData( long TSChannel, unsigned int TSField, float data ){
  int  writeSuccess = ThingSpeak.writeField( TSChannel, TSField, data, writeAPIKey ); // Write the data to the channel
  if ( writeSuccess ){
    Serial.println( String(data) + " written to Thingspeak." );
  }
  return writeSuccess;
}

// Use this function if you want to write multiple fields simultaneously.
int write2TSData( long TSChannel, unsigned int TSField1, float field1Data, unsigned int TSField2, long field2Data, unsigned int TSField3, long field3Data ){
  ThingSpeak.setField( TSField1, field1Data );
  ThingSpeak.setField( TSField2, field2Data );
  ThingSpeak.setField( TSField3, field3Data );
   
  int writeSuccess = ThingSpeak.writeFields( TSChannel, writeAPIKey );
  return writeSuccess;
}
