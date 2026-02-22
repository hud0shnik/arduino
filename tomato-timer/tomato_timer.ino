

// ESP8266  + GMT130 ips 240*240
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <U8g2_for_Adafruit_GFX.h>

// Пины по схеме. На дисплее: GND, VCC, SCK, SDA, RES, DC, BLK
// SCK → D5 (GPIO14), SDA → D7 (GPIO13) — аппаратный SPI, не меняем
#define TFT_CS   15  // D8 (GPIO15) — на модуле нет CS, но пин нужен библиотеке (держим LOW)
#define TFT_DC   0   // DC → D3 (GPIO0), чтобы D4 (GPIO2) не трогать — на нём встроенный светодиод
#define TFT_RST  5   // RES → D1 (GPIO5)
#define BTN_PIN  12  // D6 (GPIO12) — кнопка: один вывод на D6, второй на GND
#define BUZZER_PIN 4 // D2 (GPIO4) — HW-508 (3 ноги): VCC→3.3V, GND→GND, I/O→D2
// Встроенный светодиод у RST на D4 (GPIO2). DC дисплея перенесён на D3 — этот пин держим HIGH, светодиод не горит.
#define BOARD_LED_PIN 2
const int timerCycleMin = 45;     // длительность таймера в минутах

// Цвета начального экрана (RGB565)
#define COL_RGB565(r, g, b) ((((r)>>3)<<11) | (((g)>>2)<<5) | ((b)>>3))
#define color_one   COL_RGB565(0x5a, 0x39, 0x21)  // #5a3921
#define color_two   COL_RGB565(0x6b, 0x8c, 0x42)  // #6b8c42
#define color_three COL_RGB565(0x7b, 0xc6, 0x7b)  // #7bc67b
#define color_four  COL_RGB565(0xff, 0xff, 0xb5)  // #ffffb5


// Аппаратный SPI (важно для стабильной работы): только CS, DC, RST
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;  // шрифты U8g2 с поддержкой UTF-8/кириллицы

void setup() {
  // Погасить встроенный светодиод у RST (на многих платах он на D4/GPIO2, активен по LOW)
  pinMode(BOARD_LED_PIN, OUTPUT);
  digitalWrite(BOARD_LED_PIN, HIGH);

  Serial.begin(115200);
  Serial.println("Инициализация дисплея GMT130...");

  // Сброс дисплея вручную (на всякий случай)
  pinMode(TFT_RST, OUTPUT);
  digitalWrite(TFT_RST, HIGH);
  delay(20);
  digitalWrite(TFT_RST, LOW);
  delay(20);
  digitalWrite(TFT_RST, HIGH);
  delay(150);

  // Инициализация 240x240; для многих GMT130 нужен именно SPI_MODE2
  tft.init(240, 240, SPI_MODE2);
  SPI.setFrequency(40000000);  // 40 МГц — быстрее отрисовка, меньше «рваного» обновления
  tft.setRotation(2);  // 180° — картинка не вверх ногами
  u8g2_for_adafruit_gfx.begin(tft);  // подключить шрифты U8g2 к дисплею

  pinMode(BTN_PIN, INPUT_PULLUP);  // кнопка: нажатие = LOW
  pinMode(BUZZER_PIN, OUTPUT);

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
  u8g2_for_adafruit_gfx.print("v0.3.2-rc");
}

void runTimer() {
  tft.fillScreen(color_one);
  tft.setTextColor(color_four);
  tft.setTextSize(5);

  // Один раз вычисляем прямоугольник под максимальную строку — обновляем только его
  char maxBuf[16];
  snprintf(maxBuf, sizeof(maxBuf), "%dm %ds", timerCycleMin, 0);
  int16_t mx, my;
  uint16_t mw, mh;
  tft.getTextBounds(maxBuf, 0, 0, &mx, &my, &mw, &mh);
  int timerX = (240 - mw) / 2 + mx;
  int timerY = (240 - mh) / 2 + my;
  // Запас по краям, чтобы не было артефактов
  int pad = 4;
  int rx = timerX - pad;
  int ry = timerY - pad;
  int rw = mw + pad * 2;
  int rh = mh + pad * 2;
  if (rx < 0) { rw += rx; rx = 0; }
  if (ry < 0) { rh += ry; ry = 0; }
  if (rx + rw > 240) rw = 240 - rx;
  if (ry + rh > 240) rh = 240 - ry;

  int totalSec = timerCycleMin * 60;
  int sec = totalSec;

  while (sec >= 1) {
    int minutes = sec / 60;
    int seconds = sec % 60;

    char countdownBuf[16];
    snprintf(countdownBuf, sizeof(countdownBuf), "%dm %ds", minutes, seconds);
    int16_t bx, by;
    uint16_t bw, bh;
    tft.getTextBounds(countdownBuf, 0, 0, &bx, &by, &bw, &bh);

    // Всё обновление в одной SPI-транзакции — экран не рвёт между fillRect и текстом
    tft.startWrite();
    tft.fillRect(rx, ry, rw, rh, color_one);
    tft.setCursor((240 - bw) / 2, (240 - bh) / 2);
    tft.print(countdownBuf);
    tft.endWrite();

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
  drawAsciiArt();  // рисунок на экране во время проигрывания мелодии

  //  G C~ Eb G~ Eb F#~ Eb F Eb C A# G~  
  const float freq[] = { 196.0f, 261.6f, 261.6f, 311.1f, 392.0f, 392.0f, 311.1f, 370.0f, 370.0f, 311.1f, 349.2f, 311.1f, 261.6f, 233.1f, 196.0f, 196.0f };  // G4 C4 Eb4 G4 Eb4 F#4 Eb4 F4 C4 A#3 G4 G3
  const int numNotes = 16;
  const int noteMs = 220;   // длительность ноты
  const int dutyPercent = 1;  

  for (int n = 0; n < numNotes; n++) {
    float f = freq[n];
    unsigned int periodUs = (unsigned int)(1000000.0f / f);
    unsigned int onUs = (unsigned int)(periodUs * dutyPercent / 100);
    if (onUs < 10) onUs = 10;
    unsigned int offUs = periodUs - onUs;
    const unsigned long noteEnd = millis() + noteMs;
    while (millis() < noteEnd) {
      digitalWrite(BUZZER_PIN, HIGH);
      delayMicroseconds(onUs);
      digitalWrite(BUZZER_PIN, LOW);
      delayMicroseconds(offUs);
      yield();  // даём ESP8266 обслужить watchdog, иначе ресет ~3.2 с
    }
  }
  digitalWrite(BUZZER_PIN, LOW);
}

// Отрисовка ASCII-арта: каждый не-пробел = квадрат, масштаб под весь экран 240x240
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

  // Крупный шрифт с кириллицей — картинка на весь экран (края могут обрезаться)
  u8g2_for_adafruit_gfx.setFont(u8g2_font_10x20_t_cyrillic);
  u8g2_for_adafruit_gfx.setFontMode(1);  // прозрачный фон
  u8g2_for_adafruit_gfx.setForegroundColor(color_four);

  int maxLen = 0;
  for (int i = 0; i < asciiArtLines; i++) {
    int n = strlen(asciiArt[i]);
    if (n > maxLen) maxLen = n;
  }
  // Шрифт 10x20: картинка больше экрана — рисуем с (0,0), обрежется справа и снизу
  const int fontW = 10;
  const int fontLineH = 20;
  int picW = maxLen * fontW;
  int picH = asciiArtLines * fontLineH;
  int x0 = 0;   // без обрезки слева
  int y0 = 0;   // без обрезки сверху

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

