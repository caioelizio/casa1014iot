/* Author: Caio Elizio - caioelizio@gmail.com 62 98223-9516"
 * Date: 08/08/20
 * Definicoes LCD 16x2
 * Versao 1.0 lcd
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
  LiquidCrystal_I2C lcd(0x27,16,2); //Esp LCD_Azul  D2=SDA D1=SCL 
//LiquidCrystal_I2C lcd(0x27,20,4); //Arduino azul  A4=SDA A5=SCL set the LCD address to 0x27 for a 16 chars and 2 line display
//LiquidCrystal_I2C lcd(0x3F,16,2); //Esp LCD_Verde D2=SDA D1=SCL 
//LiquidCrystal_I2C lcd(0x3F,16,2); //Wemos D1R1Uno LCD_Verde D4=SDA D3=SCL 

void iniLCD();
void f_LCD_Inf_Sistema();
void f_LCD_Wifi();
void f_LCD_Wifi_Erro();
void f_LCD_DigitalClock();
void f_LCD_f_printDigits_zero(int digits);
void f_LCD_DHT_erro();
void f_LCD_DHT11();
void f_atualiza_LCD();



void f_atualiza_LCD(){
  if ((i_segAtivo > 0) & (i_segAtivo < 10)){
    f_LCD_DigitalClock();
  }
  if ((i_segAtivo > 10) & (i_segAtivo < 20)){
    f_LCD_Inf_Sistema();
  }
  if ((i_segAtivo > 20) & (i_segAtivo < 30)){
    f_LCD_Wifi();
  }
  if ((i_segAtivo > 30) & (i_segAtivo < 40)){
    f_LCD_DHT11();
  }
  if ((i_segAtivo > 40) & (i_segAtivo < 40)){
  }
}


//char degree[10];
//uint8_t degree[10];
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
  //lcd.createChar(5, degree);
}
void f_LCD_Inf_Sistema(){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Sistema: "); lcd.print(programa);
    lcd.setCursor(0,1);
    lcd.print("Version:  "); lcd.print(Version);  
}
void f_LCD_Wifi(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Ativo:"); lcd.print(longTimeStr(millis() / 1000)); //tempo desde o ultimo reset
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("ip"); lcd.print(ipLocal);
}
void f_LCD_Wifi_erro(){
  lcd.clear();
//lcd.print("                ");
  lcd.setCursor(0,0);
  lcd.print("ERRO WI-FI      ");
  lcd.setCursor(0,1);
}

void f_LCD_DigitalClock(){
//lcd.print("                ");
  lcd.clear();
  lcd.setCursor(0, 0);
  //lcd.print(hour());  minute();  second();  
  if (ano != 1970){
//    lcd.print("Data: "); lcd.print(day()); lcd.print("/");  lcd.print(month());  lcd.print("/");  lcd.print(year());
    lcd.print("Data: "); lcd.print(diadomes); lcd.print("/");  lcd.print(mes);  lcd.print("/");  lcd.print(ano);
    lcd.setCursor(0, 1);
//    lcd.print("Hora: "); lcd.print(hour()); f_LCD_f_printDigits_zero(minute());  f_LCD_f_printDigits_zero(second());  
    lcd.print("Hora: "); lcd.print(horas); 
    f_LCD_f_printDigits_zero(minutos);
  }else{
    lcd.print("Falha Time!     ");
    lcd.setCursor(0, 1);
//    lcd.print("                ");
    lcd.print("Hora: "); lcd.print(i_horas);
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
void f_LCD_DHT11(){
  lcd.clear();
//lcd.print("                ");
  lcd.setCursor(0,0);
  lcd.print("Temp Umi Print3D");
  lcd.setCursor(0,1);
  lcd.print("T:"); lcd.print(f_temp); lcd.print(" H:"); lcd.print(f_humid);
//  lcd.print("T:"); lcd.print(2,f_temp); lcd.print(" H:"); lcd.print(f_humid);
}
void f_LCD_DHT_erro(){
  lcd.clear();
//lcd.print("                ");
  lcd.setCursor(0,0);
  lcd.print("Falha sensor DHT");
  lcd.setCursor(0,1);
  lcd.print("T: "); lcd.print(f_temp); lcd.print(" H: "); lcd.print(f_humid); lcd.print("");
}
void f_LCD_Backlight(boolean b_valor) {
//  Serial.println();
  if (b_valor == ON) {
    lcd.setBacklight(HIGH);
  } else {
    lcd.setBacklight(LOW);
  }
}
