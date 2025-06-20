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

// Tabla de interpolación termocupla J de 0 a 1200 °C
const int table_size = 1201;
const float j_temps[table_size] = {
  0.000, 0.050, 0.101, 0.151, 0.202, 0.253, 0.303, 0.354, 0.405, 0.456, 0.507,
    0.558, 0.609, 0.660, 0.711, 0.762, 0.814, 0.865, 0.916, 0.968, 1.019,
    1.071, 1.122, 1.174, 1.226, 1.277, 1.329, 1.381, 1.433, 1.485, 1.537,
    1.589, 1.641, 1.693, 1.745, 1.797, 1.849, 1.902, 1.954, 2.006, 2.059,
    2.111, 2.164, 2.216, 2.269, 2.322, 2.374, 2.427, 2.480, 2.532, 2.585,
    2.638, 2.691, 2.744, 2.797, 2.850, 2.903, 2.956, 3.009, 3.062, 3.116,
    3.169, 3.222, 3.275, 3.329, 3.382, 3.436, 3.489, 3.543, 3.596, 3.650,
    3.703, 3.757, 3.810, 3.864, 3.918, 3.971, 4.025, 4.079, 4.133, 4.187,
    4.240, 4.294, 4.348, 4.402, 4.456, 4.510, 4.564, 4.618, 4.672, 4.726,
    4.781, 4.835, 4.889, 4.943, 4.997, 5.052, 5.106, 5.160, 5.215, 5.269,
    5.323, 5.378, 5.432, 5.487, 5.541, 5.595, 5.650, 5.705, 5.759, 5.814,
    5.868, 5.923, 5.977, 6.032, 6.087, 6.141, 6.196, 6.251, 6.306, 6.360,
    6.415, 6.470, 6.525, 6.579, 6.634, 6.689, 6.744, 6.799, 6.854, 6.909,
    6.964, 7.019, 7.074, 7.129, 7.184, 7.239, 7.294, 7.349, 7.404, 7.459,
    7.514, 7.569, 7.624, 7.679, 7.734, 7.789, 7.844, 7.900, 7.955, 8.010,
    8.065, 8.120, 8.175, 8.231, 8.286, 8.341, 8.396, 8.452, 8.507, 8.562,
    8.618, 8.673, 8.728, 8.783, 8.839, 8.894, 8.949, 9.005, 9.060, 9.115,
    9.171, 9.226, 9.282, 9.337, 9.392, 9.448, 9.503, 9.559, 9.614, 9.669,
    9.725, 9.780, 9.836, 9.891, 9.947, 10.002, 10.057, 10.113, 10.168, 10.224,
    10.279, 10.335, 10.390, 10.446, 10.501, 10.557, 10.612, 10.668, 10.723, 10.779,
    10.834, 10.890, 10.945, 11.001, 11.056, 11.112, 11.167, 11.223, 11.278, 11.334,
    11.389, 11.445, 11.501, 11.556, 11.612, 11.667, 11.723, 11.778, 11.834, 11.889,
    11.945, 12.000, 12.056, 12.111, 12.167, 12.222, 12.278, 12.334, 12.389, 12.445,
    12.500, 12.556, 12.611, 12.667, 12.722, 12.778, 12.833, 12.889, 12.944, 13.000,
    13.056, 13.111, 13.167, 13.222, 13.278, 13.333, 13.389, 13.444, 13.500, 13.555,
    13.611, 13.666, 13.722, 13.777, 13.833, 13.888, 13.944, 13.999, 14.055, 14.110,
    14.166, 14.221, 14.277, 14.332, 14.388, 14.443, 14.499, 14.554, 14.609, 14.665,
    14.720, 14.776, 14.831, 14.887, 14.942, 14.998, 15.053, 15.109, 15.164, 15.219,
    15.275, 15.330, 15.386, 15.441, 15.496, 15.552, 15.607, 15.663, 15.718, 15.773,
    15.829, 15.884, 15.940, 15.995, 16.050, 16.106, 16.161, 16.216, 16.272, 16.327,
    16.383, 16.438, 16.493, 16.549, 16.604, 16.659, 16.715, 16.770, 16.825, 16.881,
    16.936, 16.991, 17.046, 17.102, 17.157, 17.212, 17.268, 17.323, 17.378, 17.434,
    17.489, 17.544, 17.599, 17.655, 17.710, 17.765, 17.820, 17.876, 17.931, 17.986,
    18.041, 18.097, 18.152, 18.207, 18.262, 18.318, 18.373, 18.428, 18.483, 18.538,
    18.594, 18.649, 18.704, 18.759, 18.814, 18.870, 18.925, 18.980, 19.035, 19.090,
    19.146, 19.201, 19.256, 19.311, 19.366, 19.422, 19.477, 19.532, 19.587, 19.642,
    19.697, 19.753, 19.808, 19.863, 19.918, 19.973, 20.028, 20.083, 20.139, 20.194,
    20.249, 20.304, 20.359, 20.414, 20.469, 20.525, 20.580, 20.635, 20.690, 20.745,
    20.800, 20.855, 20.911, 20.966, 21.021, 21.076, 21.131, 21.186, 21.241, 21.297,
    21.352, 21.407, 21.462, 21.517, 21.572, 21.627, 21.683, 21.738, 21.793, 21.848,
    21.903, 21.958, 22.014, 22.069, 22.124, 22.179, 22.234, 22.289, 22.345, 22.400,
    22.455, 22.510, 22.565, 22.620, 22.676, 22.731, 22.786, 22.841, 22.896, 22.952,
    23.007, 23.062, 23.117, 23.172, 23.228, 23.283, 23.338, 23.393, 23.449, 23.504,
    23.559, 23.614, 23.670, 23.725, 23.780, 23.835, 23.891, 23.946, 24.001, 24.057,
    24.112, 24.167, 24.223, 24.278, 24.333, 24.389, 24.444, 24.499, 24.555, 24.610,
    24.665, 24.721, 24.776, 24.832, 24.887, 24.943, 24.998, 25.053, 25.109, 25.164,
    25.220, 25.275, 25.331, 25.386, 25.442, 25.497, 25.553, 25.608, 25.664, 25.720,
    25.775, 25.831, 25.886, 25.942, 25.998, 26.053, 26.109, 26.165, 26.220, 26.276,
    26.332, 26.387, 26.443, 26.499, 26.555, 26.610, 26.666, 26.722, 26.778, 26.834,
    26.889, 26.945, 27.001, 27.057, 27.113, 27.169, 27.225, 27.281, 27.337, 27.393,
    27.449, 27.505, 27.561, 27.617, 27.673, 27.729, 27.785, 27.841, 27.897, 27.953,
    28.010, 28.066, 28.122, 28.178, 28.234, 28.291, 28.347, 28.403, 28.460, 28.516,
    28.572, 28.629, 28.685, 28.741, 28.798, 28.854, 28.911, 28.967, 29.024, 29.080,
    29.137, 29.194, 29.250, 29.307, 29.363, 29.420, 29.477, 29.534, 29.590, 29.647,
    29.704, 29.761, 29.818, 29.874, 29.931, 29.988, 30.045, 30.102, 30.159, 30.216,
    30.273, 30.330, 30.387, 30.444, 30.502, 30.559, 30.616, 30.673, 30.730, 30.788,
    30.845, 30.902, 30.960, 31.017, 31.074, 31.132, 31.189, 31.247, 31.304, 31.362,
    31.419, 31.477, 31.535, 31.592, 31.650, 31.708, 31.766, 31.823, 31.881, 31.939,
    31.997, 32.055, 32.113, 32.171, 32.229, 32.287, 32.345, 32.403, 32.461, 32.519,
    32.577, 32.636, 32.694, 32.752, 32.810, 32.869, 32.927, 32.985, 33.044, 33.102,
    33.161, 33.219, 33.278, 33.337, 33.395, 33.454, 33.513, 33.571, 33.630, 33.689,
    33.748, 33.807, 33.866, 33.925, 33.984, 34.043, 34.102, 34.161, 34.220, 34.279,
    34.338, 34.397, 34.457, 34.516, 34.575, 34.635, 34.694, 34.754, 34.813, 34.873,
    34.932, 34.992, 35.051, 35.111, 35.171, 35.230, 35.290, 35.350, 35.410, 35.470,
    35.530, 35.590, 35.650, 35.710, 35.770, 35.830, 35.890, 35.950, 36.010, 36.071,
    36.131, 36.191, 36.252, 36.312, 36.373, 36.433, 36.494, 36.554, 36.615, 36.675,
    36.736, 36.797, 36.858, 36.918, 36.979, 37.040, 37.101, 37.162, 37.223, 37.284,
    37.345, 37.406, 37.467, 37.528, 37.590, 37.651, 37.712, 37.773, 37.835, 37.896,
    37.958, 38.019, 38.081, 38.142, 38.204, 38.265, 38.327, 38.389, 38.450, 38.512,
    38.574, 38.636, 38.698, 38.760, 38.822, 38.884, 38.946, 39.008, 39.070, 39.132,
    39.194, 39.256, 39.318, 39.381, 39.443, 39.505, 39.568, 39.630, 39.693, 39.755,
    39.818, 39.880, 39.943, 40.005, 40.068, 40.131, 40.193, 40.256, 40.319, 40.382,
    40.445, 40.508, 40.570, 40.633, 40.696, 40.759, 40.822, 40.886, 40.949, 41.012,
    41.075, 41.138, 41.201, 41.265, 41.328, 41.391, 41.455, 41.518, 41.581, 41.645,
    41.708, 41.772, 41.835, 41.899, 41.962, 42.026, 42.090, 42.153, 42.217, 42.281,
    42.344, 42.408, 42.472, 42.536, 42.599, 42.663, 42.727, 42.791, 42.855, 42.919,
    42.983, 43.047, 43.111, 43.175, 43.239, 43.303, 43.367, 43.431, 43.495, 43.559,
    43.624, 43.688, 43.752, 43.817, 43.881, 43.945, 44.010, 44.074, 44.139, 44.203,
    44.267, 44.332, 44.396, 44.461, 44.525, 44.590, 44.655, 44.719, 44.784, 44.848,
    44.913, 44.977, 45.042, 45.107, 45.171, 45.236, 45.301, 45.365, 45.430, 45.494,
    45.559, 45.624, 45.688, 45.753, 45.818, 45.882, 45.947, 46.011, 46.076, 46.141,
    46.205, 46.270, 46.334, 46.399, 46.464, 46.528, 46.593, 46.657, 46.722, 46.786,
    46.851, 46.915, 46.980, 47.044, 47.109, 47.173, 47.238, 47.302, 47.367, 47.431,
    47.495, 47.560, 47.624, 47.688, 47.753, 47.817, 47.881, 47.946, 48.010, 48.074,
    48.138, 48.202, 48.267, 48.331, 48.395, 48.459, 48.523, 48.587, 48.651, 48.715,
    48.779, 48.843, 48.907, 48.971, 49.034, 49.098, 49.162, 49.226, 49.290, 49.353,
    49.417, 49.481, 49.544, 49.608, 49.672, 49.735, 49.799, 49.862, 49.926, 49.989,
    50.052, 50.116, 50.179, 50.243, 50.306, 50.369, 50.432, 50.495, 50.559, 50.622,
    50.685, 50.748, 50.811, 50.874, 50.937, 51.000, 51.063, 51.126, 51.188, 51.251,
    51.314, 51.377, 51.439, 51.502, 51.565, 51.627, 51.690, 51.752, 51.815, 51.877,
    51.940, 52.002, 52.064, 52.127, 52.189, 52.251, 52.314, 52.376, 52.438, 52.500,
    52.562, 52.624, 52.686, 52.748, 52.810, 52.872, 52.934, 52.996, 53.057, 53.119,
    53.181, 53.243, 53.304, 53.366, 53.427, 53.489, 53.550, 53.612, 53.673, 53.735,
    53.796, 53.857, 53.919, 53.980, 54.041, 54.102, 54.164, 54.225, 54.286, 54.347,
    54.408, 54.469, 54.530, 54.591, 54.652, 54.713, 54.773, 54.834, 54.895, 54.956,
    55.016, 55.077, 55.138, 55.198, 55.259, 55.319, 55.380, 55.440, 55.501, 55.561,
    55.622, 55.682, 55.742, 55.803, 55.863, 55.923, 55.983, 56.043, 56.104, 56.164,
    56.224, 56.284, 56.344, 56.404, 56.464, 56.524, 56.584, 56.643, 56.703, 56.763,
    56.823, 56.883, 56.942, 57.002, 57.062, 57.121, 57.181, 57.240, 57.300, 57.360,
    57.419, 57.479, 57.538, 57.597, 57.657, 57.716, 57.776, 57.835, 57.894, 57.953,
    58.013, 58.072, 58.131, 58.190, 58.249, 58.309, 58.368, 58.427, 58.486, 58.545,
    58.604, 58.663, 58.722, 58.781, 58.840, 58.899, 58.957, 59.016, 59.075, 59.134,
    59.193, 59.252, 59.310, 59.369, 59.428, 59.487, 59.545, 59.604, 59.663, 59.721,
    59.780, 59.838, 59.897, 59.956, 60.014, 60.073, 60.131, 60.190, 60.248, 60.307,
    60.365, 60.423, 60.482, 60.540, 60.599, 60.657, 60.715, 60.774, 60.832, 60.890,
    60.949, 61.007, 61.065, 61.123, 61.182, 61.240, 61.298, 61.356, 61.415, 61.473,
    61.531, 61.589, 61.647, 61.705, 61.763, 61.822, 61.880, 61.938, 61.996, 62.054,
    62.112, 62.170, 62.228, 62.286, 62.344, 62.402, 62.460, 62.518, 62.576, 62.634,
    62.692, 62.750, 62.808, 62.866, 62.924, 62.982, 63.040, 63.098, 63.156, 63.214,
    63.271, 63.329, 63.387, 63.445, 63.503, 63.561, 63.619, 63.677, 63.734, 63.792,
    63.850, 63.908, 63.966, 64.024, 64.081, 64.139, 64.197, 64.255, 64.313, 64.370,
    64.428, 64.486, 64.544, 64.602, 64.659, 64.717, 64.775, 64.833, 64.890, 64.948,
    65.006, 65.064, 65.121, 65.179, 65.237, 65.295, 65.352, 65.410, 65.468, 65.525,
    65.583, 65.641, 65.699, 65.756, 65.814, 65.872, 65.929, 65.987, 66.045, 66.102,
    66.160, 66.218, 66.275, 66.333, 66.391, 66.448, 66.506, 66.564, 66.621, 66.679,
    66.737, 66.794, 66.852, 66.910, 66.967, 67.025, 67.082, 67.140, 67.198, 67.255,
    67.313, 67.370, 67.428, 67.486, 67.543, 67.601, 67.658, 67.716, 67.773, 67.831,
    67.888, 67.946, 68.003, 68.061, 68.119, 68.176, 68.234, 68.291, 68.348, 68.406,
    68.463, 68.521, 68.578, 68.636, 68.693, 68.751, 68.808, 68.865, 68.923, 68.980,
    69.037, 69.095, 69.152, 69.209, 69.267, 69.324, 69.381, 69.439, 69.496, 69.553
};

