#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <LiquidCrystal.h>


// LCD pins: RS = 8, E = 9, D4 = 4, D5 = 5, D6 = 6, D7 = 7
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// IR sensor analog pins
const int IR_LEFT   = A5;
const int IR_CENTER = A1;
const int IR_RIGHT  = A4;

void setup() {
  lcd.begin(16, 2);
  lcd.print("IR Sensor Test");
  delay(1000);
  lcd.clear();

  Serial.begin(9600);
}

void loop() {
  // Read raw IR values
  int rawL = analogRead(IR_LEFT);
  int rawC = analogRead(IR_CENTER);
  int rawR = analogRead(IR_RIGHT);

  // Map 0–1023 to 0–10
  int L = map(rawL, 0, 1023, 0, 10);
  int C = map(rawC, 0, 1023, 0, 10);
  int R = map(rawR, 0, 1023, 0, 10);

  // Ensure values stay between 0–10
  L = constrain(L, 0, 10);
  C = constrain(C, 0, 10);
  R = constrain(R, 0, 10);

  // Display on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("L:");
  lcd.print(L);
  lcd.print("  C:");
  lcd.print(C);
  lcd.print("  R:");
  lcd.print(R);

  Serial.print("L:"); Serial.print(L);
  Serial.print("  C:"); Serial.print(C);
  Serial.print("  R:"); Serial.println(R);

  delay(120);
}
