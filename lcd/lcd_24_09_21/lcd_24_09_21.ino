#include <LiquidCrystal_PCF8574.h>
#include <Wire.h> 
LiquidCrystal_PCF8574 lcd(0x27); // адрес

void setup() {
lcd.begin(16, 2);// размер дисплея
lcd.setBacklight(255); //яркость
lcd.clear(); 
lcd.print(" UwU");
lcd.setCursor(2, 1);// переход на 2ую строчку
lcd.print("OwO");
}

void loop() {

}
