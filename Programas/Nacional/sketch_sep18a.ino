#include <Servo.h>
#include <Wire.h>
#include "HUSKYLENS.h"

HUSKYLENS huskylens;
Servo direccion;

const int motorPin = 6;     // Motor DC
const int servoPin = 10;    // Servo de dirección

int ultimoColor = 0;          
bool giroPendiente = false;  
int direccionGiro = 0;        // 1 = izquierda, 2 = derecha
unsigned long tiempoGiro = 0; 

// Variables para filtro de estabilidad
int colorEstable = 0;
int contadorColor = 0;
const int umbralEstable = 3; // Número de lecturas consecutivas para confirmar color

void setup() {
  Serial.begin(9600);
  direccion.attach(servoPin);
  pinMode(motorPin, OUTPUT);

  Wire.begin();
  if (!huskylens.begin(Wire)) {
    Serial.println("Error al iniciar HuskyLens!");
    while (1);
  }
  huskylens.writeAlgorithm(ALGORITHM_COLOR_RECOGNITION);

  Serial.println("Sistema iniciado. Reconocimiento continuo de colores...");
}

void loop() {
  if (huskylens.request()) {
    HUSKYLENSResult resultado = huskylens.read();
    
    int colorDetectado = 0;
    if (resultado.ID == 1) colorDetectado = 1; // Naranja
    else if (resultado.ID == 2) colorDetectado = 2; // Azul

    // Filtrar estabilidad
    if (colorDetectado != 0) {
      if (colorDetectado == colorEstable) {
        contadorColor++;
      } else {
        colorEstable = colorDetectado;
        contadorColor = 1;
      }

      if (contadorColor >= umbralEstable && colorEstable != ultimoColor) {
        // Se confirma cambio de color
        Serial.print("Color confirmado: "); Serial.println(colorEstable);

        // Verificar secuencia
        if (ultimoColor == 2 && colorEstable == 1) {
          // Azul → Naranja = Giro derecha
          direccionGiro = 2;
          giroPendiente = true;
          tiempoGiro = millis();
          Serial.println("Secuencia Azul→Naranja → Giro DERECHA");
        } 
        else if (ultimoColor == 1 && colorEstable == 2) {
          // Naranja → Azul = Giro izquierda
          direccionGiro = 1;
          giroPendiente = true;
          tiempoGiro = millis();
          Serial.println("Secuencia Naranja→Azul → Giro IZQUIERDA");
        }

        ultimoColor = colorEstable;
      }
    }
  }

  // Control de giro momentáneo
  if (giroPendiente) {
    if (direccionGiro == 1) direccion.write(120); // Izquierda
    else if (direccionGiro == 2) direccion.write(30); // Derecha

    analogWrite(motorPin, 120);

    if (millis() - tiempoGiro > 1000) { // Mantener giro 1 seg
      giroPendiente = false;
      direccion.write(87); // Recto
      Serial.println("Regresando a posición recta");
    }
  } else {
    // Conducción recta
    direccion.write(87);
    analogWrite(motorPin, 160);
  }
}
