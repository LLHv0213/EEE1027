#include <LiquidCrystal.h>

// LCD pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// IR sensor pins (unchanged)
const int IR_LEFT   = A5;
const int IR_CENTER = A1;
const int IR_RIGHT  = A4;

// Motor pins (unchanged)
const int IN1 = 1;   // Right motor
const int IN2 = 2;
const int IN3 = 12;  // Left motor
const int IN4 = 13;
const int ENA = 11;  // Left PWM
const int ENB = 3;   // Right PWM


const int ENC_LEFT  = A3;
const int ENC_RIGHT = A2;
volatile long leftCount = 0;
volatile long rightCount = 0;

void leftEncoderISR() {
  leftCount++;
}

void rightEncoderISR() {
  rightCount++;
}

int lastLeftState = LOW;
int lastRightState = LOW;

const float wheelDiameter = 6.5; // cm
const int pulsesPerRev = 20;
const float cmPerPulse = 3.1416 * wheelDiameter / pulsesPerRev;
float getDistanceCM() {
  long leftP = leftCount;
  long rightP = rightCount;

  float avg = (leftP + rightP) / 2.0;
  return avg * cmPerPulse;
}

const int motorSpeed = 180;

// ----- simple memory flags -----
bool lostCenter = false;
int lastTurn = 0;
bool sharpTurning = false;
int sharpTurnDirection = 0;
bool sharpRecovery = false;

// ===== Distance & Time control =====
unsigned long startTime = 0;
bool reached40 = false;        // lock to prevent re-triggering when passing 40cm
bool stoppingNow = false;      // currently in 3s stop
unsigned long stopStartTime = 0;