struct Config {
  float emissivity = 0.18;
  float offset_mV = 0.0;
  float alpha = 0.2;
  uint32_t sampling_interval = 500;
  float slope_correction = 1.00;
  float offset_correction = 0.0;
  float display_max_temp = 1200.0;
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
  if (isnan(config.offset_mV) || fabs(config.offset_mV) > 100.0) valid = false;
  if (isnan(config.alpha) || config.alpha <= 0.0 || config.alpha > 1.0) valid = false;
  if (config.sampling_interval < 100 || config.sampling_interval > 60000) valid = false;
  if (isnan(config.slope_correction) || config.slope_correction < 0.5 || config.slope_correction > 2.0) valid = false;
  if (isnan(config.offset_correction) || fabs(config.offset_correction) > 100.0) valid = false;
  if (isnan(config.display_max_temp) || config.display_max_temp < 10 || config.display_max_temp > 1200.0) valid = false;

  if (!valid) {
    Serial1.println("EEPROM invalida, usando valores por defecto.");
    config = {0.18, 0.0, 0.2, 500, 1.00, 0.0, 1200.0};
  }
}

float interpolateJ(float temp) {
  if (temp <= 0.0) return 0.0;
  if (temp >= 1200.0) return j_temps[1200];
  int i = (int)temp;
  float frac = temp - i;
  return j_temps[i] + (j_temps[i + 1] - j_temps[i]) * frac;
}

