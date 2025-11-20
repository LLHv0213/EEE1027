#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// ----- Motor Pins -----
const int ENA = 11; // RIGHT motor PWM
const int IN1 = A2;
const int IN2 = A3;
const int ENB = 3;  // LEFT motor PWM
const int IN3 = 2;
const int IN4 = 12;

// ----- IR Pins -----
const int IR_LEFT = 13;
const int IR_RIGHT = A1;

// ----- Encoder Pins -----
const int ENC_LEFT = A4;
const int ENC_RIGHT = A5;

// ----- Encoder Variables -----
int leftCount = 0;
int rightCount = 0;
int lastLeftState = LOW;
int lastRightState = LOW;

// ----- Wheel Parameters -----
const float wheelDiameter = 6.5; // cm
const int pulsesPerRev = 20;

// ----- Motor Speeds -----
int forwardPWM = 150;
int swingPWM = 130;

// Timing
unsigned long startTime;
bool running = true;

void setup() {
  lcd.begin(16, 2);
  lcd.print("Swing Follower");

  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);

  pinMode(IR_LEFT, INPUT);
  pinMode(IR_RIGHT, INPUT);

  pinMode(ENC_LEFT, INPUT_PULLUP);
  pinMode(ENC_RIGHT, INPUT_PULLUP);

  delay(500);
  lcd.clear();
  startTime = millis();
}

void loop() {
  if (!running) return;

  // ----- Read IR sensors -----
  int L = digitalRead(IR_LEFT);
  int R = digitalRead(IR_RIGHT);

  // ----- Read Encoders (polling) -----
  int leftState = digitalRead(ENC_LEFT);
  int rightState = digitalRead(ENC_RIGHT);

  if (lastLeftState == LOW && leftState == HIGH) leftCount++;
  if (lastRightState == LOW && rightState == HIGH) rightCount++;

  lastLeftState = leftState;
  lastRightState = rightState;

  // ----- Calculate distance in cm -----
  float distanceLeft = (leftCount * (wheelDiameter * 3.1416) / pulsesPerRev);
  float distanceRight = (rightCount * (wheelDiameter * 3.1416) / pulsesPerRev);

  // ----- Display -----
  lcd.setCursor(0, 0);
  lcd.print("D:");
  lcd.print((distanceLeft + distanceRight)/2.0, 1);
  lcd.print("cm     ");

  lcd.setCursor(0, 1);
  lcd.print("T:");
  lcd.print((millis() - startTime)/1000);
  lcd.print("s     ");

  // ---------- Movement Logic ----------
  if (L == 1 && R == 1) {           // both white, move forward
    moveForward(forwardPWM);
  }
  else if (L == 0 && R == 1) {  // L sees black, turn L until R sees black
    while(digitalRead(IR_RIGHT) == 1) { 
      dragLeft();
      readEncoders(); // update counts during drag
    }
  }
  else if (L == 1 && R == 0) {      // R sees black, turn R until L sees black
    while(digitalRead(IR_LEFT) == 1) { 
      dragRight();
      readEncoders(); // update counts during drag
    }
  }
  else if (L == 0 && R == 0) {      // both black, stop
    stopMotor();
    running = false;
  }
}

// ----- Movement Functions -----
void moveForward(int pwm) {
  analogWrite(ENA, pwm); // RIGHT
  analogWrite(ENB, pwm); // LEFT
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

// ----- Drag / Single Motor Swing -----
void dragLeft() {
  analogWrite(ENA, swingPWM); // RIGHT motor moves
  analogWrite(ENB, 0);        // LEFT motor stops
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

void dragRight() {
  analogWrite(ENA, 0);        // RIGHT motor stops
  analogWrite(ENB, swingPWM); // LEFT motor moves
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}

void stopMotor() {
  analogWrite(ENA, 0); analogWrite(ENB, 0);
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

// ----- Helper to update encoder counts during drag -----
void readEncoders() {
  int leftState = digitalRead(ENC_LEFT);
  int rightState = digitalRead(ENC_RIGHT);

  if (lastLeftState == LOW && leftState == HIGH) leftCount++;
  if (lastRightState == LOW && rightState == HIGH) rightCount++;

  lastLeftState = leftState;
  lastRightState = rightState;
}
