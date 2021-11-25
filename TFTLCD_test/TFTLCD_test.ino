#include <SPI.h>
#include <Arduino_ST7789_Fast.h>

#define TFT_DC    7
#define TFT_RST   8 
#define SCR_WD   240
#define SCR_HT   240  
Arduino_ST7789 lcd = Arduino_ST7789(TFT_DC, TFT_RST);
/*
 #01 GND -> GND
 #02 VCC -> VCC 
 #03 SCK -> D13
 #04 SDA -> D11/MOSI
 #05 RES -> D8 
 #06 DC  -> D7 
 #07 BLK -> NOT USED
*/

int i=0;

void setup(){
  lcd.init(SCR_WD, SCR_HT);
  lcd.fillScreen(BLACK);
  lcd.drawRect(0,0,240,240,BLUE);
  lcd.setTextColor(WHITE);
  lcd.setTextSize(2);
  lcd.setCursor(24,110);
  lcd.println("SETUP...");
  lcd.setTextColor(BLACK,WHITE);
}

void loop(){
  lcd.fillScreen(BLACK);
  lcd.setTextColor(WHITE);
  lcd.setCursor(24,110);
  lcd.println(i);
  i++;
  delay(1000);
}
