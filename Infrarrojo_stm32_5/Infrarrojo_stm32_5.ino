/*
  Termocupla tipo J - Autoajuste y control serial
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

// --- Configuración y parámetros ---
struct Config {
  float emissivity = 0.30;
  float T_min = 37.0;
  float T_max = 620.0;
  float slope_base = 32.67;
  float offset_mV = 0.0;
  bool autoDetect = true;
  bool autoTuneEnabled = true;
} config;

const float VREF = 3.3;
const int dac_max = 4095;
const float resistor_divider = 101.0;

#define EEPROM_ADDR 0
#define NUM_SAMPLES 10
float tempBuffer[NUM_SAMPLES];
int sampleIndex = 0;
bool bufferFull = false;
float lastAvg = 0;
bool usingHighProfile = false;

// --- EEPROM ---
void saveConfig() {
  EEPROM.put(EEPROM_ADDR, config);
  Serial1.println("Configuracion guardada en EEPROM.");
}

void loadConfig() {
  EEPROM.get(EEPROM_ADDR, config);
  if (config.emissivity < 0.01f || config.emissivity > 1.0f) config.emissivity = 0.30f;
  if (config.T_min < 0 || config.T_max <= config.T_min) {
    config.T_min = 37.0f;
    config.T_max = 620.0f;
  }
  if (config.slope_base < 1.0f || config.slope_base > 100.0f) config.slope_base = 32.67f;
  if (abs(config.offset_mV) > 100.0f) config.offset_mV = 0.0f;
}

// --- mV cálculo ---
float calculateOutputMilliVolts(float temp) {
  if (temp < config.T_min) temp = config.T_min;
  float slope = config.slope_base / (config.T_max - config.T_min);
  return slope * (temp - config.T_min) + config.offset_mV;
}

void setAnalogOutput(float mV) {
  float Vout = mV / 1000.0 / resistor_divider;
  int value = constrain((int)(Vout / VREF * dac_max), 0, dac_max);
  dac.analogWrite(value);
}

// --- Autoajuste por salto térmico ---
void detectTempJump(float avgTemp) {
  if (!config.autoDetect) return;

  float delta = avgTemp - lastAvg;
  if (fabs(delta) > 15.0) {
    if (!usingHighProfile) {
      mlx.writeEmissivity(0.45);
      usingHighProfile = true;
      Serial1.println("Emisividad auto-ajustada: 0.45 (modo en uso)");
    }
  } else if (avgTemp < 80.0 && usingHighProfile) {
    mlx.writeEmissivity(0.08);
    usingHighProfile = false;
    Serial1.println("Emisividad auto-ajustada: 0.08 (modo en reposo)");
  }
  lastAvg = avgTemp;
}

// --- AutoTune ---
void autoTuneEmissivity(float targetTemp) {
  if (!config.autoTuneEnabled) {
    Serial1.println("AUTO TUNE deshabilitado.");
    return;
  }

  float bestError = 999;
  float bestEmi = config.emissivity;

  for (float e = 1.00; e >= 0.01; e -= 0.01) {
    mlx.writeEmissivity(e);
    delay(200);
    float t = mlx.readObjectTempC();
    float err = fabs(t - targetTemp);
    if (err < bestError) {
      bestError = err;
      bestEmi = e;
    }
    if (err < 0.1) break;
  }

  mlx.writeEmissivity(bestEmi);
  config.emissivity = bestEmi;
  saveConfig();
  Serial1.print("AUTO TUNE OK. Nueva emisividad: ");
  Serial1.println(bestEmi);
}

// --- Mostrar estado ---
void printStatus() {
  Serial1.println("--- CONFIGURACION ACTUAL ---");
  Serial1.print("EMI: "); Serial1.println(config.emissivity);
  Serial1.print("TMIN: "); Serial1.println(config.T_min);
  Serial1.print("TMAX: "); Serial1.println(config.T_max);
  Serial1.print("SLOPE: "); Serial1.println(config.slope_base);
  Serial1.print("OFFSET: "); Serial1.println(config.offset_mV);
  Serial1.print("AutoDetect: "); Serial1.println(config.autoDetect ? "ON" : "OFF");
  Serial1.print("AutoTune: "); Serial1.println(config.autoTuneEnabled ? "ON" : "OFF");
  Serial1.println("----------------------------");
}

// --- Comandos Serial ---
void parseSerialCommand() {
  if (!Serial1.available()) return;
  String cmd = Serial1.readStringUntil('\n');
  cmd.trim();

  if (cmd.startsWith("EMI ")) config.emissivity = cmd.substring(4).toFloat();
  else if (cmd.startsWith("TMIN ")) config.T_min = cmd.substring(5).toFloat();
  else if (cmd.startsWith("TMAX ")) config.T_max = cmd.substring(5).toFloat();
  else if (cmd.startsWith("SLOPE ")) config.slope_base = cmd.substring(6).toFloat();
  else if (cmd.startsWith("OFFSET ")) config.offset_mV = cmd.substring(7).toFloat();
  else if (cmd == "STATUS") printStatus();
  else if (cmd == "SAVE") saveConfig();
  else if (cmd == "RESET") NVIC_SystemReset();
  else if (cmd.startsWith("TUNE ")) autoTuneEmissivity(cmd.substring(5).toFloat());
  else if (cmd == "AUTODET ON") { config.autoDetect = true; Serial1.println("AutoDetect ACTIVADO"); }
  else if (cmd == "AUTODET OFF") { config.autoDetect = false; Serial1.println("AutoDetect DESACTIVADO"); }
  else if (cmd == "AUTOTUNE ON") { config.autoTuneEnabled = true; Serial1.println("AutoTune ACTIVADO"); }
  else if (cmd == "AUTOTUNE OFF") { config.autoTuneEnabled = false; Serial1.println("AutoTune DESACTIVADO"); }
  else Serial1.println("Comando no reconocido");
}

// --- SETUP ---
void setup() {
  Serial1.begin(9600);
  Serial.begin(115200);
  Wire.begin();

  loadConfig();

  if (!mlx.begin()) NVIC_SystemReset();
  if (!dac.begin()) NVIC_SystemReset();

  mlx.writeEmissivity(config.emissivity);
  IWatchdog.begin(WATCHDOG_TIMEOUT);
}

// --- LOOP ---
void loop() {
  IWatchdog.reload();
  parseSerialCommand();

  float temp = mlx.readObjectTempC();
  tempBuffer[sampleIndex] = temp;
  sampleIndex = (sampleIndex + 1) % NUM_SAMPLES;
  if (sampleIndex == 0) bufferFull = true;

  float avg = 0;
  int count = bufferFull ? NUM_SAMPLES : sampleIndex;
  for (int i = 0; i < count; i++) avg += tempBuffer[i];
  avg /= count;

  detectTempJump(avg);

  float mV = calculateOutputMilliVolts(avg);
  setAnalogOutput(mV);

  delay(200); // Control de tasa de muestreo
}
