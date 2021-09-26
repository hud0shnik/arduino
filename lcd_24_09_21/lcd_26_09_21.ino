#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>
LiquidCrystal_PCF8574 lcd(0x27); // адрес

void setup() {
  lcd.begin(16, 2);// размер дисплея
  lcd.setBacklight(255); //яркость
  lcd.clear();
  lcd.print(" =^)");
  lcd.setCursor(2, 1);// переход на 2ую строчку
  lcd.print("-_-");
  pinMode(8, INPUT_PULLUP);
  pinMode(7, OUTPUT);
}
bool flag = false;
bool flag2 = false;
int nn = 0;
void loop() {
  bool btnState = !digitalRead(8);
  if (btnState && !flag) {  // обработчик нажатия
    flag = true;
    flag2 = !flag2;
    lcd.clear();
    lcd.print(nn);
    nn++;
  }
  if (!btnState && flag) {  // обработчик отпускания
    flag = false;
  }

}
