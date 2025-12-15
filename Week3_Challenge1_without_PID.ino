

#include <LiquidCrystal.h>

// LCD pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// IR sensor pins
const int IR_LEFT   = A5;
const int IR_CENTER = A1;
const int IR_RIGHT  = A4;

// Motor pins
const int IN1 = 1;   // Right motor
const int IN2 = 2; 
const int IN3 = 12;  // Left motor
const int IN4 = 13; 
const int ENA = 11;  // Left PWM
const int ENB = 3;   // Right PWM

const int motorSpeed = 180;

// ----- simple memory flags -----
bool lostCenter = false;         // true after center first loses black
int lastTurn = 0;                // -1 = last turned left, 1 = last turned right
bool sharpTurning = false;       // true when doing 90-degree turn
int sharpTurnDirection = 0;      // -1 = left, 1 = right
bool sharpRecovery = false;      // true when doing recovery after sharp turn

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

void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
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

  
  lcd.print("Line Tracker Init");
  delay(800);
  lcd.clear();

  // ===== Sensor stabilization at startup =====
  for(int i=0; i<5; i++){
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
  // Read sensors and map to 0-10
  int L = constrain(map(analogRead(IR_LEFT),   0, 1023, 0, 10), 0, 10);
  int C = constrain(map(analogRead(IR_CENTER), 0, 1023, 0, 10), 0, 10);
  int R = constrain(map(analogRead(IR_RIGHT),  0, 1023, 0, 10), 0, 10);

  // Sensor thresholds
  bool leftBlack   = (L >= 0 && L <= 3);
  bool leftWhite   = (L >= 4);
  bool rightBlack  = (R >= 0 && R <= 3);
  bool rightWhite  = (R >= 4);
  bool centerBlack = (C >= 6);
  bool centerWhite = (C <= 5);

  // Debug display
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("L:"); lcd.print(L);
  lcd.print(" C:"); lcd.print(C);
  lcd.print(" R:"); lcd.print(R);

  // -------- all black -> stop forever --------
  if (leftBlack && centerBlack && rightBlack) {
    stopMotors();
    lcd.setCursor(0,1);
    lcd.print("STOP - ALL BLACK");
    while(true) delay(1000);
  }

  // -------- SHARP TURN LOGIC (both motors opposite) --------
  if (!sharpTurning && !sharpRecovery) {
    if (leftBlack && centerBlack && rightWhite) {
      stopMotors();
      delay(150);
      sharpTurning = true;
      sharpTurnDirection = -1; // left turn
    } 
    else if (rightBlack && centerBlack && leftWhite) {
      stopMotors();
      delay(150);
      sharpTurning = true;
      sharpTurnDirection = 1;  // right turn
    }
  }

  if (sharpTurning) {
    if (sharpTurnDirection == -1) {
      // LEFT TURN: Left motor backward stronger, right motor forward weaker
      if (rightWhite) {
        analogWrite(ENA, motorSpeed*0.8);   // right motor forward weaker
        analogWrite(ENB, motorSpeed*1.0);   // left motor backward stronger
        digitalWrite(IN1, LOW);    // right motor forward
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, HIGH);   // left motor backward
        digitalWrite(IN4, LOW);
        lcd.setCursor(0,1);
        lcd.print("Sharp Left Turn  ");
        delay(50);
        return;
      } else {
        sharpTurning = false;
        sharpRecovery = true; // start recovery
      }
    } else if (sharpTurnDirection == 1) {
      // RIGHT TURN: Right motor backward stronger, left motor forward weaker
      if (leftWhite) {
        analogWrite(ENA, motorSpeed*1.0);   // left motor backward stronger
        analogWrite(ENB, motorSpeed*0.8);   // right motor forward weaker
        digitalWrite(IN1, HIGH);   // right motor backward
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);    // left motor forward
        digitalWrite(IN4, HIGH);
        lcd.setCursor(0,1);
        lcd.print("Sharp Right Turn ");
        delay(50);
        return;
      } else {
        sharpTurning = false;
        sharpRecovery = true; // start recovery
      }
    }
  }

  // Sharp turn recovery (normal gentle turning)
  if (sharpRecovery) {
    if (sharpTurnDirection == -1) {
      if (!(centerBlack && leftWhite && rightWhite)) {
        turnRight(motorSpeed*0.7, motorSpeed*0.9);
        lcd.setCursor(0,1);
        lcd.print("Recover Right    ");
        delay(50);
        return;
      } else {
        sharpRecovery = false;
      }
    } else if (sharpTurnDirection == 1) {
      if (!(centerBlack && leftWhite && rightWhite)) {
        turnLeft(motorSpeed*0.9, motorSpeed*0.7);
        lcd.setCursor(0,1);
        lcd.print("Recover Left     ");
        delay(50);
        return;
      } else {
        sharpRecovery = false;
      }
    }
  }

  // -------- NORMAL MODE (not lostCenter) --------
  if (!lostCenter) {
    if (centerBlack && leftWhite && rightWhite) {
      moveForward();
      lcd.setCursor(0,1);
      lcd.print("Forward         ");
      delay(50);
      return;
    }

    if (centerBlack) {
      if (leftBlack && rightWhite) {
        turnLeft(motorSpeed*0.7, motorSpeed*0.9);
        lcd.setCursor(0,1);
        lcd.print("Adjust Left cont");
        delay(50);
        return;
      }
      if (rightBlack && leftWhite) {
        turnRight(motorSpeed*0.9, motorSpeed*0.7);
        lcd.setCursor(0,1);
        lcd.print("Adjust Right cont");
        delay(50);
        return;
      }
    }

    if (centerWhite && leftBlack && rightWhite) {
      lostCenter = true;
      lastTurn = -1;
      turnLeft();
      lcd.setCursor(0,1);
      lcd.print("Lost->Turn Left");
      delay(50);
      return;
    }
    if (centerWhite && rightBlack && leftWhite) {
      lostCenter = true;
      lastTurn = 1;
      turnRight();
      lcd.setCursor(0,1);
      lcd.print("Lost->Turn Right");
      delay(50);
      return;
    }

    if (leftWhite && centerWhite && rightWhite) {
      moveForward(motorSpeed*0.8); // slower search at startup or floating line
      lcd.setCursor(0,1);
      lcd.print("Searching...     ");
      delay(50);
      return;
    }

    stopMotors();
    lcd.setCursor(0,1);
    lcd.print("Idle             ");
    delay(50);
    return;
  }

  // -------- LOST CENTER RECOVERY --------
  if (lostCenter) {
    if (lastTurn == -1) {
      if (centerWhite) {
        turnLeft(motorSpeed*0.7, motorSpeed*0.9);
        lcd.setCursor(0,1);
        lcd.print("Recover Left...  ");
        delay(50);
        return;
      } else {
        lostCenter = false;
        lastTurn = 0;
        moveForward();
        lcd.setCursor(0,1);
        lcd.print("Forward recov   ");
        delay(50);
        return;
      }
    }

    if (lastTurn == 1) {
      if (centerWhite) {
        turnRight(motorSpeed*0.9, motorSpeed*0.7);
        lcd.setCursor(0,1);
        lcd.print("Recover Right... ");
        delay(50);
        return;
      } else {
        lostCenter = false;
        lastTurn = 0;
        moveForward();
        lcd.setCursor(0,1);
        lcd.print("Forward recov   ");
        delay(50);
        return;
      }
    }
  }

  // Safety fallback
  stopMotors();
  lcd.setCursor(0,1);
  lcd.print("Fallback stop     ");
  delay(50);
}
