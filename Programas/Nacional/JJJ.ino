// LIBRERÍAS Y CONSTANTES
#include <Servo.h>
#include <Wire.h>
#include "HUSKYLENS.h"

HUSKYLENS huskylens;
Servo direccion;

// Pines para el motor y el servo
const int motorPin = 6;
const int servoPin = 10;

int primerColor = 0;
unsigned long tiempoInicio = 0;
bool esperandoSegundo = false;

// Ajustes principales
const int tiempoEspera = 1500;
const int tiempoGiro = 3200;

// Ángulos de dirección del servo
const int recto = 90;
const int izquierda = 35;
const int derecha = 120;

// Velocidades del motor
const int velRecto = 150;
const int velGiro = 120;
const int velParo = 0;


// FUNCIÓN SETUP
void setup() {
  Serial.begin(9600);
  direccion.attach(servoPin);
  pinMode(motorPin, OUTPUT);

  Wire.begin();
  huskylens.begin(Wire);
  huskylens.writeAlgorithm(ALGORITHM_COLOR_RECOGNITION);

  // Estado inicial del carro
  direccion.write(recto);
  analogWrite(motorPin, velRecto);
  Serial.println("Carro iniciado en marcha recta...");
}

// BUCLE PRINCIPAL
void loop() {
  // Continuar con la lógica de detección de color
  if (huskylens.request()) {
    HUSKYLENSResult resultado = huskylens.read();

    if (!esperandoSegundo) {
      if (resultado.ID == 1 || resultado.ID == 2) {
        primerColor = resultado.ID;
        esperandoSegundo = true;
        tiempoInicio = millis();
        Serial.print("Primer color detectado: ");
        Serial.println(primerColor == 1 ? "Naranja" : "Azul");
      }
    } else {
      if ((millis() - tiempoInicio) < tiempoEspera) {
        if (primerColor == 2 && resultado.ID == 1) {
          // Azul -> Naranja = Izquierda
          direccion.write(izquierda);
          analogWrite(motorPin, velGiro);
          Serial.println("Giro IZQUIERDA");
          delay(tiempoGiro);
          direccion.write(recto);
          analogWrite(motorPin, velRecto);
          esperandoSegundo = false;
        } else if (primerColor == 1 && resultado.ID == 2) {
          // Naranja -> Azul = Derecha
          direccion.write(derecha);
          analogWrite(motorPin, velGiro);
          Serial.println("Giro DERECHA");
          delay(tiempoGiro);
          direccion.write(recto);
          analogWrite(motorPin, velRecto);
          esperandoSegundo = false;
        }
      } else {
        esperandoSegundo = false;
        direccion.write(recto);
        analogWrite(motorPin, velRecto);
        Serial.println("Tiempo expirado, sigue recto");
      }
    }
  } else {
    // Si no se detecta nada, mantener el rumbo recto
    direccion.write(recto);
    analogWrite(motorPin, velRecto);
  }

  delay(50);
}