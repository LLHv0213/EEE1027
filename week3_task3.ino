#include <LiquidCrystal.h>

// LCD pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Motor Pins
const int IN1 = A1;  
const int IN2 = A2; 
const int IN3 = 12; 
const int IN4 = 13; 
const int ENA = 11; 
const int ENB = 3;   

const int motorSpeed = 120;

// HC-SR04 Ultrasonic Pins
const int TRIG = 2;  
const int ECHO = A3;  

const int safeDistance = 30; // cm

// ----------------------
// MOTOR FUNCTIONS
// ----------------------
void moveForward(int speed = motorSpeed) {
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
  digitalWrite(IN1, LOW);  
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);   
  digitalWrite(IN4, HIGH);
}

void moveBackward(int speed = motorSpeed) {
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
  digitalWrite(IN1, HIGH);  
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);   
  digitalWrite(IN4, LOW);
}

void turnLeft(int speed = motorSpeed) {
  analogWrite(ENA, speed * 0.5);
  analogWrite(ENB, speed);
  digitalWrite(IN1, LOW);  
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);  
  digitalWrite(IN4, LOW);
}

void turnRight(int speed = motorSpeed) {
  analogWrite(ENA, speed);
  analogWrite(ENB, speed * 0.5);
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

// ----------------------
// ULTRASONIC FUNCTION
// ----------------------
long getDistanceCM() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 30000); // timeout 30ms
  long distance = duration * 0.034 / 2;

  return distance;
}

// ----------------------
// SETUP
// ----------------------
void setup() {
  lcd.begin(16, 2);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  lcd.print("Ultrasonic Init");
  delay(800);
  lcd.clear();
}

// ----------------------
// UPDATED LOOP
// ----------------------
void loop() {

  long distance = getDistanceCM();

  // -------------------------------------
  // VALID DISTANCE & CLEAR PATH > 30 CM
  // -------------------------------------
  if (distance != 0 && distance > safeDistance) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Distance:");
    lcd.setCursor(0, 1);
    lcd.print(distance);
    lcd.print(" cm");

    moveForward();
    delay(100);
    return;
  }

  // -------------------------------------
  // OBSTACLE DETECTED (1–30 CM)
  // -------------------------------------
  if (distance > 0 && distance <= safeDistance) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Obstacle in");
    lcd.setCursor(0, 1);
    lcd.print(distance);
    lcd.print("cm");
    delay(300);

    // Reverse
    moveBackward();
    delay(400);
    stopMotors();
    delay(100);

    // Turn randomly left or right
    if (random(0, 2) == 0) {
      turnLeft();
    } else {
      turnRight();
    }
    delay(400);
    stopMotors();
    delay(100);

    return;
  }

  // -------------------------------------
  // distance == 0  → no reading
  // -------------------------------------
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scanning...");
  moveForward();
  delay(100);
}
