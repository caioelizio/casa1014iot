/* Author: Caio Elizio - caioelizio@gmail.com 62 98223-9516"
 * Date: 22/11/20
 * Definicoes LCD 16x2
 * Versao 1.0 lcd
 */
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//LiquidCrystal_I2C lcd(0x27,16,2);  //Esp LCD_Azul  D2=SDA D1=SCL 
//LiquidCrystal_I2C lcd(0x27,20,4);  //Arduino azul  A4=SDA A5=SCL set the LCD address to 0x27 for a 16 chars and 2 line display
//LiquidCrystal_I2C lcd(0x3F,16,2);  //Esp LCD_Verde D2=SDA D1=SCL 
  LiquidCrystal_I2C lcd(0x3F,16,2);  //Wemos D1R1Uno LCD_Verde D4=SDA D3=SCL 

void iniLCD();
void f_LCD_Inf_Sistema();
void f_LCD_Wifi();
void f_LCD_Wifi_erro();
void f_LCD_DigitalClock();
void f_LCD_DigitalClock_DHT11();
void f_LCD_f_printDigits_zero(int digits);
void f_LCD_DHT_erro();
void f_LCD_DHT_serv();
void f_LCD_DHT_Lab();
void f_atualiza_LCD();


void f_atualiza_LCD(){
  if ((i_segAtivo > 0) & (i_segAtivo < 4)){
    f_LCD_Inf_Sistema();
  }
  if ((i_segAtivo > 4) & (i_segAtivo < 20)){
    f_LCD_DigitalClock_DHT11();
  }
  if ((i_segAtivo > 20) & (i_segAtivo < 30)){
//    f_LCD_DigitalClock();
    f_LCD_Wifi();
  }
  if ((i_segAtivo > 30) & (i_segAtivo < 40)){
    f_LCD_DigitalClock_DHT11();
//    f_LCD_DHT_serv();
  }
  if ((i_segAtivo > 45) & (i_segAtivo < 59)){
    f_LCD_Inf_Sistema();
    //f_LCD_DHT_serv();
  }//*/
/*  if ((i_segAtivo > 0) & (i_segAtivo < 10)){
    f_LCD_DigitalClock();
  }
  if ((i_segAtivo > 10) & (i_segAtivo < 20)){
    f_LCD_Inf_Sistema();
  }
  if ((i_segAtivo > 20) & (i_segAtivo < 30)){
    f_LCD_Wifi();
  }
  if ((i_segAtivo > 30) & (i_segAtivo < 40)){
    f_LCD_DHT_serv();
  }
  if ((i_segAtivo > 40) & (i_segAtivo < 59)){
    f_LCD_DHT_Lab();
  }//*/

    if ( (horas >= 17) 
    or   (horas < 8) ) {
      f_LCD_Backlight(ON);    
    }else {
      f_LCD_Backlight(OFF);    
    }
}

