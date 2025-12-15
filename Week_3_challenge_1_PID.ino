#include <LiquidCrystal.h>

// ================= LCD =================
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// ================= IR SENSORS =================
const int IR_LEFT   = A5;
const int IR_CENTER = A1;
const int IR_RIGHT  = A4;

// ================= MOTOR PINS =================
const int IN1 = 1;     // Right motor
const int IN2 = 2;
const int IN3 = 12;    // Left motor
const int IN4 = 13;
const int ENA = 11;    // Left PWM
const int ENB = 3;     // Right PWM

// ================= ENCODERS =================
const int ENC_LEFT  = A3;
const int ENC_RIGHT = A2;
long leftCount = 0;
long rightCount = 0;
int lastLeftState = LOW;
int lastRightState = LOW;

// ================= WHEEL PARAMETERS =================
const float wheelDiameter = 6.5;
const int pulsesPerRev = 20;
const float cmPerPulse = 3.1416 * wheelDiameter / pulsesPerRev;

// ================= PID PARAMETERS =================
float Kp = 18.5;
float Ki = 0.05;
float Kd = 11.5;

float error = 0;
float lastError = 0;
float integral = 0;

int baseSpeed = 160;

// ================= FLAGS =================
bool sharpTurning = false;
bool sharpRecovery = false;
int sharpTurnDirection = 0;

// ================= DISTANCE & TIME =================
unsigned long startTime;
bool reached40 = false;
bool stoppingNow = false;
unsigned long stopStartTime = 0;

// ================= MOTOR FUNCTIONS =================
void movePID(int leftPWM, int rightPWM) {
  analogWrite(ENA, leftPWM);
  analogWrite(ENB, rightPWM);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// ================= ENCODER READ =================
void readEncoders() {
  int l = digitalRead(ENC_LEFT);
  int r = digitalRead(ENC_RIGHT);

  if (lastLeftState == LOW && l == HIGH) leftCount++;
  if (lastRightState == LOW && r == HIGH) rightCount++;

  lastLeftState = l;
  lastRightState = r;
}

// ================= SETUP =================
void setup() {
  lcd.begin(16, 2);

  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);

  pinMode(ENC_LEFT, INPUT_PULLUP);
  pinMode(ENC_RIGHT, INPUT_PULLUP);

  lastLeftState = digitalRead(ENC_LEFT);
  lastRightState = digitalRead(ENC_RIGHT);

  startTime = millis();

  lcd.print("PID Line Tracker");
  delay(800);
  lcd.clear();
}

// ================= LOOP =================
void loop() {
  // ----- Read IR sensors -----
  int L = constrain(map(analogRead(IR_LEFT),   0, 1023, 0, 10), 0, 10);
  int C = constrain(map(analogRead(IR_CENTER), 0, 1023, 0, 10), 0, 10);
  int R = constrain(map(analogRead(IR_RIGHT),  0, 1023, 0, 10), 0, 10);

  bool leftBlack   = (L <= 3);
  bool rightBlack  = (R <= 3);
  bool centerBlack = (C >= 6);

  readEncoders();

  float distanceCM = ((leftCount + rightCount) / 2.0) * cmPerPulse;

  // ----- 40 CM STOP -----
  if (!reached40 && distanceCM >= 40.0) {
    reached40 = true;
    stoppingNow = true;
    stopStartTime = millis();
    stopMotors();
  }

  if (stoppingNow) {
    stopMotors();
    lcd.setCursor(0,0);
    lcd.print("Time:");
    lcd.print((millis() - startTime)/1000);
    lcd.print("s   ");

    lcd.setCursor(0,1);
    lcd.print("Dist:");
    lcd.print(distanceCM,1);
    lcd.print("cm  ");

    if (millis() - stopStartTime >= 3000) stoppingNow = false;
    else return;
  }

  // ----- DISPLAY -----
  lcd.setCursor(0,0);
  lcd.print("Time:");
  lcd.print((millis() - startTime)/1000);
  lcd.print("s   ");

  lcd.setCursor(0,1);
  lcd.print("Dist:");
  lcd.print(distanceCM,1);
  lcd.print("cm  ");

  // ----- ALL BLACK STOP -----
  if (leftBlack && centerBlack && rightBlack) {
    stopMotors();
    while(true);
  }

  // ================= PID ERROR =================
  if (centerBlack && !leftBlack && !rightBlack) error = 0;
  else if (leftBlack && !centerBlack) error = -1;
  else if (rightBlack && !centerBlack) error = 1;
  else if (leftBlack && centerBlack) error = -2;
  else if (rightBlack && centerBlack) error = 2;
  else error = lastError;

  // ================= PID COMPUTATION =================
  integral += error;
  float derivative = error - lastError;
  float correction = Kp*error + Ki*integral + Kd*derivative;
  lastError = error;

  int leftPWM  = baseSpeed - correction;
  int rightPWM = baseSpeed + correction;

  leftPWM  = constrain(leftPWM, 0, 255);
  rightPWM = constrain(rightPWM, 0, 255);

  // ================= MOVE =================
  movePID(leftPWM, rightPWM);
  delay(15);
}
