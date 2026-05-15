// ESP32 DevKit1 (WROOM) + GMT130 IPS 240x240 (ST7789)
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <U8g2_for_Adafruit_GFX.h>

/* ============ ПИНЫ ДЛЯ ESP32 DevKit1 ============
   Дисплей GMT130 (аппаратный SPI VSPI по умолчанию):
   SCK  -> GPIO18
   MOSI -> GPIO23
   MISO -> не используется (можно GPIO19)
   CS   -> GPIO5
   DC   -> GPIO17
   RST  -> GPIO16
   Кнопка, зуммер, светодиоды:
   BTN        -> GPIO4  (подтяжка INPUT_PULLUP, нажатие = LOW)
   BUZZER     -> GPIO25 (пассивный зуммер, ШИМ‑сигнал)
   LED_POWER  -> GPIO27 (HIGH = включены светодиоды во время ASCII‑арта)
   BOARD_LED  -> GPIO2  (встроенный синий LED, активный HIGH)
*/
#define TFT_CS     5
#define TFT_DC    17
#define TFT_RST   16
#define BTN_PIN    4
#define BUZZER_PIN 22
#define BUZZER_DUTY_PERCENT 80   // громкость зуммера (duty cycle, 1–100%)
#define LED_POWER_PIN 21
#define BOARD_LED_PIN  2        // встроенный светодиод DevKit1 (GPIO2, HIGH = горит)

// Пины аппаратного SPI (VSPI по умолчанию)
#define TFT_SCK  18
#define TFT_MOSI 23
#define TFT_MISO -1              // не используется

const int timerCycleMin = 45;    // длительность таймера в минутах

// Цвета начального экрана (RGB565)
#define COL_RGB565(r, g, b) ((((r)>>3)<<11) | (((g)>>2)<<5) | ((b)>>3))
#define color_one   COL_RGB565(0x5a, 0x39, 0x21)  // #5a3921
#define color_two   COL_RGB565(0x6b, 0x8c, 0x42)  // #6b8c42
#define color_three COL_RGB565(0x7b, 0xc6, 0x7b)  // #7bc67b
#define color_four  COL_RGB565(0xff, 0xff, 0xb5)  // #ffffb5

// Аппаратный SPI (CS, DC, RST заданы, SPI будет настроен в setup)
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;

void setup() {
  // Встроенный светодиод DevKit1 — выключаем (LOW, т.к. активный HIGH)
  pinMode(BOARD_LED_PIN, OUTPUT);
  digitalWrite(BOARD_LED_PIN, LOW);

  Serial.begin(115200);
  Serial.println("Инициализация дисплея GMT130 (ESP32)...");

  // Ручной сброс дисплея
  pinMode(TFT_RST, OUTPUT);
  digitalWrite(TFT_RST, HIGH);
  delay(20);
  digitalWrite(TFT_RST, LOW);
  delay(20);
  digitalWrite(TFT_RST, HIGH);
  delay(150);

  // Аппаратный SPI на пинах VSPI
  SPI.begin(TFT_SCK, TFT_MISO, TFT_MOSI);

  // Инициализация 240x240; для многих GMT130 нужен именно SPI_MODE2
  tft.init(240, 240, SPI_MODE2);
  tft.setRotation(2);  // 180° — картинка не вверх ногами
  u8g2_for_adafruit_gfx.begin(tft);  // подключить шрифты U8g2

  pinMode(BTN_PIN, INPUT_PULLUP);    // кнопка: нажатие = LOW
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_POWER_PIN, OUTPUT);
  digitalWrite(LED_POWER_PIN, LOW);  // светодиоды выключены до показа ASCII-арта

  drawStartScreen();
}

