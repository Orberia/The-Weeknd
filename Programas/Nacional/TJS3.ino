#include <Servo.h>
#include <Wire.h>
#include "HUSKYLENS.h"

HUSKYLENS huskylens;
Servo direccion;

// Pines
const int motorPin = 6;
const int servoPin = 10;
const int trigPin = 7;
const int echoPin = 8;

// Colores HuskyLens
const int COLOR_NARANJA = 1;
const int COLOR_AZUL    = 2;

// Posiciones servo
const int SERVO_CENTRO = 87;
const int SERVO_IZQ    = 125;
const int SERVO_DER    = 30;

// Velocidades y tiempos
const int velocidadRecta = 190;
const int velocidadGiro  = 180;
const unsigned long duracionGiro = 900;
const unsigned long duracionAvoid = 900;

// Distancias (cm)
const unsigned int safetyDistanceCm = 40;
const unsigned int resumeDistanceCm = 41;

// Filtro y secuencia
const int umbralEstable = 3;
const unsigned long esperaSegundoTimeout = 1000;

// Variables para colores
int colorEstable = 0;
int contadorEstable = 0;
int primerColor = 0;
bool esperandoSegundo = false;
unsigned long tiempoPrimerColor = 0;

// Giro
bool girando = false;
int direccionActualGiro = 0; // 1=izquierda, 2=derecha
unsigned long tiempoInicioGiro = 0;
unsigned long duracionGiroActual = duracionGiro;

// Evitación ultrasónica
bool evitando = false;

// ---------- Funciones ----------

unsigned int leerDistanciaCM() {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  unsigned long duracion = pulseIn(echoPin, HIGH, 30000UL);
  if (duracion == 0) return 0;
  return (unsigned int)(duracion / 58.0);
}

void startTurnColor(int dir) {
  girando = true;
  direccionActualGiro = dir;
  tiempoInicioGiro = millis();
  duracionGiroActual = duracionGiro;
  Serial.print("Giro color iniciado: ");
  Serial.println(dir == 1 ? "IZQUIERDA" : "DERECHA");
}

void startAvoidance() {
  if (primerColor == COLOR_AZUL) direccionActualGiro = 2; // derecha
  else if (primerColor == COLOR_NARANJA) direccionActualGiro = 1; // izquierda
  girando = true;
  tiempoInicioGiro = millis();
  duracionGiroActual = duracionGiro + duracionAvoid; // darle tiempo extra
  Serial.println("Obstáculo detectado -> Giro en mismo sentido que colores.");
}

void desactivarEvitacion() {
  evitando = false;
  girando = false;
  direccion.write(SERVO_CENTRO);
  duracionGiroActual = duracionGiro;
  primerColor = 0;
  esperandoSegundo = false;
  colorEstable = 0;
  contadorEstable = 0;
  Serial.println("Evitación finalizada, volviendo a recto.");
}

// --------- Setup ---------
void setup() {
  Serial.begin(9600);
  direccion.attach(servoPin);
  pinMode(motorPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Wire.begin();
  if (!huskylens.begin(Wire)) {
    Serial.println("Error al iniciar HuskyLens!");
    while (1);
  }
  huskylens.writeAlgorithm(ALGORITHM_COLOR_RECOGNITION);

  direccion.write(SERVO_CENTRO);
  analogWrite(motorPin, velocidadRecta);
  Serial.println("Sistema iniciado: secuencia colores + ultrasonido sincronizados.");
}

// --------- Loop ---------
void loop() {
  // Lectura de HuskyLens si no estamos girando
  if (!girando && huskylens.request()) {
    HUSKYLENSResult res = huskylens.read();
    int detectado = 0;
    if (res.ID == COLOR_NARANJA) detectado = COLOR_NARANJA;
    else if (res.ID == COLOR_AZUL) detectado = COLOR_AZUL;

    if (detectado != 0) {
      if (!esperandoSegundo) {
        if (detectado == colorEstable) contadorEstable++;
        else { colorEstable = detectado; contadorEstable = 1; }

        if (contadorEstable >= umbralEstable) {
          primerColor = colorEstable;
          esperandoSegundo = true;
          tiempoPrimerColor = millis();
          colorEstable = 0;
          contadorEstable = 0;
          Serial.print("Primer color confirmado: ");
          Serial.println(primerColor == COLOR_NARANJA ? "NARANJA" : "AZUL");
        }
      } else {
        if (detectado != primerColor) {
          startTurnColor((primerColor == COLOR_NARANJA) ? 1 : 2);
          Serial.println("Segundo color detectado -> Giro inmediato según secuencia.");
        }
      }
    }
  }

  // Timeout segundo color
  if (esperandoSegundo && !girando && millis() - tiempoPrimerColor > esperaSegundoTimeout) {
    primerColor = 0;
    esperandoSegundo = false;
    colorEstable = 0;
    contadorEstable = 0;
    Serial.println("Timeout esperando segundo color, reiniciando secuencia.");
  }

  // Leer distancia ultrasónica y activar evitación al final de colores
  unsigned int distancia = leerDistanciaCM();
  if (!girando && esperandoSegundo == false && distancia > 0 && distancia <= safetyDistanceCm) {
    evitando = true;
    startAvoidance();
  }

  // Mantener giro/evitación
  if (girando) {
    direccion.write((direccionActualGiro == 1) ? SERVO_IZQ : SERVO_DER);
    analogWrite(motorPin, velocidadGiro);

    if (millis() - tiempoInicioGiro >= duracionGiroActual) {
      desactivarEvitacion();
    }
  } else {
    analogWrite(motorPin, velocidadRecta);
    direccion.write(SERVO_CENTRO);
  }
}
