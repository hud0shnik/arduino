#include <SPI.h>

#include <Arduino_ST7789_Fast.h>

#define TFT_DC 7
#define TFT_RST 8
#define SCR_WD 240
#define SCR_HT 240
Arduino_ST7789 lcd = Arduino_ST7789(TFT_DC, TFT_RST);

/*  Проводочки:
  #01 GND -> GND
  #02 VCC -> VCC
  #03 SCK -> D13
  #04 SDA -> D11/MOSI
  #05 RES -> D8
  #06 DC  -> D7
  #07 BLK -> NOT USED
*/

int botChoice;
char m[9] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};

void printMap() {
  lcd.fillScreen(BLACK);
  lcd.setTextColor(WHITE);
  lcd.setCursor(24, 10);
  lcd.print(m[0]);
  lcd.print("|");
  lcd.print(m[1]);
  lcd.print("|");
  lcd.println(m[2]);
  lcd.setCursor(24, 50);
  lcd.println("-+-+-");
  lcd.setCursor(24, 90);
  lcd.print(m[3]);
  lcd.print("|");
  lcd.print(m[4]);
  lcd.print("|");
  lcd.println(m[5]);
  lcd.setCursor(24, 130);
  lcd.println("-+-+-");
  lcd.setCursor(24, 170);
  lcd.print(m[6]);
  lcd.print("|");
  lcd.print(m[7]);
  lcd.print("|");
  lcd.println(m[8]);
  color();
}

bool btn1 = false;
bool btn2 = false;
int userChoice = 0;
//int botChoice;

void color() {
  switch (userChoice) {
    case 0:
      lcd.drawRect(15, 3, 50, 60, BLUE);
      break;
    case 1:
      lcd.drawRect(80, 3, 60, 60, BLUE);
      break;
    case 2:
      lcd.drawRect(160, 3, 50, 60, BLUE);
      break;
    case 3:
      lcd.drawRect(15, 80, 50, 60, BLUE);
      break;
    case 4:
      lcd.drawRect(80, 80, 60, 60, BLUE);
      break;
    case 5:
      lcd.drawRect(160, 80, 50, 60, BLUE);
      break;
    case 6:
      lcd.drawRect(15, 160, 50, 60, BLUE);
      break;
    case 7:
      lcd.drawRect(80, 160, 60, 60, BLUE);
      break;
    case 8:
      lcd.drawRect(160, 160, 50, 60, BLUE);
      break;

  }
}

void setup() {
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  lcd.init(SCR_WD, SCR_HT);
  lcd.fillScreen(BLACK);
  lcd.drawRect(0, 0, 240, 240, BLUE);
  lcd.setTextColor(WHITE);
  lcd.setTextSize(6);
  lcd.setCursor(24, 100);
  lcd.println("SETUP");
  lcd.setTextColor(BLACK, WHITE);

  while (true) {
    printMap();
    while (!isFull()) {
      userTurn();
      if (checkWin('O')) {
        lcd.drawRect(1, 1, 238, 238, GREEN);
        lcd.drawRect(0, 0, 240, 240, GREEN);
        printMsg("You win!!!");
        break;
      }
      printMsg("Thinking...");
      ab(true);
      m[botChoice] = 'X';
      printMap();
      if (checkWin('X')) {
        printMap();
        lcd.drawRect(1, 1, 238, 238, RED);
        lcd.drawRect(0, 0, 240, 240, RED);
        printMsg("You lose...");
        break;
      }
      if (isFull()) {
        lcd.drawRect(1, 1, 238, 238, RED);
        lcd.drawRect(0, 0, 240, 240, GREEN);
        printMsg("Draw!");
        break;
      }
    }
    delay(3000);
    for (int i = 0; i < 9; i++) {
      m[i] = ' ';
    }
  }

}

void printMsg(String s) {
  lcd.setTextSize(2);
  lcd.setCursor(5, 220);
  lcd.println(s);
  lcd.setTextSize(6);
}


void userTurn() {
  while (true) {
    bool btnState = !digitalRead(3);
    if (btnState && !btn1) {
      btn1 = true;
      if (m[userChoice] == ' ') {
        m[userChoice] = 'O';
        printMap();
        break;
      }
      printMap();
    }
    if (!btnState && btn1) {
      btn1 = false;
    }
    bool btnState2 = !digitalRead(4);
    if (btnState2 && !btn2) {
      btn2 = true;
      if (userChoice >= 8) {
        userChoice = 0;
      } else {
        userChoice++;
      }
      printMap();

    }
    if (!btnState2 && btn2) {
      btn2 = false;
    }
  }
}

void loop() {

}

bool checkWin(char c) {
  if ((m[0] == m[1]) && (c == m[0]) && (m[2] == c)) {
    return true;
  }
  if ((m[3] == m[4]) && (c == m[3]) && (m[5] == c)) {
    return true;
  }
  if ((m[6] == m[7]) && (c == m[6]) && (m[8] == c)) {
    return true;
  }
  if ((m[0] == m[3]) && (c == m[3]) && (m[6] == c)) {
    return true;
  }
  if ((m[1] == m[4]) && (c == m[1]) && (m[7] == c)) {
    return true;
  }
  if ((m[2] == m[5]) && (c == m[2]) && (m[8] == c)) {
    return true;
  }
  if ((m[0] == m[4]) && (c == m[0]) && (m[8] == c)) {
    return true;
  }
  if ((m[6] == m[4]) && (c == m[6]) && (m[2] == c)) {
    return true;
  }
  return false;
}

bool isFull() {
  for (int i = 0; i < 9; i++) {
    if (m[i] == ' ') {
      return false;
    }
  }
  return true;
}

int ab(bool flag) {
  int max = -20, min = 20;
  int i, j, value = 1;
  if (checkWin('X')) {
    return 10;
  }
  if (checkWin('O')) {
    return -10;
  }
  if (isFull()) {
    return 0;
  }

  int score[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};

  for (i = 0; i < 9; i++) {
    if (m[i] == ' ') {
      if (min > max) {
        if (flag == true) {
          m[i] = 'X';
          value = ab(false);
        } else {
          m[i] = 'O';
          value = ab(true);
        }
        m[i] = ' ';
        score[i] = value;
      }
    }
  }

  if (flag) {
    max = -20;
    for (j = 0; j < 9; j++) {
      if (score[j] > max && score[j] != 1) {
        max = score[j];
        botChoice = j;
      }
    }
    return max;
  }
  if (!flag) {
    min = 20;
    for (j = 0; j < 9; j++) {
      if (score[j] < min && score[j] != 1) {
        min = score[j];
        botChoice = j;
      }
    }
    return min;
  }
}
