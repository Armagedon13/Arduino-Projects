/*
  Termocupla tipo J - Configurable por Serial1 (PA9, PA10)
  STM32F103C8T6 + MLX90614 + MCP4725 + CP2102 (UART)
*/

#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <MCP4725.h>
#include <IWatchdog.h>
#include <EEPROM.h>

#define DAC_ADDR 0x60
#define WATCHDOG_TIMEOUT 5000000

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
MCP4725 dac(DAC_ADDR);

// Pines Serial1 (UART externo via CP2102)
#define SERIAL_PORT Serial1

// Estructura de parámetros configurables
struct Config {
  float emissivity = 0.18;
  float T_min = 37.0;
  float T_max = 620.0;
  float slope_base = 32.67;  // mV en salida del divisor resistivo
  float offset_mV = 0.0;     // Offset en la salida
} config;

const float VREF = 3.3;
const int dac_max = 4095;
const float resistor_divider = 101.0;

#define EEPROM_ADDR 0
#define NUM_SAMPLES 10
float tempBuffer[NUM_SAMPLES];
int sampleIndex = 0;
bool bufferFull = false;

void saveConfig() {
  EEPROM.put(EEPROM_ADDR, config);
  SERIAL_PORT.println("Configuracion guardada en EEPROM.");
}

void loadConfig() {
  EEPROM.get(EEPROM_ADDR, config);
  if (config.emissivity < 0.01f || config.emissivity > 1.0f) config.emissivity = 1.0f;
  if (config.T_min < 0 || config.T_max < config.T_min) {
    config.T_min = 37.0f;
    config.T_max = 620.0f;
  }
  if (config.slope_base < 1.0f || config.slope_base > 100.0f) config.slope_base = 32.67f;
  if (abs(config.offset_mV) > 100.0f) config.offset_mV = 0.0f;
}

float calculateOutputMilliVolts(float temp) {
  if (temp < config.T_min) temp = config.T_min;
  float slope = config.slope_base / (config.T_max - config.T_min);
  return slope * (temp - config.T_min) + config.offset_mV;
}

void printStatus() {
  SERIAL_PORT.println("--- CONFIGURACION ACTUAL ---");
  SERIAL_PORT.print("EMI: "); SERIAL_PORT.println(config.emissivity);
  SERIAL_PORT.print("TMIN: "); SERIAL_PORT.println(config.T_min);
  SERIAL_PORT.print("TMAX: "); SERIAL_PORT.println(config.T_max);
  SERIAL_PORT.print("SLOPE: "); SERIAL_PORT.println(config.slope_base);
  SERIAL_PORT.print("OFFSET: "); SERIAL_PORT.println(config.offset_mV);
  SERIAL_PORT.println("----------------------------");
}

void parseSerialCommand() {
  if (!SERIAL_PORT.available()) return;
  String cmd = SERIAL_PORT.readStringUntil('\n');
  cmd.trim();

  if (cmd.startsWith("EMI ")) config.emissivity = cmd.substring(4).toFloat();
  else if (cmd.startsWith("TMIN ")) config.T_min = cmd.substring(5).toFloat();
  else if (cmd.startsWith("TMAX ")) config.T_max = cmd.substring(5).toFloat();
  else if (cmd.startsWith("SLOPE ")) config.slope_base = cmd.substring(6).toFloat();
  else if (cmd.startsWith("OFFSET ")) config.offset_mV = cmd.substring(7).toFloat();
  else if (cmd.equals("STATUS")) printStatus();
  else if (cmd.equals("SAVE")) saveConfig();
  else if (cmd.equals("RESET")) NVIC_SystemReset();
  else SERIAL_PORT.println("Comando no reconocido");
}

void setup() {
  Serial.begin(115200);      // Debug interno por USB (opcional)
  SERIAL_PORT.begin(9600);   // UART externo (CP2102)
  Wire.begin();

  loadConfig();

  if (!mlx.begin()) NVIC_SystemReset();
  if (!dac.begin()) NVIC_SystemReset();

  mlx.writeEmissivity(config.emissivity);

  IWatchdog.begin(WATCHDOG_TIMEOUT);
}

void loop() {
  IWatchdog.reload();
  parseSerialCommand();

  float tempC = mlx.readObjectTempC();
  tempBuffer[sampleIndex] = tempC;
  sampleIndex = (sampleIndex + 1) % NUM_SAMPLES;
  if (sampleIndex == 0) bufferFull = true;

  float tempAvg = 0;
  int count = bufferFull ? NUM_SAMPLES : sampleIndex;
  for (int i = 0; i < count; i++) tempAvg += tempBuffer[i];
  tempAvg /= count;

  float output_mV = calculateOutputMilliVolts(tempAvg);
  float V_DAC = (output_mV * resistor_divider) / 1000.0;
  int dac_value = constrain(round((V_DAC / VREF) * dac_max), 0, dac_max);

  dac.setValue(dac_value);

  delay(500);
}
