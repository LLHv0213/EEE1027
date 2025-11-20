#include <LiquidCrystal.h>

// LCD pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Motor pins
const int IN1 = A2; // Right motor
const int IN2 = A3;
const int IN3 = 2; // Left motor
const int IN4 = 12;
const int ENA = 11; // Left PWM
const int ENB = 3;  // Right PWM

const int motorSpeed = 200; // PWM speed
const int buttonPin = A0;   // Button shield analog pin

// Function to read shield button
String readButton() {
  int val = analogRead(buttonPin);
  if (val < 60) return "RIGHT";
  else if (val < 200) return "UP";
  else if (val < 400) return "DOWN";
  else if (val < 600) return "LEFT";
  else if (val < 800) return "SELECT";
  else return "NONE";
}

void setup() {
  lcd.begin(16, 2);
  lcd.print("Motor Test Ready");

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  delay(1000);
  lcd.clear();
  lcd.print("Use Shield Buttons");
}

void loop() {
  String button = readButton();

  lcd.setCursor(0, 0);
  lcd.print("                "); // clear line
  lcd.setCursor(0, 0);

  if (button == "UP") { // Forward
    lcd.print("Forward");
    analogWrite(ENA, motorSpeed);
    analogWrite(ENB, motorSpeed);
    digitalWrite(IN1, LOW);  
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);   
    digitalWrite(IN4, HIGH);
  }
  else if (button == "DOWN") { // Backward
    lcd.print("Backward");
    analogWrite(ENA, motorSpeed);
    analogWrite(ENB, motorSpeed);
    digitalWrite(IN1, HIGH);   
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);  
    digitalWrite(IN4, LOW);
  }
  else if (button == "LEFT") { // Turn Left
    lcd.print("Turn Left");
    analogWrite(ENA, motorSpeed); // slow left motor
    analogWrite(ENB, motorSpeed/2);
    digitalWrite(IN1, LOW);  
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);  
    digitalWrite(IN4, LOW);
  }
  else if (button == "RIGHT") { // Turn Right
    lcd.print("Turn Right");
    analogWrite(ENA, motorSpeed/2);  
    analogWrite(ENB, motorSpeed); // slow right motor
    digitalWrite(IN1, HIGH);   
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);  
    digitalWrite(IN4, HIGH);
  }
  else if (button == "SELECT") { // Stop
    lcd.print("Stop");
    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }
  else { // No button pressed
    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }

  delay(100); // debounce
}