void printStatus() {
  Serial1.println("--- CONFIGURACION ACTUAL ---");
  Serial1.print("EMI: "); Serial1.println(config.emissivity, 2);
  Serial1.print("OFFSET: "); Serial1.println(config.offset_mV, 3);
  Serial1.print("ALPHA: "); Serial1.println(config.alpha, 3);
  Serial1.print("SAMPLE_MS: "); Serial1.println(config.sampling_interval);
  Serial1.print("SLOPEC: "); Serial1.println(config.slope_correction, 3);
  Serial1.print("OFFSETC: "); Serial1.println(config.offset_correction, 3);
  Serial1.print("MAXTEMP: "); Serial1.println(config.display_max_temp, 1);
  Serial1.print("TEMP_EMA: "); Serial1.print(emaTemp, 2); Serial1.println(" °C");
  Serial1.println("----------------------------");
}

void parseSerialCommand() {
  if (!Serial1.available()) return;
  String cmd = Serial1.readStringUntil('\n');
  cmd.trim();

  if (cmd.startsWith("EMI ")) config.emissivity = cmd.substring(4).toFloat();
  else if (cmd.startsWith("OFFSET ")) config.offset_mV = cmd.substring(7).toFloat();
  else if (cmd.startsWith("ALPHA ")) config.alpha = cmd.substring(6).toFloat();
  else if (cmd.startsWith("SAMPLE ")) config.sampling_interval = constrain(cmd.substring(7).toInt(), 100, 60000);
  else if (cmd.startsWith("SLOPEC ")) config.slope_correction = cmd.substring(8).toFloat();
  else if (cmd.startsWith("OFFSETC ")) config.offset_correction = cmd.substring(9).toFloat();
  else if (cmd.startsWith("MAXTEMP ")) config.display_max_temp = constrain(cmd.substring(8).toFloat(), 10.0, 1200.0);
  else if (cmd.startsWith("CALIB ")) {
    float realTemp = cmd.substring(6).toFloat();
    if (!isnan(emaTemp)) {
      config.offset_correction = realTemp - emaTemp * config.slope_correction;
      Serial1.print("OFFSETC ajustado a: "); Serial1.println(config.offset_correction, 3);
    } else {
      Serial1.println("ERROR: No hay lectura EMA válida.");
    }
  }
  else if (cmd.startsWith("SIM " )) {
    float inputTemp = cmd.substring(4).toFloat();
    float corrected = inputTemp * config.slope_correction + config.offset_correction;
    float mV = interpolateJ(corrected) + config.offset_mV;
    if (corrected > config.display_max_temp) corrected = config.display_max_temp;
    Serial1.print("Simulado: "); Serial1.print(inputTemp);
    Serial1.print(" °C -> Corr: "); Serial1.print(corrected);
    Serial1.print(" °C -> mV: "); Serial1.print(mV, 3); Serial1.println(" mV");
  }
  else if (cmd == "STATUS") printStatus();
  else if (cmd == "SAVE") saveConfig();
  else if (cmd == "RESET") NVIC_SystemReset();
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
      Serial1.println("ERROR: Lectura MLX. Intentando reconectar...");
      if (!restartMLX()) {
        Serial1.println("Fallo al reconectar. Reiniciando...");
        delay(500);
        NVIC_SystemReset();
      }
      return;
    }

    if (isnan(emaTemp)) emaTemp = tempC;
    else emaTemp = config.alpha * tempC + (1.0 - config.alpha) * emaTemp;

    float corrected = emaTemp * config.slope_correction + config.offset_correction;
    if (corrected > config.display_max_temp) corrected = config.display_max_temp;
    float output_mV = interpolateJ(corrected) + config.offset_mV;
    float V_DAC = (output_mV * resistor_divider) / 1000.0;
    int dac_value = constrain(round((V_DAC / VREF) * dac_max), 0, dac_max);
    dac.setValue(dac_value);
  }
}
