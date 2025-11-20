#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// IR pins
int IR_LEFT  = 13;
int IR_RIGHT = A1;

void setup() {
  lcd.begin(16, 2);
  lcd.print("2-IR Test");

  pinMode(IR_LEFT, INPUT);
  pinMode(IR_RIGHT, INPUT);
}

void loop() {
  int leftValue  = digitalRead(IR_LEFT);   // 1 = white, 0 = black
  int rightValue = digitalRead(IR_RIGHT);

  // -------------------------
  // Display as 0/1 like "10"
  // -------------------------
  lcd.setCursor(0,1);
  lcd.print(leftValue);
  lcd.print(rightValue);
  lcd.print("          "); // clear leftover chars

  delay(100);
}
