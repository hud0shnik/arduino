#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Переменные для подключения к wi-fi
const char* ssid = "имя_сети";
const char* password = "пароль";

// Ссылка на апиху
String url =  "http://hud0shnikgitapi.herokuapp.com/commits/hud0shnik";

// Порт дисплея и его размеры
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Временная метка последней проверки
unsigned long lastCheck = 0;

// Задержка между проверками
unsigned long timeDelay = 1000000;

// Фикс вывода только первого символа в строке
#define printIIC(args) Wire.write(args)
inline size_t LiquidCrystal_I2C::write(uint8_t value) {
  send(value, Rs);
  return 1;
}

// Первичная настройка
void setup() {

  //  Инициализация дисплея
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);

  // Подключение к wi-fi
  WiFi.begin(ssid, password);
  lcd.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(800);
    lcd.print(".");
  }
  lcd.init();
  lcd.print("Connected!");

}

// Цикл работы
void loop() {

  // Проверка времени
  if ((millis() - lastCheck) > timeDelay) {

    // Проверка wifi
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;

      // http Реквест
      http.begin(client, url.c_str());
      int httpResponseCode = http.GET();

      // Проверка респонса
      if (httpResponseCode > 0) {

        // Перевод респонса в массив символов
        String jsonStr = http.getString();
        char json[200];
        jsonStr.toCharArray(json, 200);

        // Парсинг Json'a
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, json);

        // Проверка на ошибку парсинга
        if (error) {
          lcd.init();
          lcd.backlight();
          lcd.setCursor(0, 0);
          lcd.print("Error");
        }

        // Получение количества коммитов
        int commits = doc["commits"];

        // Вывод результата
        lcd.init();
        lcd.backlight();
        lcd.setCursor(0, 0);
        lcd.print("Today GitHub");
        lcd.setCursor(0, 1);
        lcd.print("contributions:");
        if (commits < 10) {
          lcd.setCursor(15, 1);
          lcd.print(commits);
        } else {
          lcd.setCursor(14, 1);
          lcd.print(commits);
        }

      }

      // Вывод ошибки респонса
      else {
        lcd.init();
        lcd.backlight();
        lcd.setCursor(0, 0);
        lcd.print("Error");
      }
      http.end();
    }

    // Вывод информации о недоступности сети
    else {
      lcd.init();
      lcd.backlight();
      lcd.setCursor(0, 0);
      lcd.print("WiFi");
      lcd.setCursor(0, 1);
      lcd.print("Disconnected");
    }

    // Запись временной метки последней проверки
    lastCheck = millis();
  }
}