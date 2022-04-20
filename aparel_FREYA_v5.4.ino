// библиотеки
#include <iarduino_RTC.h>
#include <dht11.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <microLED.h>
#include <TimeLib.h>
#include <FastLEDsupport.h>
#include "GyverButton.h"

#define STRIP_PIN 10
#define NUMLEDS 30
#define COLOR_DEBTH 3
dht11 DHT;
#define DHT11_PIN 8
#define BTN_PIN 3

// инициализация
LiquidCrystal_I2C lcd(0x27, 20, 4);
microLED<NUMLEDS, STRIP_PIN, MLED_NO_CLOCK, LED_WS2818, ORDER_GRB, CLI_AVER> strip;
iarduino_RTC watch(RTC_DS1307);
GButton butt1(BTN_PIN);

DEFINE_GRADIENT_PALETTE( heatmap_gp ) {
  0,     0,  0,  0,     // black
  128,   255,  0,  0,   // red
  224,   255, 255,  0,  // bright yellow
  255,   255, 255, 255  // full white
};
CRGBPalette16 fire_p = heatmap_gp;
// переменные
byte mode = 1; // меню
byte rez = 1;
int t;      //  температура
int x;
int hum;
byte brig = 200;
int eq;
int pot;
byte sp = 100;
byte menu = 2;
unsigned long b;
//светомузыка
byte color;
byte del;
byte wid;
int off;
int vlow;
int low;
int med;
int high;

void setup() {
  //Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  butt1.setTimeout(300);        // настройка таймаута на удержание (по умолчанию 500 мс)
  butt1.setClickTimeout(200);
  butt1.setType(LOW_PULL);
  butt1.setDirection(NORM_OPEN);
  watch.begin();
  lcd.init();
  lcd.backlight();
  strip.setBrightness(brig);
  //    intro
  rainbow();
  strip.show();
  lcd.setCursor(0, 2);
  lcd.print("--------------------");
  lcd.setCursor(0, 3);
  lcd.print("v5.4                ");
  delay(300);
  lcd.setCursor(0, 0);
  lcd.print("    A P A R E L'");
  delay(800);
  lcd.setCursor(0, 1);
  lcd.print("     F R E Y A");
  delay(1800);
  lcd.clear();
  strip.clear();
  strip.show();
}

