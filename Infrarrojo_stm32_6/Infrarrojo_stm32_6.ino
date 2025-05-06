#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <MCP4725.h>
#include <IWatchdog.h>
#include <EEPROM.h>
#include "HardwareSerial.h"

#define DAC_ADDR 0x60
#define WATCHDOG_TIMEOUT 5000000

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
MCP4725 dac(DAC_ADDR);

const float VREF = 3.3;
const int dac_max = 4095;
const float resistor_divider = 101.0;
#define EEPROM_ADDR 0

// Estructura con todos los parámetros configurables
struct Config {
  float emissivity = 0.18;
  float T_min = 37.0;
  float T_max = 620.0;
  float slope_base = 32.67;
  float offset_mV = 0.0;
  float alpha = 0.2;              // Suavizado EMA
  uint32_t sampling_interval = 500; // Intervalo de muestreo en ms
} config;

float emaTemp = NAN;
unsigned long lastSampleTime = 0;

void saveConfig() {
  EEPROM.put(EEPROM_ADDR, config);
  Serial1.println("Configuracion guardada en EEPROM.");
}

void loadConfig() {
  EEPROM.get(EEPROM_ADDR, config);
  bool valid = true;
  if (isnan(config.emissivity) || config.emissivity < 0.01 || config.emissivity > 1.0) valid = false;
  if (isnan(config.T_min) || config.T_min < 0) valid = false;
  if (isnan(config.T_max) || config.T_max <= config.T_min || config.T_max > 1000) valid = false;
  if (isnan(config.slope_base) || config.slope_base <= 0.0 || config.slope_base > 100.0) valid = false;
  if (isnan(config.offset_mV) || fabs(config.offset_mV) > 100.0) valid = false;
  if (isnan(config.alpha) || config.alpha <= 0.0 || config.alpha > 1.0) valid = false;
  if (config.sampling_interval < 100 || config.sampling_interval > 60000) valid = false;

  if (!valid) {
    Serial1.println("EEPROM invalida, usando valores por defecto.");
    config = {0.18, 37.0, 620.0, 32.67, 0.0, 0.2, 500};
  }
}

float calculateOutputMilliVolts(float temp) {
  if (temp < config.T_min) temp = config.T_min;
  float slope = config.slope_base / (config.T_max - config.T_min);
  return slope * (temp - config.T_min) + config.offset_mV;
}

void printStatus() {
  Serial1.println("--- CONFIGURACION ACTUAL ---");
  Serial1.print("EMI: "); Serial1.println(config.emissivity, 2);
  Serial1.print("TMIN: "); Serial1.println(config.T_min, 2);
  Serial1.print("TMAX: "); Serial1.println(config.T_max, 2);
  Serial1.print("SLOPE: "); Serial1.println(config.slope_base, 2);
  Serial1.print("OFFSET: "); Serial1.println(config.offset_mV, 3);
  Serial1.print("ALPHA: "); Serial1.println(config.alpha, 3);
  Serial1.print("SAMPLE_MS: "); Serial1.println(config.sampling_interval);
  Serial1.print("TEMP_EMA: "); Serial1.print(emaTemp, 2); Serial1.println(" °C");
  Serial1.println("----------------------------");
}

void parseSerialCommand() {
  if (!Serial1.available()) return;
  String cmd = Serial1.readStringUntil('\n');
  cmd.trim();

  if (cmd.startsWith("EMI ")) config.emissivity = cmd.substring(4).toFloat();
  else if (cmd.startsWith("TMIN ")) config.T_min = cmd.substring(5).toFloat();
  else if (cmd.startsWith("TMAX ")) config.T_max = cmd.substring(5).toFloat();
  else if (cmd.startsWith("SLOPE ")) config.slope_base = cmd.substring(6).toFloat();
  else if (cmd.startsWith("OFFSET ")) config.offset_mV = cmd.substring(7).toFloat();
  else if (cmd.startsWith("ALPHA ")) config.alpha = cmd.substring(6).toFloat();
  else if (cmd.startsWith("SAMPLE ")) config.sampling_interval = constrain(cmd.substring(7).toInt(), 100, 60000);
  else if (cmd.equals("STATUS")) printStatus();
  else if (cmd.equals("SAVE")) saveConfig();
  else if (cmd.equals("RESET")) NVIC_SystemReset();
  else Serial1.println("Comando no reconocido");
}

bool restartMLX() {
  Wire.end();
  delay(100);
  Wire.begin();
  delay(100);
  return mlx.begin();
}

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);
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

  if (millis() - lastSampleTime >= config.sampling_interval) {
    lastSampleTime = millis();

    float tempC = mlx.readObjectTempC();
    if (isnan(tempC)) {
      Serial1.println("ERROR: Lectura MLX es NaN. Intentando reconectar...");
      if (!restartMLX()) {
        Serial1.println("Fallo al reconectar. Reiniciando...");
        delay(500);
        NVIC_SystemReset();
      }
      return;
    }

    if (isnan(emaTemp)) emaTemp = tempC;
    else emaTemp = config.alpha * tempC + (1.0 - config.alpha) * emaTemp;

    float output_mV = calculateOutputMilliVolts(emaTemp);
    float V_DAC = (output_mV * resistor_divider) / 1000.0;
    int dac_value = constrain(round((V_DAC / VREF) * dac_max), 0, dac_max);
    dac.setValue(dac_value);
  }
}
