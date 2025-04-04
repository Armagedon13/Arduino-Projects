#include <Arduino.h>

// Pines para el encoder
static const int pinA = PA0;
static const int pinB = PA1;

// Variables volátiles para el manejo del encoder
volatile bool aFlag = false;
volatile bool bFlag = false;
volatile uint16_t encoderPos = 0;   // Valor acumulado del encoder
volatile uint16_t oldEncPos = 0;
volatile uint16_t reading = 0;

// Pin de salida PWM (usar PA8 para la salida PWM)
const int pwmPin = PA8;

void setup() {
  // Configurar pines del encoder con pull-up interno
  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);
  
  // Configurar interrupciones en ambos pines
  attachInterrupt(digitalPinToInterrupt(pinA), PinA_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(pinB), PinB_ISR, RISING);
  
  Serial.begin(115200);

  // Configurar salida PWM
  pinMode(pwmPin, OUTPUT);
  analogWriteResolution(12);  // Resolución de 12 bits (0 a 4095)
  analogWrite(pwmPin, 0);       // Inicialmente apagado
}

void PinA_ISR() {
  noInterrupts();
  // Leer los dos bits correspondientes a PA0 y PA1
  reading = GPIOA->IDR & 0x3;  // Extrae los dos bits menos significativos
  // Si ambos pines están en HIGH (0b11) y se activó el flag aFlag, disminuye el contador
  if (reading == 0b11 && aFlag && encoderPos > 0) {
    encoderPos--;
    bFlag = false;
    aFlag = false;
  }
  // Si solo PA0 está en HIGH (0b01), marca el flag bFlag para indicar dirección
  else if (reading == 0b01) {
    bFlag = true;
  }
  interrupts();
}

void PinB_ISR() {
  noInterrupts();
  reading = GPIOA->IDR & 0x3;
  // Si ambos pines están en HIGH y se activó bFlag, incrementa el contador
  if (reading == 0b11 && bFlag) {
    encoderPos++;
    bFlag = false;
    aFlag = false;
  }
  // Si solo PA1 está en HIGH (0b10), marca el flag aFlag
  else if (reading == 0b10) {
    aFlag = true;
  }
  interrupts();
}

void loop() {
  // Actualiza la salida PWM solo si el valor del encoder ha cambiado
  if(oldEncPos != encoderPos) {
    Serial.print("Encoder: ");
    Serial.println(encoderPos);
    
    // Suponiendo que deseamos usar el rango 0 a 4095 directamente,
    // se puede ajustar el valor si es necesario con constrain().
    int pwmValue = constrain(encoderPos, 0, 4095);
    analogWrite(pwmPin, pwmValue);
    
    oldEncPos = encoderPos;
  }
  delay(10);
}
