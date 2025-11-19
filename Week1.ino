#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int ENA = 11;
int ENB = 3;
int IN1 = A2;
int IN2 = A3;
int IN3 = 2;
int IN4 = 12;
int t;

void setup() {
    

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 200);
  analogWrite(ENB, 181);

  
  for (t = 1; t <= 10; t++) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Time: ");
    lcd.print(t);
    lcd.print(" s");
    delay(1000);
  }

  // Stop the motors
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);

  lcd.clear();
  lcd.setCursor(0, 0);
}

void loop() {

}
