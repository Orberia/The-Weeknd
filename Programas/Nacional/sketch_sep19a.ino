#include <Servo.h>
#include <Wire.h>
#include "HUSKYLENS.h"

HUSKYLENS huskylens;
Servo direccion;

const int motorPin = 6;     // Motor DC
const int servoPin = 10;    // Servo de dirección

int primerColor = 0;        
unsigned long tiempoInicio = 0;  
bool esperandoSegundo = false;   

// Ajustes principales
const int tiempoEspera = 1500;  // Tiempo máximo para esperar el segundo color (ms)
const int tiempoGiro   = 3200;  // Tiempo que se mantiene girando (ms) después del 2do color

// Ángulos de dirección del servo
const int recto     = 90;
const int izquierda = 35;
const int derecha   = 120;

// Velocidades del motor
const int velRecto  = 150;
const int velGiro   = 120;

void setup() {
  Serial.begin(9600);
  direccion.attach(servoPin);
  pinMode(motorPin, OUTPUT);

  Wire.begin();
  huskylens.begin(Wire);
  huskylens.writeAlgorithm(ALGORITHM_COLOR_RECOGNITION);  

  direccion.write(recto);      
  analogWrite(motorPin, velRecto);  
  Serial.println("Carro iniciado en marcha recta...");
}

void loop() {
  if (huskylens.request()) {
    HUSKYLENSResult resultado = huskylens.read();

    if (!esperandoSegundo) {
      // Guardar primer color
      if (resultado.ID == 1 || resultado.ID == 2) {
        primerColor = resultado.ID;
        esperandoSegundo = true;
        tiempoInicio = millis();
        Serial.print("Primer color detectado: ");
        Serial.println(primerColor == 1 ? "Naranja" : "Azul");
      }
    } else {
      // Esperando segundo color dentro del tiempo límite
      if ((millis() - tiempoInicio) < tiempoEspera) {
        if (primerColor == 2 && resultado.ID == 1) {
          // Azul → Naranja = Izquierda
          direccion.write(izquierda);     
          analogWrite(motorPin, velGiro);
          Serial.println("Giro IZQUIERDA");
          delay(tiempoGiro);   // Mantiene el giro más tiempo
          direccion.write(recto);
          analogWrite(motorPin, velRecto);
          esperandoSegundo = false;  
        }
        else if (primerColor == 1 && resultado.ID == 2) {
          // Naranja → Azul = Derecha
          direccion.write(derecha);     
          analogWrite(motorPin, velGiro);
          Serial.println("Giro DERECHA");
          delay(tiempoGiro);   // Mantiene el giro más tiempo
          direccion.write(recto);
          analogWrite(motorPin, velRecto);
          esperandoSegundo = false;
        }
      } else {
        // Tiempo vencido sin segundo color → volver recto
        esperandoSegundo = false;
        direccion.write(recto);
        analogWrite(motorPin, velRecto);
        Serial.println("Tiempo expirado, sigue recto");
      }
    }
  } else {
    // Sin detección de HuskyLens → recto
    direccion.write(recto);
    analogWrite(motorPin, velRecto);
  }

  delay(50);
}
