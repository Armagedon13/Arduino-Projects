#include <Wire.h>
// Si usas la librería de Rob Tillaart:
#include "MCP4725.h"
// MCP4725.h -> https://github.com/RobTillaart/MCP4725

// Direccion I2C del MCP4725 (típicamente 0x60, pero depende de A0)
MCP4725 dac(0x60);

// Voltaje de referencia en mV (3.3V o 5000.0 si usas 5V)
#define VREF_MV 3300.0

// Offset en mV medido cuando DAC=0 (por defecto 0, se ajusta por comando "offset X")
float offset_mV = 0.0;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Inicializamos el DAC
  // if (!dac.begin()) {
  //   Serial.println("No se encontro el MCP4725 en la direccion 0x60. Verifica cableado!");
  //   while (1);
  // }

  Serial.println("=== Calibracion MCP4725 ===");
  Serial.println("Comandos disponibles:");
  Serial.println("  offset X   -> Ajusta el offset en mV (por ejemplo, offset 23)");
  Serial.println("  set X      -> Ajusta la salida a X mV (por ejemplo, set 1000)");
  Serial.println("  save       -> Guarda el valor DAC actual en la EEPROM del MCP4725");
  Serial.println("  help       -> Muestra este mensaje de ayuda");
  Serial.println();
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    parseCommand(input);
  }
}

// --------------------------------------------------------
// Funcion para parsear los comandos del Monitor Serie
// --------------------------------------------------------
void parseCommand(const String& command) {
  if (command.startsWith("offset")) {
    // Sintaxis: offset X
    // Ejemplo: offset 23  (significa que midio 23mV en DAC=0)
    int spaceIndex = command.indexOf(' ');
    if (spaceIndex > 0) {
      offset_mV = command.substring(spaceIndex + 1).toFloat();
      Serial.print("Offset ajustado a: ");
      Serial.print(offset_mV);
      Serial.println(" mV");
    } else {
      Serial.println("Uso: offset <mV>");
    }
  }
  else if (command.startsWith("set")) {
    // Sintaxis: set X
    // Ejemplo: set 1000  (queremos 1000mV)
    int spaceIndex = command.indexOf(' ');
    if (spaceIndex > 0) {
      float desired_mV = command.substring(spaceIndex + 1).toFloat();
      // Restamos offset y saturamos a 0 si es negativo
      float corrected_mV = desired_mV - offset_mV;
      if (corrected_mV < 0) {
        corrected_mV = 0;
      }
      // Convertir a codigo DAC
      int dacValue = (int)((corrected_mV * 4095.0) / VREF_MV);
      // Aseguramos que quede entre 0 y 4095
      dacValue = constrain(dacValue, 0, 4095);

      // Escribimos al DAC (modo normal, sin EEPROM)
      // Si usas librería RobTillaart:
      dac.setValue(dacValue);

      Serial.print("Deseado: ");
      Serial.print(desired_mV);
      Serial.print(" mV | Offset: ");
      Serial.print(offset_mV);
      Serial.print(" mV => DAC code: ");
      Serial.print(dacValue);
      Serial.print(" => Salida corregida: ~");
      Serial.print(corrected_mV);
      Serial.println(" mV");
    } else {
      Serial.println("Uso: set <mV>");
    }
  }
  else if (command.equalsIgnoreCase("save")) {
    // Guardar valor actual en EEPROM del MCP4725
    // Con la libreria de RobTillaart se hace con writeDAC(value, EEPROM)
    int currentValue = dac.getValue();  // valor actual
    Serial.print("Guardando DAC code ");
    Serial.print(currentValue);
    Serial.println(" en EEPROM del MCP4725...");
    int err = dac.writeDAC(currentValue, true);  // EEPROM = true
    if (err == MCP4725_OK) {
      Serial.println("Guardado correctamente en EEPROM.");
    } else {
      Serial.print("Error al guardar en EEPROM. Codigo: ");
      Serial.println(err);
    }
  }
  else if (command.equalsIgnoreCase("help")) {
    Serial.println("Comandos disponibles:");
    Serial.println("  offset X   -> Ajusta el offset en mV (p.e. offset 23)");
    Serial.println("  set X      -> Ajusta la salida a X mV (p.e. set 1000)");
    Serial.println("  save       -> Guarda el valor DAC actual en la EEPROM del MCP4725");
    Serial.println("  help       -> Muestra este mensaje de ayuda");
  }
  else {
    Serial.println("Comando no reconocido. Escribe 'help' para ayuda.");
  }
}