// Отрисовка начального экрана (крупный шрифт, кириллица + время таймера)
void drawStartScreen() {
  tft.fillScreen(color_one);
  u8g2_for_adafruit_gfx.setFont(u8g2_font_inr24_t_cyrillic);
  u8g2_for_adafruit_gfx.setFontMode(1);

  int ascent = u8g2_for_adafruit_gfx.getFontAscent();
  int descent = u8g2_for_adafruit_gfx.getFontDescent();
  int lineH = (ascent - descent > 0) ? (ascent - descent) : 26;
  int y0 = 18;
  int x0 = 10;

  u8g2_for_adafruit_gfx.setForegroundColor(color_four);
  u8g2_for_adafruit_gfx.setCursor(x0, y0 + ascent);
  u8g2_for_adafruit_gfx.print("Жопотаймер");

  u8g2_for_adafruit_gfx.setCursor(x0, y0 + ascent + lineH);
  u8g2_for_adafruit_gfx.setForegroundColor(color_two);
  u8g2_for_adafruit_gfx.print(" от ");
  u8g2_for_adafruit_gfx.setForegroundColor(color_three);
  u8g2_for_adafruit_gfx.print("Данечки");

  u8g2_for_adafruit_gfx.setCursor(x0, y0 + ascent + 2 * lineH);
  u8g2_for_adafruit_gfx.setForegroundColor(color_four);
  u8g2_for_adafruit_gfx.print("Время: ");
  u8g2_for_adafruit_gfx.setForegroundColor(color_three);
  u8g2_for_adafruit_gfx.print(timerCycleMin);
  u8g2_for_adafruit_gfx.print("m");

  u8g2_for_adafruit_gfx.setCursor(x0, y0 + ascent + 6 * lineH);
  u8g2_for_adafruit_gfx.setForegroundColor(color_two);
  u8g2_for_adafruit_gfx.print("v0.4.1-rc");
}

void runTimer() {
  digitalWrite(LED_POWER_PIN, LOW);  // выключить светодиоды при переходе к таймеру
  tft.fillScreen(color_one);
  tft.setTextColor(color_four);
  tft.setTextSize(5);

  // Прямоугольник под таймер: считаем по самой широкой строке "Xm 59s", чтобы при 10m→9m ничего не съезжало
  char maxBuf[16];
  snprintf(maxBuf, sizeof(maxBuf), "%dm 59s", timerCycleMin);
  int16_t mx, my;
  uint16_t mw, mh;
  tft.getTextBounds(maxBuf, 0, 0, &mx, &my, &mw, &mh);
  int pad = 6;
  int rx = (240 - mw) / 2 - mx - pad;
  int ry = (240 - mh) / 2 - my - pad;
  int rw = mw + pad * 2;
  int rh = mh + pad * 2;
  if (rx < 0) { rw += rx; rx = 0; }
  if (ry < 0) { rh += ry; ry = 0; }
  if (rx + rw > 240) rw = 240 - rx;
  if (ry + rh > 240) rh = 240 - ry;

  // Ширина блока секунд под "59s" — одна и та же зона под секунды, без хвостов при 10s→9s
  char secMaxBuf[8];
  snprintf(secMaxBuf, sizeof(secMaxBuf), "59s");
  int16_t sx0, sy0;
  uint16_t secMaxW, secMaxH;
  tft.getTextBounds(secMaxBuf, 0, 0, &sx0, &sy0, &secMaxW, &secMaxH);

  int totalSec = timerCycleMin * 60;
  int sec = totalSec;
  int lastMinutes = -1;       // минуты в прошлом кадре — перерисовываем их только при смене
  int lastMinutesWidth = 0;   // ширина строки "Xm " в пикселях (для позиции секунд)
  int lastTimerX = 0, lastTimerY = 0;  // позиция последней полной отрисовки
  char minBuf[8];

  while (sec >= 1) {
    int minutes = sec / 60;
    int seconds = sec % 60;

    if (minutes != lastMinutes) {
      // Минуты изменились — перерисовываем весь таймер (центр строки не съезжает: учитываем bx, by)
      tft.fillRect(rx, ry, rw, rh, color_one);
      char countdownBuf[16];
      snprintf(countdownBuf, sizeof(countdownBuf), "%dm %ds", minutes, seconds);
      int16_t bx, by;
      uint16_t bw, bh;
      tft.getTextBounds(countdownBuf, 0, 0, &bx, &by, &bw, &bh);
      lastTimerX = (240 - bw) / 2 - bx;
      lastTimerY = (240 - bh) / 2 - by;
      tft.setCursor(lastTimerX, lastTimerY);
      tft.print(countdownBuf);
      lastMinutes = minutes;
      snprintf(minBuf, sizeof(minBuf), "%dm ", minutes);
      tft.getTextBounds(minBuf, 0, 0, &bx, &by, &bw, &bh);
      lastMinutesWidth = bw;
    } else {
      // Минуты те же — обновляем только блок секунд (всегда очищаем зону под "59s", без хвостов)
      char secBuf[8];
      snprintf(secBuf, sizeof(secBuf), "%ds", seconds);
      int secX = lastTimerX + lastMinutesWidth;
      if (secX < rx) secX = rx;
      int secW = secMaxW + 6;
      if (secX + secW > rx + rw) secW = rx + rw - secX;
      tft.fillRect(secX, ry, secW, rh, color_one);
      tft.setCursor(secX, lastTimerY);
      tft.print(secBuf);
    }

    // Секунда: проверка кнопки — сброс таймера
    unsigned long until = millis() + 1000;
    while (millis() < until) {
      if (digitalRead(BTN_PIN) == LOW) {
        delay(50);
        if (digitalRead(BTN_PIN) == LOW) {
          sec = totalSec + 1;  // сброс — старт с начала
          while (digitalRead(BTN_PIN) == LOW) delay(10);  // ждём отпускания
          break;
        }
      }
      delay(10);
      yield();
    }
    sec--;
  }

  tft.fillScreen(color_one);
  digitalWrite(LED_POWER_PIN, HIGH);  // питание светодиодов: включено пока на экране ASCII-арт
  drawAsciiArt();  // рисунок на экране во время проигрывания мелодии

  //  G C~ Eb G~ Eb F#~ Eb F Eb C A# G~  
  const float freq[] = { 196.0f, 261.6f, 261.6f, 311.1f, 392.0f, 392.0f, 311.1f, 370.0f, 370.0f, 311.1f, 349.2f, 311.1f, 261.6f, 233.1f, 196.0f, 196.0f };  // G4 C4 Eb4 G4 Eb4 F#4 Eb4 F4 C4 A#3 G4 G3
  const int numNotes = 16;
  const int noteMs = 220;

  for (int n = 0; n < numNotes; n++) {
    float f = freq[n];
    unsigned int periodUs = (unsigned int)(1000000.0f / f);
    unsigned int onUs = (unsigned int)(periodUs * BUZZER_DUTY_PERCENT / 100);
    if (onUs < 10) onUs = 10;
    unsigned int offUs = periodUs - onUs;
    const unsigned long noteEnd = millis() + noteMs;
    while (millis() < noteEnd) {
      digitalWrite(BUZZER_PIN, HIGH);
      delayMicroseconds(onUs);
      digitalWrite(BUZZER_PIN, LOW);
      delayMicroseconds(offUs);
      yield();
    }
  }
  digitalWrite(BUZZER_PIN, LOW);
}

