#include <SoftwareSerial.h>

// === HC-05 Bluetooth Pins ===
SoftwareSerial BT(10, 11);  
// RX = 10 (Arduino receives from HC05 TX)
// TX = 11  (Arduino sends to HC05 RX) → Use voltage divider!

// === Motor Driver L298N ===
// Left motor
#define ENA 9
#define ENB 3
#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7

// === Motor Speed (Smooth control) ===
int MOTOR_SPEED = 180;  // 0–255

void setup() {
  // Motor pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);


  // Start serial connections
  Serial.begin(9600);   // Debug monitor
  BT.begin(9600);       // HC-05 communication

  Serial.println("Bluetooth Robot Ready.");
  BT.println("Connected to Arduino.");
}

// === Motor Control Functions ===
void forward() {
  analogWrite(ENA, MOTOR_SPEED);
  analogWrite(ENB, MOTOR_SPEED);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void backward() {
  analogWrite(ENA, MOTOR_SPEED);
  analogWrite(ENB, MOTOR_SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnLeft() {
  analogWrite(ENA, MOTOR_SPEED);
  analogWrite(ENB, MOTOR_SPEED);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight() {
  analogWrite(ENA, MOTOR_SPEED);
  analogWrite(ENB, MOTOR_SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopRobot() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

void loop() {
  // Check if there is a command from Bluetooth
  if (BT.available()) {
    char cmd = BT.read();

    Serial.print("Received: ");
    Serial.println(cmd);

    // === Command Handling ===
    switch (cmd) {
      case 'F': forward();   BT.println("Forward");  break;
      case 'B': backward();  BT.println("Backward"); break;
      case 'L': turnLeft();  BT.println("Left");     break;
      case 'R': turnRight(); BT.println("Right");    break;
      case 'S': stopRobot();   BT.println("Stop");     break;

      // **Adjust speed command**
      case '+':
        MOTOR_SPEED = min(MOTOR_SPEED + 20, 255);
        BT.print("Speed: "); BT.println(MOTOR_SPEED);
        break;

      case '-':
        MOTOR_SPEED = max(MOTOR_SPEED - 20, 0);
        BT.print("Speed: "); BT.println(MOTOR_SPEED);
        break;

      default:
        BT.println("Unknown Command");
        break;
    }
  }
}