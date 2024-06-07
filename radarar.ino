#include <Servo.h>

const int trigPin = 10;
const int echoPin = 11;
const int laserPin = 13;
const int buttonPin = 8; // Botón para detener el movimiento automático
const int buttonRightPin = 7; // Botón para mover el servo a la derecha en modo manual
const int buttonLeftPin = 9; // Botón para mover el servo a la izquierda en modo manual

long duration;
int distance;
Servo myServo;

bool isAutomatic = true; // Variable para controlar el estado automático/manual
unsigned long lastDebounceTime = 0; // Tiempo de la última pulsación del botón
unsigned long debounceDelay = 200; // Retardo para el debounce
int currentPosition = 90; // Posición inicial del servo

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(laserPin, OUTPUT); // Configura el pin del láser como salida
  pinMode(buttonPin, INPUT_PULLUP); // Configura el pin del botón como entrada con pull-up
  pinMode(buttonRightPin, INPUT_PULLUP); // Configura el pin del botón derecho como entrada con pull-up
  pinMode(buttonLeftPin, INPUT_PULLUP); // Configura el pin del botón izquierdo como entrada con pull-up

  Serial.begin(9600);
  myServo.attach(12);
  myServo.write(currentPosition); // Posiciona el servo en la posición inicial
}

void loop() {
  checkButton();

  if (isAutomatic) {
    automaticMovement();
  } else {
    // Detiene el movimiento del servo y permite el control manual
    stopAutomaticMovement();
    manualMovement();
  }
}

void checkButton() {
  int buttonState = digitalRead(buttonPin);
  if (buttonState == LOW) {
    unsigned long currentTime = millis();
    if (currentTime - lastDebounceTime > debounceDelay) {
      isAutomatic = !isAutomatic; // Cambia entre automático y manual
      lastDebounceTime = currentTime;
    }
    // Espera a que el botón se suelte
    while (digitalRead(buttonPin) == LOW);
    delay(50); // Pequeño retardo para asegurar que el botón se haya soltado
  }
}

void automaticMovement() {
  for (int i = 15; i <= 165; i++) {
    if (!isAutomatic) return; // Si el modo cambia a manual, salir del bucle
    moveServo(i);
    delay(30);
    controlLaser();
    checkButton(); // Verifica el estado del botón dentro del bucle
  }

  for (int i = 165; i > 15; i--) {
    if (!isAutomatic) return; // Si el modo cambia a manual, salir del bucle
    moveServo(i);
    delay(30);
    controlLaser();
    checkButton(); // Verifica el estado del botón dentro del bucle
  }
}

void stopAutomaticMovement() {
  moveServo(currentPosition); // Mantiene el servo en la posición actual
  digitalWrite(laserPin, LOW); // Apaga el láser
}

void manualMovement() {
  if (digitalRead(buttonRightPin) == LOW) {
    if (currentPosition < 165) {
      currentPosition++;
      moveServo(currentPosition);
      delay(10); // Debounce delay
    }
  }
  if (digitalRead(buttonLeftPin) == LOW) {
    if (currentPosition > 15) {
      currentPosition--;
      moveServo(currentPosition);
      delay(10); // Debounce delay
    }
  }
  controlLaser();
}

void moveServo(int position) {
  myServo.write(position);
  distance = calculateDistance();
  Serial.print(position);
  Serial.print(",");
  Serial.print(distance);
  Serial.print(".");
}

void controlLaser() {
  if (distance < 10) {
    digitalWrite(laserPin, HIGH); // Enciende el láser si la distancia es menor a 10
  } else {
    digitalWrite(laserPin, LOW); // Apaga el láser si la distancia no es menor a 10
  }
}


int calculateDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  return distance;
}