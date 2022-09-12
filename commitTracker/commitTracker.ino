#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "имя_сети";
const char* password = "пароль";
String url =  "http://hud0shnikgitapi.herokuapp.com/commits/hud0shnik";

// Фикс вывода только первого символа в строке
#define printIIC(args) Wire.write(args)
inline size_t LiquidCrystal_I2C::write(uint8_t value) {
  send(value, Rs);
  return 1;
}

LiquidCrystal_I2C lcd(0x27, 16, 2);
unsigned long lastCheck = 0;
unsigned long timeDelay = 10000;

void setup() {

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);

  WiFi.begin(ssid, password);
  lcd.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(800);
    lcd.print(".");
  }
  lcd.print("Connected!");

}

void loop() {

  if ((millis() - lastCheck) > timeDelay) {

    // Проверка wifi
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;

      http.begin(client, url.c_str());

      int httpResponseCode = http.GET();

      if (httpResponseCode > 0) {
        String jsonStr = http.getString();
        char json[200];
        jsonStr.toCharArray(json, 200);

        StaticJsonDocument<200> doc;

        DeserializationError error = deserializeJson(doc, json);

        if (error) {
          lcd.init();
          lcd.backlight();
          lcd.setCursor(0, 0);
          lcd.print("Error");
        }

        int commits = doc["commits"];
        lcd.init();
        lcd.backlight();
        lcd.setCursor(0, 0);
        lcd.print("Today GitHub");
        lcd.setCursor(0, 1);
        lcd.print("contributions:");
        lcd.setCursor(14, 1);
        lcd.print(commits);

      }
      else {
        lcd.init();
        lcd.backlight();
        lcd.setCursor(0, 0);
        lcd.print("Error");
      }
      http.end();
    }
    else {
      lcd.init();
      lcd.backlight();
      lcd.setCursor(0, 0);
      lcd.print("WiFi");
      lcd.setCursor(0, 1);
      lcd.print("Disconnected");
    }
    lastCheck = millis();
  }
}