void iniLCD() {
  lcd.begin (16,2);
  lcd.init();                      // initialize the lcd 
//  lcd.backlight();
  lcd.setBacklight(HIGH);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Iniciando LCD...");
  lcd.setCursor(0,1);
  lcd.print("LCD e modulo I2C");
//  lcd.setBacklight(LOW);

  delay(500);
  //f_LCD_Inf_Sistema();
  
//  lcd.createChar(5, degree);
}
void f_LCD_Inf_Sistema(){
//  lcd.clear();
  lcd.print("                ");
  lcd.setCursor(0,0);
//  lcd.print("Sistema: "); lcd.print(programa);
  lcd.print(programa);
  lcd.print("");
  lcd.print(versao);  
  lcd.print("                ");
  lcd.setCursor(0,1);
//  lcd.print("Versao:  "); lcd.print(versao);  
}
void f_LCD_Wifi(){
//  lcd.clear();
  lcd.print("                ");
  lcd.setCursor(0,0);
  lcd.print("Ativo:"); lcd.print(longTimeStr(millis() / 1000)); //tempo desde o ultimo reset
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("ip"); lcd.print(ipLocal);
}
void f_LCD_Wifi_erro(){
  lcd.clear();
//  lcd.print("                ");
  lcd.setCursor(0,0);
  lcd.print("ERRO WI-FI      ");
  lcd.setCursor(0,1);
  lcd.print("Sinal: "); lcd.print(l_rssi);
}
void f_LCD_DigitalClock_DHT11(){
//  lcd.clear();
  lcd.print("                ");
  lcd.setCursor(0, 0);
  if (ano != 1970){
//    lcd.print("Hora: "); 
    lcd.print(horas);
//    lcd.print(" - Data: "); 
    lcd.print(" - "); 
    lcd.print(diadomes); lcd.print("/");  lcd.print(mes);  lcd.print("/");  lcd.print(ano);
    f_LCD_f_printDigits_zero(minutos);
  }else{
    lcd.print(programa);
    lcd.print(i_horas); f_LCD_f_printDigits_zero(i_minutos);
//    lcd.print(" - "); 
//    lcd.print(diadomes); lcd.print("/");  lcd.print(mes);  lcd.print("/");  lcd.print(ano);
  }
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("T:"); lcd.print(f_temp); lcd.print(" H:"); lcd.print(f_humid);
}

//
void f_LCD_DigitalClock(){
//lcd.print("                ");
  lcd.clear();
  lcd.setCursor(0, 0);
  if (ano != 1970){
    lcd.print("Data: "); lcd.print(diadomes); lcd.print("/");  lcd.print(mes);  lcd.print("/");  lcd.print(ano);
    lcd.setCursor(0, 1);
//    lcd.print("Hora: "); lcd.print(horas); f_LCD_f_printDigits_zero(minutos);  f_LCD_f_printDigits_zero(segundos);  
    lcd.print("Hora: "); lcd.print(horas);
    f_LCD_f_printDigits_zero(minutos);
  }else{
    lcd.print("Falha Time!     ");
    lcd.setCursor(0, 1);
//    lcd.print("                ");
    lcd.print("Hora: "); lcd.print(i_horas);
//    lcd.print(i_minutos);
    f_LCD_f_printDigits_zero(i_minutos);
  }
}
void f_LCD_f_printDigits_zero(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  lcd.print(":");
  if(digits < 10)
    lcd.print('0');
  lcd.print(digits);
}
void f_LCD_DHT_serv(){
    lcd.clear();
//  lcd.print("                ");
    lcd.setCursor(0,0);
    lcd.print("Temp Umi Serv.  ");
    lcd.setCursor(0,1);
    lcd.print("T:"); lcd.print(f_temp); lcd.print(" H:"); lcd.print(f_humid);
}
void f_LCD_DHT_Lab(){
    lcd.clear();
//  lcd.print("                ");
    lcd.setCursor(0,0);
    lcd.print("Temp Umi - Lab  ");
    lcd.setCursor(0,1);
    lcd.print("T:"); lcd.print(s_temp_lab); lcd.print(" H:"); lcd.print(s_humid_lab);
}
void f_LCD_DHT_erro(){
  lcd.clear();
//lcd.print("                ");
  lcd.setCursor(0,0);
  lcd.print("Falha sensor DHT");
  lcd.setCursor(0,1);
  lcd.print("T: "); lcd.print(f_temp); lcd.print(" H: "); lcd.print(f_humid); lcd.print("");
//  myBroker.publish(TOPICO_PUBLISH_T, "ERRO");
//  myBroker.publish(TOPICO_PUBLISH_H, "ERRO");
}
void f_LCD_Backlight(boolean b_valor) {
//  Serial.println();
  if (b_valor == ON) {
    lcd.setBacklight(HIGH);
  } else {
    lcd.setBacklight(LOW);
  }
}
