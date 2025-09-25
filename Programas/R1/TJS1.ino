// Pines del motor
int IN1 = 8;
int IN2 = 9;
int ENA = 5; // PWM

// Pines Bluetooth
#include <SoftwareSerial.h>
SoftwareSerial BT(10, 11); // RX, TX

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  BT.begin(9600); // Velocidad del HC-05
}

void loop() {
  if (BT.available()) {
    char comando = BT.read();

    if (comando == 'F') { // Forward
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      analogWrite(ENA, 200);
    }
    else if (comando == 'B') { // Backward
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      analogWrite(ENA, 200);
    }
    else if (comando == 'S') { // Stop
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      analogWrite(ENA, 0);
    }
  }
}
