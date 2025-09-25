#include <Servo.h>

// Pines de conexión
const int trigPin = 7;
const int echoPin = 8;
const int motorPin = 6;     // Motor DC con PWM
const int servoPin = 10;    // Servo para dirección

// Constantes del comportamiento
const int DISTANCIA_UMBRAL = 45;    // Distancia en cm para detectar obstáculo
const int TIEMPO_DETENCION = 1000;  // 1 segundo para detenerse
const int TIEMPO_RETROCESO = 1500;  // 1.5 segundos para retroceder (~30 cm)
const int TIEMPO_GIRO = 5000;       // 5 segundos para girar
const int ANGULO_GIRO = 45;         // 45 grados para girar a la izquierda

// Velocidades del motor
const int VELOCIDAD_AVANCE = 210;   // Velocidad normal (0-255)
const int VELOCIDAD_RETROCESO = 180; // Velocidad de retroceso (0-255)
const int VELOCIDAD_DETENIDO = 0;   // Motor detenido

// Estados del vehículo
enum Estado {
  AVANZANDO,
  DETENIENDO,
  RETROCEDIENDO,
  GIRANDO
};

// Variables globales
Servo direccion;
Estado estadoActual = AVANZANDO;
unsigned long tiempoEstado = 0;
int distanciaObstaculo = 0;

void setup() {
  Serial.begin(9600);
  
  // Configurar pines
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(motorPin, OUTPUT);
  
  // Configurar servo
  direccion.attach(servoPin);
  direccion.write(90); // Posición central (recto)
  
  Serial.println("Iniciando vehiculo autonomo...");
  Serial.println("Comportamiento: Detectar -> Parar -> Retroceder -> Girar -> Continuar");
}

void loop() {
  // Medir distancia continuamente
  distanciaObstaculo = medirDistancia();
  
  // Mostrar información por serial
  Serial.print("Distancia: ");
  Serial.print(distanciaObstaculo);
  Serial.print(" cm | Estado: ");
  
  // Máquina de estados del vehículo
  switch (estadoActual) {
    case AVANZANDO:
      Serial.println("AVANZANDO");
      avanzar();
      
      // Si detecta un obstáculo, cambiar estado
      if (distanciaObstaculo > 0 && distanciaObstaculo < DISTANCIA_UMBRAL) {
        cambiarEstado(DETENIENDO);
      }
      break;
      
    case DETENIENDO:
      Serial.println("DETENIENDO");
      detener();
      
      // Después del tiempo de detención, cambiar a retroceso
      if (millis() - tiempoEstado >= TIEMPO_DETENCION) {
        cambiarEstado(RETROCEDIENDO);
      }
      break;
      
    case RETROCEDIENDO:
      Serial.println("RETROCEDIENDO");
      retroceder();
      
      // Después del tiempo de retroceso, cambiar a giro
      if (millis() - tiempoEstado >= TIEMPO_RETROCESO) {
        cambiarEstado(GIRANDO);
      }
      break;
      
    case GIRANDO:
      Serial.println("GIRANDO");
      girar();
      
      // Después del tiempo de giro, volver a avanzar
      if (millis() - tiempoEstado >= TIEMPO_GIRO) {
        cambiarEstado(AVANZANDO);
      }
      break;
  }
  
  delay(100); // Pequeña pausa para estabilidad
}

// Función para medir distancia con sensor ultrasónico
int medirDistancia() {
  long duracion, distancia;
  
  // Enviar pulso ultrasónico
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Calcular distancia
  duracion = pulseIn(echoPin, HIGH);
  distancia = duracion * 0.034 / 2;
  
  return distancia;
}

// Función para cambiar de estado
void cambiarEstado(Estado nuevoEstado) {
  estadoActual = nuevoEstado;
  tiempoEstado = millis();
  Serial.print("Cambiando a estado: ");
  Serial.println(estadoActual);
}

// Comportamiento: Avanzar recto
void avanzar() {
  direccion.write(90);           // Dirección recta
  analogWrite(motorPin, VELOCIDAD_AVANCE); // Avanzar a velocidad normal
}

// Comportamiento: Detenerse completamente
void detener() {
  analogWrite(motorPin, VELOCIDAD_DETENIDO); // Detener motor
}

// Comportamiento: Retroceder
void retroceder() {
  direccion.write(90);           // Mantener dirección recta
  analogWrite(motorPin, VELOCIDAD_RETROCESO); // Retroceder
}

// Comportamiento: Girar a la izquierda
void girar() {
  direccion.write(90 - ANGULO_GIRO); // Girar a la izquierda
  analogWrite(motorPin, VELOCIDAD_AVANCE); // Avanzar lentamente mientras gira
}

// Comportamiento: Volver a posición recta
void volverRecto() {
  direccion.write(90); // Volver a posición central
}
