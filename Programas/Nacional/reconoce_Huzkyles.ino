#include <HUSKYLENS.h>
#include <SoftwareSerial.h>
#include <Servo.h>

HUSKYLENS huskylens;
SoftwareSerial mySerial(10, 11); // RX, TX para HuskyLens
Servo direccion;

#define IR_DERECHO 2
#define IR_IZQUIERDO 4
#define TRIG 6
#define ECHO 7

// Motor L298N
#define IN1 8
#define IN2 9
#define ENA 5

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  huskylens.begin(mySerial);

  pinMode(IR_DERECHO, INPUT);
  pinMode(IR_IZQUIERDO, INPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);

  direccion.attach(A0); // Dirección por servo
  direccion.write(90);  // Recto
}

void loop() {
  huskylens.request();
  int colorID = huskylens.countBlocks() > 0 ? huskylens.getBlock(0).ID : -1;

  // Lógica por color
  if (colorID == 2) {  // Suponiendo que ID 1 es rojo
    direccion.write(60); // gira a la derecha
  } else if (colorID == 1) { // Suponiendo que ID 2 es verde
    direccion.write(125); // gira a la izquierda
  } else {
    direccion.write(60); // recto
  }

  // Movimiento del motor hacia adelante
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 150);  // Velocidad moderada

  // Lógica con IR y ultrasónico (opcional)
  int distancia = leerDistancia();
  if (distancia < 15 || (digitalRead(IR_DERECHO) == LOW && digitalRead(IR_IZQUIERDO) == LOW)) {
    frenar();
  }

  delay(100);
}

int leerDistancia() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long duracion = pulseIn(ECHO, HIGH);
  int distancia = duracion * 0.034 / 2;
  return distancia;
}

void frenar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);
}
