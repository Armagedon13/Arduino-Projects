#include <Arduino.h>
#include <driver/dac.h>
#include <ESP32Encoder.h>

// Instancia del encoder (usaremos uno)
ESP32Encoder encoder;

// Configuración del pin para el DAC (usaremos DAC1 en GPIO25)
const int dacPin = 25; 
// Rango del DAC del ESP32: 8 bits (0 a 255)
const int dacMax = 255;

// Definir límites del encoder para controlar la salida
// Queremos que el encoder ajuste de 0 a 255, de modo que
// la salida del DAC sea de 0 a 3.3 V (y luego se reduzca externamente a 0-50 mV)
const long encoderMin = 0;
const long encoderMax = 255;

void setup() {
  Serial.begin(115200);
  
  // Habilitar las resistencias internas con pull-up (opcional)
  ESP32Encoder::useInternalWeakPullResistors = puType::up;
  
  // Configurar el encoder en "half-quad" usando pines 19 y 18
  encoder.attachHalfQuad(19, 18);
  
  // Inicializar el contador del encoder a 0
  encoder.clearCount();
  encoder.setCount(0);
  
  Serial.println("Encoder inicializado. Gira para ajustar la salida del DAC.");
}

void loop() {
  // Leer el valor actual del encoder
  long count = encoder.getCount();
  
  // Limitar el conteo entre encoderMin y encoderMax
  if(count < encoderMin) {
    count = encoderMin;
    encoder.setCount(encoderMin);
  } else if(count > encoderMax) {
    count = encoderMax;
    encoder.setCount(encoderMax);
  }
  
  // El valor del encoder se usará directamente como valor para el DAC (0 a 255)
  int dacValue = (int)count;
  
  // Escribir el valor en el DAC
  //dacWrite(dacPin, dacValue);
  dac_output_voltage(0, dacValue);
  
  // Mostrar información por el monitor serie
  Serial.print("Encoder count: ");
  Serial.print(count);
  Serial.print(" -> DAC value: ");
  Serial.println(dacValue);
  
  delay(100);
}
