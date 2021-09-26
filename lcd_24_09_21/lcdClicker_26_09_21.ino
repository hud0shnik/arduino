#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>
LiquidCrystal_PCF8574 lcd(0x27); // адрес

void setup() {
  lcd.begin(16, 2);// размер дисплея
  lcd.setBacklight(255); //яркость
  lcd.clear();
  lcd.print(" :^)");
  lcd.setCursor(1, 1);// переход на 2ую строчку
  lcd.print("Click to start!");
  pinMode(8, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
}

bool btn1 = false;
bool btn2 = false;
int num = 0;

void loop() {
  bool btnState = !digitalRead(8);
  if (btnState && !btn1) {
    btn1 = true;
    num++;
    lcd.clear();
    lcd.print("n: ");
    lcd.setCursor(4, 0);
    lcd.print(num);
  }
  if (!btnState && btn1) {
    btn1 = false;
  }
  //
  bool btnState2 = !digitalRead(7);
  if (btnState2 && !btn2) {
    btn2 = true;
    num--;
    lcd.clear();
    lcd.print("n: ");
    lcd.setCursor(4, 0);
    lcd.print(num);
  }
  if (!btnState2 && btn2) {
    btn2 = false;
  }
}
