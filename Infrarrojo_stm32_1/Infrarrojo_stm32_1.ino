#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <MCP4725.h>

#define DAC_ADDR 0x60  // Dirección I2C del MCP4725

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
MCP4725 dac(DAC_ADDR);

// Definir el divisor resistivo (100k y 1k)
const float voltage_division_factor = 101.0;
const float v_ref = 3.3; // Voltaje de referencia del DAC
const int dac_max = 4095; // 12 bits -> 0-4095

// Conversión de temperatura a milivoltios para termocupla J
float tempToMillivolts(float temp) {
    if (temp < 30.0) temp = 30.0;  // Ignorar valores inferiores a 30°C
    
    // Aproximación lineal basada en la tabla de termopares tipo J
    return 0.05 * temp - 1.88; // Coeficientes ajustados para el rango de interés
}

void setup() {
    Serial.begin(115200);
    Wire.begin();
    // if (!mlx.begin()) {
    //     Serial.println("Error al iniciar MLX90614");
    //     while (1);
    // }
    dac.begin();
}

void loop() {
    float tempC = mlx.readObjectTempC();
    float millivolts = tempToMillivolts(tempC);
    
    // Convertir milivoltios a valor del DAC antes del divisor resistivo
    float dac_output = (millivolts * voltage_division_factor) / v_ref;
    int dac_value = constrain(round(dac_output * dac_max / v_ref), 0, dac_max);
    
    dac.setValue(dac_value);
    
    Serial.print("Temp: "); Serial.print(tempC);
    Serial.print(" °C -> Output: "); Serial.print(millivolts);
    Serial.print(" mV -> DAC: "); Serial.println(dac_value);
    Serial.print("MLX °C: "); Serial.println(mlx.readObjectTempC());
    
    delay(500);
}
