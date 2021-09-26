#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>
LiquidCrystal_PCF8574 lcd(0x27); // адрес

void setup() {
  lcd.begin(16, 2);// размер дисплея
  lcd.setBacklight(255); //яркость
  lcd.clear();
  lcd.print(" :^)");
  lcd.setCursor(2, 1);// переход на 2ую строчку
  lcd.print("Hello!");
  pinMode(8, INPUT_PULLUP);
}

bool flag = false;
bool flag2 = false;
int nn = 0;
int u = 0;

void loop() {
  bool btnState = !digitalRead(8);
  if (btnState && !flag) {  // обработчик нажатия
    flag = true;
    lcd.clear();
    lcd.setCursor(nn, u);
    lcd.print("0");
    nn++;
    if (nn == 16){
      nn = 0;
      flag2 = !flag2;
    }
    if (flag2){
      u = 1;
    }else{
      u = 0;
    }
  }
  if (!btnState && flag) {  // обработчик отпускания
    flag = false;
  }

}
