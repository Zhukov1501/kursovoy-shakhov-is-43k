// Метеостанция Weather, Arduino Uno (Шахов С.Ю., ИС-43к)
#include <Adafruit_LiquidCrystal.h>

const int TMP = A1, HUM = A3, LDR = A2;
const int LED_G = 3, LED_Y = 5, LED_R = 10, BUZ = 11, MUTE = 6, MODE = 8;
const float COLD_T = 15.0, HOT_T = 28.0;
const int NORM = 0, COLD = 1, HOT = 2;
int st = NORM, last = NORM, page = 0;
bool muted = false, pMute = HIGH, pMode = HIGH;
unsigned long tBeep = 0, tLcd = 0;
Adafruit_LiquidCrystal lcd(0);

float tempC() {
  float mv = analogRead(TMP) * 5000.0 / 1023.0;
  return (mv - 500.0) / 10.0;
}

int pct(int pin) {
  return constrain(map(analogRead(pin), 0, 1023, 0, 100), 0, 100);
}

int classify(float t) {
  if (t < COLD_T) return COLD;
  if (t > HOT_T) return HOT;
  return NORM;
}

const char* name(int s) {
  if (s == COLD) return "COLD";
  if (s == HOT) return "HOT";
  return "NORM";
}

void setLeds(int s) {
  digitalWrite(LED_G, s == NORM); digitalWrite(LED_Y, s == COLD); digitalWrite(LED_R, s == HOT);
}

void show(float t, int h, int light) {
  char a[17], b[17];
  if (page == 0) {
    snprintf(a, 17, "T:%4.1fC H:%2d%%   ", t, h);
    snprintf(b, 17, muted ? "Mode:%-4s  M    " : "Mode:%-4s       ", name(st));
  } else {
    snprintf(a, 17, "Light: %3d%%    ", light);
    snprintf(b, 17, "WxStation  v1.0");
  }
  lcd.setCursor(0, 0); lcd.print(a);
  lcd.setCursor(0, 1); lcd.print(b);
}

bool edge(bool now, bool& prev) {
  bool e = (prev == HIGH && now == LOW); prev = now; return e;
}

void setup() {
  pinMode(LED_G, OUTPUT); pinMode(LED_Y, OUTPUT); pinMode(LED_R, OUTPUT);
  pinMode(BUZ, OUTPUT); pinMode(MUTE, INPUT_PULLUP); pinMode(MODE, INPUT_PULLUP);
  lcd.begin(16, 2); lcd.setBacklight(1); lcd.print("Weather v1.0");
  digitalWrite(LED_G, 1); delay(150); digitalWrite(LED_G, 0);
  digitalWrite(LED_Y, 1); delay(150); digitalWrite(LED_Y, 0);
  digitalWrite(LED_R, 1); delay(150); digitalWrite(LED_R, 0);
  tone(BUZ, 1000, 80); delay(200); lcd.clear(); setLeds(NORM);
}

void loop() {
  float t = tempC(); int h = pct(HUM), light = pct(LDR); st = classify(t);
  if (edge(digitalRead(MUTE), pMute)) muted = !muted;
  if (edge(digitalRead(MODE), pMode)) page = 1 - page;
  if (st != last) last = st;
  setLeds(st);
  if (muted || st != HOT) noTone(BUZ);
  else if (millis() - tBeep >= 400) { tone(BUZ, 1800, 120); tBeep = millis(); }
  if (millis() - tLcd >= 250) { show(t, h, light); tLcd = millis(); }
}