void loop() {
  pot = analogRead(A1);
  butt1.tick();
  if (butt1.isHold()) {
    if (millis() - b > 500) {
      b = millis();
      mode = mode + 1;
      if (mode < 1) mode = 1;
      if (mode > 4) mode = 1;
      lcd.clear();
    }
  }

  if (mode == 1) {
    lcd.setCursor(0, 0);
    lcd.print(watch.gettime("d D      "));
    lcd.print(watch.gettime("M Y"));
    lcd.setCursor(0, 1);
    lcd.print(watch.gettime("      H : i      "));
    lcd.setCursor(0, 2);
    lcd.print("--------------------");
    lcd.setCursor(0, 3);
    x = DHT.read(DHT11_PIN);
    t = DHT.temperature;
    x = DHT.read(DHT11_PIN);
    hum = DHT.humidity;
    lcd.print("t= ");
    lcd.print(t);
    lcd.print(" C     ");
    lcd.print(" h= ");
    lcd.print(hum);
    lcd.print(" %");
    strip.clear();
    strip.show();
    delay(1);
  }

  if (mode == 2) {
    lcd.setCursor(0, 0);
    lcd.print("      LED mods      ");
    lcd.setCursor(1, 2);
    lcd.print("  Brightness: ");
    lcd.print(brig);
    lcd.print("  ");
    lcd.setCursor(1, 3);
    lcd.print("  Speed: ");
    lcd.print(sp);
    lcd.print("        ");
    if ((digitalRead(3)) == 1) {
      menu = menu + 1;
      if (menu > 3 || menu < 1) menu = 1;
      delay(100);
    }
    if (menu == 1) {
      rez = map(pot, 0, 1023, 1, 12);
      lcd.setCursor(0, 1);
      lcd.print(">");
      lcd.setCursor(0, 2);
      lcd.print(" ");
      lcd.setCursor(0, 3);
      lcd.print(" ");
      delay(50);
    }
    if (menu == 2) {
      brig = map(pot, 0, 1023, 0, 250);
      lcd.setCursor(0, 2);
      lcd.print(">");
      strip.setBrightness(brig);
      lcd.setCursor(0, 1);
      lcd.print(" ");
      lcd.setCursor(0, 3);
      lcd.print(" ");
      delay(5);
    }
    if (menu == 3) {
      sp = map(pot, 0, 1023, 0, 200);
      lcd.setCursor(0, 3);
      lcd.print(">");
      lcd.setCursor(0, 1);
      lcd.print(" ");
      lcd.setCursor(0, 2);
      lcd.print(" ");
      delay(5);
    }

    lcd.setCursor(1, 1);
    if (rez == 1) {
      lcd.print("   fast rainbow    ");
      rainbow();
      strip.show();
      delay(0.5);
    }
    if (rez == 2) {
      lcd.print("      Cycle        ");
      colorCycle();
      strip.show();
      delay(sp);
    }
    if (rez == 3) {
      lcd.print("      rainbow      ");
      rainbow();
      strip.show();
      delay(sp);
    }
    if (rez == 4) {
      lcd.print("       Wheel       ");
      static byte counter = 0;
      strip.fill(mWheel8(counter));
      counter += sp;
      strip.show();
      delay(60);
    }
    if (rez == 5) {
      lcd.print("     FILLER AQUA    ");
      static int counter = 0;
      strip.clear();
      strip.fill(0, counter, mAqua);
      counter++;
      if (counter >= NUMLEDS) counter = 0;
      strip.show();
      delay(sp);
    }
    if (rez == 6) {
      lcd.print("   breathing  AQUA ");
      strip.fill(mAqua);
      breathing();
      strip.show();
      delay(sp);
    }
    if (rez == 7) {
      lcd.print("       FIRE        ");
      static int count = 0;
      count += 10;
      strip.begin();
      for (int i = 0; i < 50; i++)
        strip.send(CRGBtoData(ColorFromPalette(fire_p, inoise8(i * 25, count), 255, LINEARBLEND)));

      strip.end();
      delay(sp);
    }
    if (rez == 8) {
      lcd.print("    FILLER white   ");
      static int counter = 0;
      strip.clear();
      strip.fill(0, counter, mWhite);
      counter++;
      if (counter >= NUMLEDS) counter = 0;
      strip.show();
      delay(sp);
    }
    if (rez == 9) {
      lcd.print("       Dots WB     ");
      static byte counter = 0;
      // перемотка буфера со сдвигом (иллюзия движения пикселей)
      for (int i = 0; i < NUMLEDS - 1; i++) strip.leds[i] = strip.leds[i + 1];

      // каждый третий вызов - последний пиксель красным, иначе чёрным
      if (counter % 3 == 0) strip.leds[NUMLEDS - 1] = mWhite;
      else strip.leds[NUMLEDS - 1] = mBlack;
      counter++;
      delay(100);   // дополнительная задержка
      strip.show();
      delay(sp);
    }
    if (rez == 10) {
      lcd.print("       Dots RL     ");
      static byte counter = 0;
      // перемотка буфера со сдвигом (иллюзия движения пикселей)
      for (int i = 0; i < NUMLEDS - 1; i++) strip.leds[i] = strip.leds[i + 1];

      // каждый третий вызов - последний пиксель красным, иначе чёрным
      if (counter % 3 == 0) strip.leds[NUMLEDS - 1] = mRed;
      else strip.leds[NUMLEDS - 1] = mLime;
      counter++;
      delay(100);   // дополнительная задержка
      strip.show();
      delay(sp);
    }
    if (rez == 11) {
      lcd.print("       Dots AP     ");
      static byte counter = 0;
      // перемотка буфера со сдвигом (иллюзия движения пикселей)
      for (int i = 0; i < NUMLEDS - 1; i++) strip.leds[i] = strip.leds[i + 1];

      // каждый третий вызов - последний пиксель красным, иначе чёрным
      if (counter % 3 == 0) strip.leds[NUMLEDS - 1] = mPurple;
      else strip.leds[NUMLEDS - 1] = mAqua;
      counter++;
      delay(100);   // дополнительная задержка
      strip.show();
      delay(sp);
    }
  }

  if (mode == 3) {
    lcd.setCursor(0, 0);
    lcd.print("     LED colors     ");
    lcd.setCursor(0, 2);
    lcd.print("--------------------");
    if ((digitalRead(3)) == 1) {
      menu = menu + 1;
      if (menu > 2 || menu < 1) menu = 1;
      delay(100);
    }
    if (menu == 1) {
      color = map(pot, 0, 1023, 0, 11);
      lcd.setCursor(0, 1);
      lcd.print(">");
      lcd.setCursor(0, 3);
      lcd.print(" ");
      brig = 200;
      delay(50);
    }
    if (menu == 2) {
      brig = map(pot, 0, 1023, 0, 250);
      lcd.setCursor(0, 3);
      lcd.print(">");
      strip.setBrightness(brig);
      lcd.setCursor(0, 1);
      lcd.print(" ");
      lcd.setCursor(1, 3);
      lcd.print("  Brightness: ");
      lcd.print(brig);
      lcd.print("  ");
      delay(5);
    }
    lcd.setCursor(1, 1);
    if (color == 1) {
      lcd.print("       White       ");
      strip.fill(mWhite);
      strip.show();
      delay(50);
    }

    // готовые цвета mWhite, mSilver, mGray, mBlack, mRed, mMaroon, mOrange,
    //mYellow, mOlive, mLime, mGreen, mAqua, mTeal, mBlue, mNavy, mMagenta, mPurple.

    if (color == 2) {
      lcd.print("        RED        ");
      strip.fill(mRed);
      strip.show();
      delay(50);
    }
    if (color == 3) {
      lcd.print("       Orange      ");
      strip.fill(mOrange);
      strip.show();
      delay(50);
    }
    if (color == 4) {
      lcd.print("       Yellow      ");
      strip.fill(mYellow);
      strip.show();
      delay(50);
    }
    if (color == 5) {
      lcd.print("        Olive      ");
      strip.fill(mOlive);
      strip.show();
      delay(50);
    }
    if (color == 6) {
      lcd.print("        Lime       ");
      strip.fill(mLime);
      strip.show();
      delay(50);
    }
    if (color == 7) {
      lcd.print("        AQUA       ");
      strip.fill(mAqua);
      strip.show();
      delay(50);
    }
    if (color == 8) {
      lcd.print("        Blue       ");
      strip.fill(mBlue);
      strip.show();
      delay(50);
    }
    if (color == 9) {
      lcd.print("        Navy       ");
      strip.fill(mNavy);
      strip.show();
      delay(50);
    }
    if (color == 10) {
      lcd.print("      Magenta      ");
      strip.fill(mMagenta);
      strip.show();
      delay(50);
    }
  }
  if (mode == 4) {
    strip.setBrightness(250);
    lcd.setCursor(0, 0);
    lcd.print("   MUSIC   LIGHT    ");
    lcd.setCursor(2, 1);
    lcd.print("OFF level: ");
    lcd.print(off);
    lcd.setCursor(2, 2);
    lcd.print("delay: ");
    lcd.print(del);
    lcd.print("   ");
    lcd.setCursor(2, 3);
    lcd.print("width: ");
    lcd.print(wid);
    lcd.print("   ");
    if ((digitalRead(3)) == 1) {
      menu = menu + 1;
      if (menu > 3 || menu < 1) menu = 1;
      delay(100);
    }
    if (menu == 1) {
      off = map(pot, 0, 1023, 520, 570);
      lcd.setCursor(0, 1);
      lcd.print(">");
      lcd.setCursor(0, 3);
      lcd.print(" ");
      lcd.setCursor(0, 2);
      lcd.print(" ");
      delay(5);
    }
    if (menu == 2) {
      del = map(pot, 0, 1023, 0, 100);
      lcd.setCursor(0, 2);
      lcd.print(">");
      lcd.setCursor(0, 1);
      lcd.print(" ");
      lcd.setCursor(0, 3);
      lcd.print(" ");
      delay(5);
    }
    if (menu == 3) {
      wid = map(pot, 0, 1023, 0, 10);
      lcd.setCursor(0, 3);
      lcd.print(">");
      lcd.setCursor(0, 1);
      lcd.print(" ");
      lcd.setCursor(0, 2);
      lcd.print(" ");
      delay(5);
    }
    vlow = off + wid;
    low = vlow + wid;
    med = low + wid;
    high = med + wid;
    eq = analogRead(A0);
    if (eq >= off) {
      if (eq >= off && eq < vlow) {
        strip.fill(mAqua);
        strip.show();
      }
      if (eq >= vlow && eq < low) {
        strip.fill(mNavy);
        strip.show();
      }
      if (eq >= low && eq < med) {
        strip.fill(mOrange);
        strip.show();
      }
      if (eq >= med && eq < high) {
        strip.fill(mMagenta);
        strip.show();
      }
      if (eq >= high) {
        strip.fill(mWhite);
        strip.show();
      }
      delay(del);
    }
    if (eq < off) {
      strip.clear();
      strip.show();
    }
  }
}
void rainbow() {
  static byte counter = 0;
  for (int i = 0; i < NUMLEDS; i++) {
    strip.set(i, mWheel8(counter + i * 255 / NUMLEDS));   // counter смещает цвет
  }
  counter += 3;   // counter имеет тип byte и при достижении 255 сбросится в 0
}
void colorCycle() {
  static byte counter = 0;
  strip.fill(mWheel8(counter));
  counter += 1;
}
void breathing() {
  static int dir = 1;
  static int bright = 0;
  bright += dir * 5;    // 5 - множитель скорости изменения

  if (bright > 255) {
    bright = 255;
    dir = -1;
  }
  if (bright < 0) {
    bright = 0;
    dir = 1;
  }
  strip.setBrightness(bright);
}
