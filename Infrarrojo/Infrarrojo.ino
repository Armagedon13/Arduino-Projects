#include <Wire.h>
#include <Adafruit_MLX90614.h>

// Instancia del sensor MLX90614
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// Pin PWM de salida (asegúrate de usar un pin compatible con PWM en tu placa STM32)
const int pwmPin = PA8;

// Parámetros para el mapeo PWM
const int pwmMax = 255;
const float Vref = 3.3;  // Tensión de referencia para la conversión (por ejemplo, 3.3V)

// Definición de la tabla de referencia para la termocupla tipo J
// Estos valores son ejemplos basados en la tabla proporcionada.
// Las temperaturas están en °C y los voltajes en milivoltios (mV)
const int numPoints = 5;
float tempPoints[numPoints] = {0.0, 50.0, 100.0, 150.0, 200.0};
float voltagePoints[numPoints] = {0.0, 2.47, 4.095, 5.68, 7.27};  // Valores aproximados en mV

// Función de interpolación lineal para calcular el voltaje (en mV) dado una temperatura
float interpolate(float temperature) {
  if (temperature <= tempPoints[0]) return voltagePoints[0];
  if (temperature >= tempPoints[numPoints - 1]) return voltagePoints[numPoints - 1];
  
  for (int i = 0; i < numPoints - 1; i++) {
    if (temperature >= tempPoints[i] && temperature < tempPoints[i+1]) {
      float ratio = (temperature - tempPoints[i]) / (tempPoints[i+1] - tempPoints[i]);
      return voltagePoints[i] + ratio * (voltagePoints[i+1] - voltagePoints[i]);
    }
  }
  return voltagePoints[numPoints - 1];
}

void setup() {
  Serial.begin(9600);
  mlx.begin();
  pinMode(pwmPin, OUTPUT); 
}

void loop() {
  // Lee la temperatura del objeto en °C
  analogWriteResolution(12);
  float temperatura = mlx.readObjectTempC();
  
  // Calcula el voltaje simulado en mV a partir de la temperatura usando la interpolación
  float voltaje_mV = interpolate(temperatura);
  
  // Convierte de mV a V
  float voltaje = voltaje_mV / 1000.0;
  
  // Mapea el voltaje (suponiendo una salida máxima de Vref) al rango PWM (0-255)
  int valorPWM = (int)((voltaje / Vref) * pwmMax);
  valorPWM = constrain(valorPWM, 0, pwmMax);
  
  // Escribe el valor PWM al pin de salida
  analogWrite(pwmPin, valorPWM);
  
  // Imprime los valores para monitoreo
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.print(" °C, Voltaje simulado: ");
  Serial.print(voltaje_mV);
  Serial.print(" mV, Valor PWM: ");
  Serial.println(valorPWM);
  
  delay(1000);
}
