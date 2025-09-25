#include <Servo.h>

Servo direccion;

// Pines
const int motorPin = 6;
const int servoPin = 10;
const int trigPin = 7;
const int echoPin = 8;

// Posiciones servo
const int SERVO_CENTRO = 87;
const int SERVO_DERECHA = 57;  // 30 grados a la derecha desde centro (87-30=57)

// Velocidades
const int VELOCIDAD_ADELANTE = 190;
const int VELOCIDAD_REVERSA = 170;  // Velocidad para retroceder
const int VELOCIDAD_GIRO = 180;

// Distancia de detección (cm)
const int DISTANCIA_OBSTACULO = 20;
const int DISTANCIA_RETROCESO = 30;  // Distancia a retroceder

// Tiempos
const unsigned long TIEMPO_GIRO = 1000;  // 1 segundo para girar

// Variables de control
enum Estado {AVANZAR, RETROCEDER, GIRAR};
Estado estadoActual = AVANZAR;
unsigned long tiempoInicioAccion = 0;
float distanciaRecorrida = 0;

// Variables para medición de distancia
const int numLecturas = 5;
unsigned int lecturas[numLecturas];
int indiceLectura = 0;
unsigned long total = 0;

// Función para leer distancia con filtro de promedio móvil
unsigned int leerDistanciaCM() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  unsigned long duracion = pulseIn(echoPin, HIGH, 30000UL);
  if (duracion == 0) return 0;
  
  // Filtro de promedio móvil
  total = total - lecturas[indiceLectura];
  lecturas[indiceLectura] = (unsigned int)(duracion / 58.0);
  total = total + lecturas[indiceLectura];
  indiceLectura = (indiceLectura + 1) % numLecturas;
  
  return total / numLecturas;
}

// Función para calcular distancia recorrida (aproximada)
float calcularDistanciaRecorrida(unsigned long tiempo, int velocidad) {
  // Conversión aproximada: velocidad PWM a cm/segundo
  // Esta calibración puede necesitar ajustes según el robot específico
  float cmPorSegundo = velocidad * 0.3;  // Factor de conversión aproximado
  return (tiempo / 1000.0) * cmPorSegundo;
}

void setup() {
  Serial.begin(9600);
  direccion.attach(servoPin);
  pinMode(motorPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Inicializar filtro de distancia
  for (int i = 0; i < numLecturas; i++) {
    lecturas[i] = 0;
  }
  
  direccion.write(SERVO_CENTRO);
  analogWrite(motorPin, VELOCIDAD_ADELANTE);
  Serial.println("Sistema iniciado: Evasión de obstáculos activa");
}

void loop() {
  unsigned int distancia = leerDistanciaCM();
  
  Serial.print("Estado: ");
  Serial.print(estadoActual);
  Serial.print(" | Distancia: ");
  Serial.print(distancia);
  Serial.print(" cm | Distancia Recorrida: ");
  Serial.print(distanciaRecorrida);
  Serial.println(" cm");
  
  switch (estadoActual) {
    case AVANZAR:
      if (distancia > 0 && distancia <= DISTANCIA_OBSTACULO) {
        // Obstáculo detectado, comenzar a retroceder
        estadoActual = RETROCEDER;
        tiempoInicioAccion = millis();
        distanciaRecorrida = 0;
        analogWrite(motorPin, VELOCIDAD_REVERSA);
        Serial.println("Obstáculo detectado. Iniciando retroceso.");
      } else {
        // Continuar avanzando
        direccion.write(SERVO_CENTRO);
        analogWrite(motorPin, VELOCIDAD_ADELANTE);
      }
      break;
      
    case RETROCEDER:
      // Calcular distancia retrocedida
      distanciaRecorrida = calcularDistanciaRecorrida(millis() - tiempoInicioAccion, VELOCIDAD_REVERSA);
      
      if (distanciaRecorrida >= DISTANCIA_RETROCESO) {
        // Ha retrocedido suficiente, iniciar giro
        estadoActual = GIRAR;
        tiempoInicioAccion = millis();
        direccion.write(SERVO_DERECHA);
        analogWrite(motorPin, VELOCIDAD_GIRO);
        Serial.println("Retroceso completado. Iniciando giro.");
      }
      break;
      
    case GIRAR:
      if (millis() - tiempoInicioAccion >= TIEMPO_GIRO) {
        // Giro completado, volver a avanzar
        estadoActual = AVANZAR;
        direccion.write(SERVO_CENTRO);
        analogWrite(motorPin, VELOCIDAD_ADELANTE);
        Serial.println("Giro completado. Volviendo a avanzar.");
      }
      break;
  }
  
  delay(50);  // Pequeña pausa para estabilidad
}