const char* asciiArt[] = {
 " |`-.._____..-'|",
 " :  > .  ,  <  :",
 " `./ __`' __ \\,'",
 "  | (|_) (|_) |",
 "  ; _  .  __  :",
 "  `.,' - `-. ,'",
 "    `, `_  .'",
 "    /       \\",
 "   /         :",
 "  :          |_",
 " ,|  .    .  | \\",
 ": :   \\   |  |  :",
 "|  \\   :`-;  ;  |",
 ":   :  | /  /   ;",
 " :-.'  ;'  / _,'`------.",
 " `'`''-`'''-'-''--.---  )",
 "      Кнопку жми   `----'"
};
const int asciiArtLines = 17;

void drawAsciiArt() { 
  tft.fillScreen(color_one);

  u8g2_for_adafruit_gfx.setFont(u8g2_font_10x20_t_cyrillic);
  u8g2_for_adafruit_gfx.setFontMode(1);
  u8g2_for_adafruit_gfx.setForegroundColor(color_four);

  int maxLen = 0;
  for (int i = 0; i < asciiArtLines; i++) {
    int n = strlen(asciiArt[i]);
    if (n > maxLen) maxLen = n;
  }

  const int fontW = 10;
  const int fontLineH = 20;
  int x0 = 0;
  int y0 = 0;

  int ascent = u8g2_for_adafruit_gfx.getFontAscent();
  int descent = u8g2_for_adafruit_gfx.getFontDescent();
  int lineH = ascent - descent;
  if (lineH < 1) lineH = fontLineH;

  for (int row = 0; row < asciiArtLines; row++) {
    // В U8g2 setCursor(y) — это базовая линия символа (низ буквы)
    int baselineY = y0 + ascent + row * lineH;
    u8g2_for_adafruit_gfx.setCursor(x0, baselineY);
    u8g2_for_adafruit_gfx.print(asciiArt[row]);
  }
}

void loop() {
  // Кнопка нажата (LOW при подключении к GND)
  if (digitalRead(BTN_PIN) == LOW) {
    delay(50);  // антидребезг
    if (digitalRead(BTN_PIN) == LOW) {
      runTimer();
      // Ждём отпускания кнопки, чтобы не запускать таймер повторно
      while (digitalRead(BTN_PIN) == LOW) {
        delay(10);
      }
    }
  }
  delay(20);
}