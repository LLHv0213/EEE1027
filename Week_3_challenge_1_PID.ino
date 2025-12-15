#include <LiquidCrystal.h>

// ================= LCD =================
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// ================= IR Sensors =================
const int IR_LEFT   = A5;
const int IR_CENTER = A1;
const int IR_RIGHT  = A4;

// ================= Motors =================
const int IN1 = 1;    // Right motor
const int IN2 = 2;
const int IN3 = 12;   // Left motor
const int IN4 = 13;
const int ENA = 11;   // Left PWM
const int ENB = 3;    // Right PWM

// ================= PID Parameters =================
float Kp = 25.0;
float Ki = 0.0;
float Kd = 12.0;

float error = 0;
float lastError = 0;
float integral = 0;

// ================= Motion =================
int baseSpeed = 160;

// ================= Timing =================
unsigned long startTime;

// ================= Motor Functions =================
void movePID(int leftSpeed, int rightSpeed) {
  leftSpeed  = constrain(leftSpeed,  0, 255);
  rightSpeed = constrain(rightSpeed, 0, 255);

  analogWrite(ENA, leftSpeed);
  analogWrite(ENB, rightSpeed);

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

// ================= SETUP =================
void setup() {
  lcd.begin(16, 2);

  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);

  lcd.print("PID Line Follower");
  delay(1000);
  lcd.clear();

  startTime = millis();
}

// ================= LOOP =================
void loop() {
  // ----- Read sensors -----
  int L = analogRead(IR_LEFT);
  int C = analogRead(IR_CENTER);
  int R = analogRead(IR_RIGHT);

  // ----- Convert to black / white -----
  int Lb = (L < 400);
  int Cb = (C < 400);
  int Rb = (R < 400);

  // ----- End condition -----
  if (Lb && Cb && Rb) {
    stopMotors();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("END OF TRACK");
    lcd.setCursor(0,1);
    lcd.print("Time:");
    lcd.print((millis() - startTime)/1000);
    lcd.print("s");
    while(true);
  }

  // ----- Calculate PID error -----
  error = (-1 * Lb) + (0 * Cb) + (1 * Rb);

  integral += error;
  float derivative = error - lastError;

  float correction = Kp * error + Ki * integral + Kd * derivative;

  int leftSpeed  = baseSpeed - correction;
  int rightSpeed = baseSpeed + correction;

  movePID(leftSpeed, rightSpeed);

  lastError = error;

  // ----- LCD Display -----
  lcd.setCursor(0,0);
  lcd.print("Err:");
  lcd.print(error);
  lcd.print("       ");

  lcd.setCursor(0,1);
  lcd.print("T:");
  lcd.print((millis() - startTime)/1000);
  lcd.print("s     ");

  delay(20);
}
