/*
Programa para convertir entrada a mV para termocupla J
STM32F103C8T6 - MLX90614 - MCP4725
*/

#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <MCP4725.h>
#include <IWatchdog.h>
#include <EEPROM.h>

// Dirección I2C del MCP4725
#define DAC_ADDR 0x60  

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
MCP4725 dac(DAC_ADDR);

// Configuración del Watchdog
#define WATCHDOG_TIMEOUT 5000000  // 5 segundos

// EEPROM: Dirección donde se guarda la emisividad
#define EEPROM_ADDR 0

// Emisividad por defecto
double emissivity = 0.18;

// Configuración del DAC
const float VREF = 3.3;  
const int dac_max = 4095; 

// Rango de temperatura
const float T_min = 37.0;
const float T_max = 620.0;

// Escalado de la salida
const float slope = 32.67 / (T_max - T_min);  

// Promedio móvil
#define NUM_SAMPLES 10
float tempBuffer[NUM_SAMPLES] = {0};
int sampleIndex = 0;
bool bufferFull = false;

// Función para convertir temperatura a mV en la salida del divisor resistivo
float tempToOutputMilliVolts(float temp) {
  if (temp < T_min) temp = T_min;
  return slope * (temp - T_min);  
}

// Función para guardar emisividad en EEPROM
void saveEmissivity(double value) {
  EEPROM.put(EEPROM_ADDR, value);
  Serial.println("Emisividad guardada en EEPROM.");
}

// Función para cargar emisividad desde EEPROM
void loadEmissivity() {
  EEPROM.get(EEPROM_ADDR, emissivity);
  if (emissivity < 0.1 || emissivity > 1.0) {
    emissivity = 1.0;  // Valor por defecto si EEPROM no tiene datos válidos
  }
}

// Función para procesar comandos seriales
void checkSerialCommand() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    if (command.startsWith("SET ")) {
      double newEmissivity = command.substring(4).toFloat();
      if (newEmissivity >= 0.1 && newEmissivity <= 1.0) {
        Serial.print("Nueva emisividad: ");
        Serial.println(newEmissivity);
        mlx.writeEmissivity(newEmissivity);
        saveEmissivity(newEmissivity);
        delay(1000);
        NVIC_SystemReset();  // Reinicia el STM32
      } else {
        Serial.println("Valor fuera de rango. Usa SET 0.1 - 1.0");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  // Cargar emisividad desde EEPROM
  loadEmissivity();
  
  // Iniciar sensores I2C
  if (!mlx.begin()) {
    Serial.println("Error: MLX90614 no detectado. Reiniciando...");
    delay(1000);
    NVIC_SystemReset();
  }
  
  if (!dac.begin()) {
    Serial.println("Error: MCP4725 no detectado. Reiniciando...");
    delay(1000);
    NVIC_SystemReset();
  }

  // Aplicar emisividad guardada
  mlx.writeEmissivity(emissivity);
  Serial.print("Emisividad cargada: ");
  Serial.println(emissivity);

  // Iniciar el Watchdog
  IWatchdog.begin(WATCHDOG_TIMEOUT);
  if (!IWatchdog.isEnabled()) {
    Serial.println("Error: Watchdog no habilitado.");
    while (1);
  }
}

void loop() {
  IWatchdog.reload();
  checkSerialCommand();  // Verifica si hay un comando serial para cambiar emisividad

  // Leer temperatura y actualizar el buffer de promedios
  float tempC = mlx.readObjectTempC();
  tempBuffer[sampleIndex] = tempC;
  sampleIndex = (sampleIndex + 1) % NUM_SAMPLES;
  if (sampleIndex == 0) bufferFull = true;

  // Calcular el promedio de temperatura
  float tempAvg = 0;
  int numSamples = bufferFull ? NUM_SAMPLES : sampleIndex;
  for (int i = 0; i < numSamples; i++) {
    tempAvg += tempBuffer[i];
  }
  tempAvg /= numSamples;

  // Convertir temperatura a mV en la salida del divisor
  float output_mV = tempToOutputMilliVolts(tempAvg);

  // V_DAC = output_mV * 101 / 1000   (convierte mV a V)
  float V_DAC = (output_mV * 101.0) / 1000.0;

  // Calculo para el DAC
  int dac_value = constrain(round((V_DAC / VREF) * dac_max), 0, dac_max);
  
  // Enviar salida al DAC
  dac.setValue(dac_value);
  
  // Mostrar datos en el serial
  Serial.print("Temp Avg: ");
  Serial.print(tempAvg);
  Serial.print(" °C -> Output (div): ");
  Serial.print(output_mV);
  Serial.print(" mV -> V_DAC: ");
  Serial.print(V_DAC, 3);
  Serial.print(" V -> DAC: ");
  Serial.println(dac_value);
  
  delay(500);
}