// =========================
// MOTOR FUNCTIONS
// =========================
void moveForward(int speed = motorSpeed) {
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turnLeft(int speedLeft = motorSpeed*0.8, int speedRight = motorSpeed) {
  analogWrite(ENA, speedLeft);
  analogWrite(ENB, speedRight);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight(int speedLeft = motorSpeed, int speedRight = motorSpeed*0.8) {
  analogWrite(ENA, speedLeft);
  analogWrite(ENB, speedRight);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void forwardNudge() {
    // move forward gently for ~3 cm
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);

    analogWrite(ENA, 100);   // adjust if too fast
    analogWrite(ENB, 100);

    delay(90);  // around 3 cm, adjust if needed

    // stop
    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
}


void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// =========================
// HELPER: Update encoder counts (rising-edge)
 // call frequently (e.g. at top of loop and after motor commands when desired)
void readEncoders() {
  int leftState = digitalRead(ENC_LEFT);
  int rightState = digitalRead(ENC_RIGHT);

  if (lastLeftState == LOW && leftState == HIGH) leftCount++;
  if (lastRightState == LOW && rightState == HIGH) rightCount++;

  lastLeftState = leftState;
  lastRightState = rightState;
}

// =========================
// SETUP
// =========================
void setup() {
  lcd.begin(16, 2);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Encoder pins
  pinMode(ENC_LEFT, INPUT_PULLUP);
  pinMode(ENC_RIGHT, INPUT_PULLUP);

  // Seed last states to avoid false first counts
  lastLeftState = digitalRead(ENC_LEFT);
  lastRightState = digitalRead(ENC_RIGHT);

  startTime = millis();

  lcd.print("Line Tracker Init");
  delay(800);
  lcd.clear();

  // Sensor stabilization at startup
  for (int i = 0; i < 5; i++) {
    analogRead(IR_LEFT);
    analogRead(IR_CENTER);
    analogRead(IR_RIGHT);
    delay(50);
  }
}

// =========================
// LOOP
// =========================
void loop() {
  // ----- Read IR sensors and map to 0-10 -----
  int L = constrain(map(analogRead(IR_LEFT),   0, 1023, 0, 10), 0, 10);
  int C = constrain(map(analogRead(IR_CENTER), 0, 1023, 0, 10), 0, 10);
  int R = constrain(map(analogRead(IR_RIGHT),  0, 1023, 0, 10), 0, 10);

  // ----- Sensor thresholds -----
  bool leftBlack   = (L >= 0 && L <= 3);
  bool leftWhite   = (L >= 4);
  bool rightBlack  = (R >= 0 && R <= 3);
  bool rightWhite  = (R >= 4);
  bool centerBlack = (C >= 6);
  bool centerWhite = (C <= 5);

  // ----- Update encoders -----
  readEncoders();

  // ----- Calculate distance in cm (average of both wheels) -----
  float distanceCM = ((float)(leftCount + rightCount) / 2.0) * cmPerPulse;

  // ===== Improved 40 cm stop logic (non-retriggering) =====
  if (!reached40 && distanceCM >= 40.0) {
    // First time crossing threshold → start stopping
    reached40 = true;          // lock so we won't re-trigger on small fluctuations
    stoppingNow = true;        // enter stopping state
    stopStartTime = millis();
    stopMotors();
  }

  if (stoppingNow) {
    // Keep motors stopped and display time/distance while pausing
    stopMotors();

    lcd.setCursor(0,0);
    lcd.print("Time:"); lcd.print((millis() - startTime)/1000);
    lcd.print("s  "); // clear tail

    lcd.setCursor(0,1);
    lcd.print("Dist:"); lcd.print(distanceCM,1); lcd.print("cm  ");

    // Exit stopping state after 3 seconds (resume next iterations)
    if (millis() - stopStartTime >= 3000) {
      stoppingNow = false;
      // do NOT clear reached40 — prevents re-trigger until you reset leftCount/rightCount or restart
      // Robot will continue normal line-following from next lines (no return here)
    } else {
      return; // skip driving logic while paused
    }
  }

  // ----- Normal display (when not pausing) -----
  lcd.setCursor(0,0);
  lcd.print("Time:"); lcd.print((millis() - startTime)/1000);
  lcd.print("s  ");

  lcd.setCursor(0,1);
  lcd.print("Dist:"); lcd.print(distanceCM,1); lcd.print("cm  ");

  // -------- all black -> stop forever --------
  if (leftBlack && centerBlack && rightBlack) {
    stopMotors();
    while(true) delay(1000);
  }

  // -------- SHARP TURN LOGIC  --------
  if (!sharpTurning && !sharpRecovery) {
    if (leftBlack && centerBlack && rightWhite) { 
      stopMotors(); 
      delay(150); 
      forwardNudge();

      sharpTurning = true; 
      sharpTurnDirection = -1; 
    }
    else if (rightBlack && centerBlack && leftWhite) { 
      stopMotors(); 
      delay(150);
      forwardNudge();

      sharpTurning = true; 
      sharpTurnDirection = 1; }
    }

  if (sharpTurning) {
    if (sharpTurnDirection == -1) {
      if (rightWhite) { 
        // LEFT sharp turn behavior (left motor backward stronger, right forward weaker)
        analogWrite(ENA, motorSpeed*0.8);   // right motor forward weaker
        analogWrite(ENB, motorSpeed);   // left motor backward stronger
        digitalWrite(IN1, LOW);    
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, HIGH);   
        digitalWrite(IN4, LOW);
        readEncoders();
        delay(50);
        return;
      } else { sharpTurning = false; sharpRecovery = true; }
    } else if (sharpTurnDirection == 1) {
      if (leftWhite) {
        // RIGHT sharp turn behavior (left forward stronger, right backward weaker)
        analogWrite(ENA, motorSpeed);
        analogWrite(ENB, motorSpeed*0.8);
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        readEncoders();
        delay(50);
        return;
      } else { sharpTurning = false; sharpRecovery = true; }
    }
  }

  // Sharp recovery (unchanged)
  if (sharpRecovery) {
    if (sharpTurnDirection == -1) {
      if (!(centerBlack && leftWhite && rightWhite)) {
        turnRight(motorSpeed*0.7, motorSpeed*0.9);
        readEncoders();
        delay(50);
        return;
      } else sharpRecovery = false;
    } else if (sharpTurnDirection == 1) {
      if (!(centerBlack && leftWhite && rightWhite)) {
        turnLeft(motorSpeed*0.9, motorSpeed*0.7);
        readEncoders();
        delay(50);
        return;
      } else sharpRecovery = false;
    }
  }

  // -------- NORMAL MODE (not lostCenter) --------
  if (!lostCenter) {
    if (centerBlack && leftWhite && rightWhite) { 
      moveForward(); 
      readEncoders(); 
      delay(50); 
      return; }

    if (centerBlack) {
      if (leftBlack && rightWhite) { 
        turnLeft(motorSpeed*0.7, motorSpeed*0.9); 
        readEncoders(); 
        delay(50); 
        return; }
      if (rightBlack && leftWhite) { 
        turnRight(motorSpeed*0.9, motorSpeed*0.7);
        readEncoders(); 
        delay(50); 
        return; }
    }

    if (centerWhite && leftBlack && rightWhite) { 
      lostCenter = true; 
      lastTurn = -1; 
      turnLeft(); 
      readEncoders(); 
      delay(50); 
      return; }
    if (centerWhite && rightBlack && leftWhite) { 
      lostCenter = true; 
      lastTurn = 1; 
      turnRight(); 
      readEncoders(); 
      delay(50); 
      return; }

    if (leftWhite && centerWhite && rightWhite) { moveForward(motorSpeed*0.8); readEncoders(); delay(50); return; }

    stopMotors();
    delay(50);
    return;
  }

  // -------- LOST CENTER RECOVERY --------
  if (lostCenter) {
    if (lastTurn == -1) {
      if (centerWhite) { 
        turnLeft(motorSpeed*0.7, motorSpeed*0.9); 
        readEncoders(); 
        delay(50); 
        return; }
      else { lostCenter = false; lastTurn = 0; 
      moveForward(); 
      readEncoders(); 
      delay(50); 
      return; }
    }

    if (lastTurn == 1) {
      if (centerWhite) { 
        turnRight(motorSpeed*0.9, motorSpeed*0.8); 
        readEncoders(); 
        delay(50); 
        return; }
      else { lostCenter = false; lastTurn = 0; 
      moveForward(); 
      readEncoders(); 
      delay(50); 
      return; }
    }
  }

  // Safety fallback
  stopMotors();
  delay(50);
}
