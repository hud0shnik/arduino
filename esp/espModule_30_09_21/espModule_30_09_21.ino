#include <SoftwareSerial.h>
SoftwareSerial mySerial(8, 9);
#define WIFI_SERIAL    mySerial

void setup()
{
  // открываем последовательный порт для мониторинга действий в программе
  // и передаём скорость 9600 бод
  Serial.begin(9600);
  while (!Serial) {  }
  Serial.print("Serial init OK\r\n");
  // открываем Serial-соединение с Wi-Fi модулем
  WIFI_SERIAL.begin(9600);
}

void loop()
{
  // если приходят данные из Wi-Fi модуля - отправим их в порт компьютера
  if (WIFI_SERIAL.available()) {
    Serial.write(WIFI_SERIAL.read());
  }
  // если приходят данные из компьютера - отправим их в Wi-Fi модуль
  if (Serial.available()) {
    WIFI_SERIAL.write(Serial.read());
  }
}
