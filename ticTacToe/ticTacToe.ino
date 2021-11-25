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

int botChoice;
char m[9] = {'O', ' ', 'X', ' ', ' ', ' ', ' ', 'O', ' '};

void setup(){
  lcd.init(SCR_WD, SCR_HT);
  lcd.fillScreen(BLACK);
  lcd.drawRect(0,0,240,240,BLUE);
  lcd.setTextColor(WHITE);
  lcd.setTextSize(6);
  lcd.setCursor(24,110);
  lcd.println("SETUP...");
  lcd.setTextColor(BLACK,WHITE);
}

void printMap() {
  lcd.fillScreen(BLACK);
  lcd.setTextColor(WHITE);
  lcd.setCursor(0,0);
 lcd.print(m[0]);
 lcd.print("|");
 lcd.print(m[1]);
 lcd.print("|");
 lcd.println(m[2]);
 lcd.println("-+-+-");
 lcd.print(m[3]);
 lcd.print("|");
 lcd.print(m[4]);
 lcd.print("|");
 lcd.println(m[5]);
 lcd.println("-+-+-");
 lcd.print(m[6]);
 lcd.print("|");
 lcd.print(m[7]);
 lcd.print("|");
 lcd.println(m[8]);
}

void loop(){
 printMap();
  delay(1000);
}
