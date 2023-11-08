// Definimos las variables
const int numMotors = 4;   // Cantidad de motores
int motorPins[numMotors] = {PA8, PA9, PB6_ALT0, PB7_ALT0}; // Pines de los motores
int buttonPins[numMotors] = {PB12, PB13, PB14, PB15}; // Pines de los botones
int ledPins[numMotors] = {PA4, PA5, PA6, PA7}; // Pines de los LEDs
int potPins[numMotors] = {PA0, PA1, PA2, PA3}; // Pines de los potenciómetros

int potValues[numMotors] = {0}; // Valores de los potenciómetros
bool motorEnabled[numMotors] = {false}; // Estados de los motores
int motorSpeeds[numMotors] = {0}; // Velocidades de los motores
unsigned long lastMotorUpdateTimes[numMotors] = {0}; // Tiempos de la última actualización

// Variables para el debounce de los botones
int buttonStates[numMotors] = {LOW};
int lastButtonStates[numMotors] = {LOW};
unsigned long lastDebounceTimes[numMotors] = {0};
unsigned long debounceDelay = 10; // Tiempo de debounce en milisegundos

void setup() {
  // Analog resolution 12 bits
  analogReadResolution(12);
  // PWM frequency
  analogWriteFrequency(200);
  // PWM resolution to 12 bits
  analogWriteResolution(12);

  // Configuramos pines de motor, botón y LED
  for (int i = 0; i < numMotors; i++) {
    pinMode(motorPins[i], OUTPUT);
    pinMode(buttonPins[i], INPUT_PULLUP);
    pinMode(ledPins[i], OUTPUT);
    pinMode(potPins[i], INPUT);
  }
}

void loop() {
  for (int i = 0; i < numMotors; i++) {
    // Leemos el estado actual del botón con debounce
    int reading = digitalRead(buttonPins[i]);
    if (reading != lastButtonStates[i]) {
      lastDebounceTimes[i] = millis();
    }

    if ((millis() - lastDebounceTimes[i]) > debounceDelay) {
      if (reading != buttonStates[i]) {
        buttonStates[i] = reading;

        // Comprobamos si el botón ha sido presionado
        if (buttonStates[i] == LOW) {
          // Cambiamos el estado del motor
          motorEnabled[i] = !motorEnabled[i];
        }
      }
    }

    // Si el motor está habilitado, establecemos la velocidad del PWM según el valor del potenciómetro
    if (motorEnabled[i]) {
      // Actualizamos el estado del LED
      digitalWrite(ledPins[i], HIGH);
      potValues[i] = analogRead(potPins[i]);

      // Aplicamos arranque suave si la velocidad es alta
      if (potValues[i] > motorSpeeds[i]) {
        unsigned long currentTime = millis();
        if (currentTime - lastMotorUpdateTimes[i] >= 10) {
          motorSpeeds[i] += 10; // Incremento gradual de velocidad
          if (motorSpeeds[i] > potValues[i]) {
            motorSpeeds[i] = potValues[i]; // Limitamos la velocidad al valor del potenciómetro
          }
          analogWrite(motorPins[i], motorSpeeds[i]);
          lastMotorUpdateTimes[i] = currentTime;
        }
      } else {
        analogWrite(motorPins[i], potValues[i]);
        motorSpeeds[i] = potValues[i]; // Establecemos la velocidad actual
      }
    } else {
      // Actualizamos el estado del LED
      digitalWrite(ledPins[i], LOW);
      // Si el motor está deshabilitado, apagamos el motor
      analogWrite(motorPins[i], 0);
      motorSpeeds[i] = 0; // Restablecemos la velocidad para un arranque suave la próxima vez
    }

    // Almacenamos el estado actual del botón como el estado anterior
    lastButtonStates[i] = reading;
  }
}
