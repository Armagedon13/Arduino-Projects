#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include "MCP4725.h"

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

double emissivity = 0.18;

void setup() {
  Serial.begin(9600);
  mlx.begin();

  Serial.println("Ingrese el valor de emisividad (entre 0 y 1) y presione Enter:");
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    double newEmissivity = input.toFloat();
    if (newEmissivity > 0 && newEmissivity <= 1) {
      emissivity = newEmissivity;
      mlx.writeEmissivity(emissivity);
      Serial.print("Nueva emisividad establecida: ");
      Serial.println(emissivity);
    } else {
      Serial.println("Valor inválido. Intente de nuevo (entre 0 y 1).");
    }
  }

  Serial.print("Ambiente = ");
  Serial.print(mlx.readAmbientTempC());
  Serial.print("ºC\tObjeto = ");
  Serial.print(mlx.readObjectTempC());
  Serial.println("ºC");
  
  delay(500);
}
