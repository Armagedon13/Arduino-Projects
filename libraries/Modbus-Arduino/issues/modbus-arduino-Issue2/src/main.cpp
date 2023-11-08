#include <Arduino.h>
#include <ModbusSerial.h>

ModbusSerial mb (Serial, 10);

const uint8_t NbOfCoil = 19;
const uint16_t FirstCoil = 20;
const uint32_t CoilValues = 0x056BCD;

void setup() {
#ifdef ESP32
  Serial.begin (38400, SERIAL_8E1);
#else
  Serial.begin (38400);
#endif
  mb.config (38400);

  for (uint8_t i = 0; i < NbOfCoil; i++) {

    mb.addCoil (FirstCoil -1 + i, (CoilValues & (1 << i)) != 0);
  }
}

void loop() {

  mb.task();
}

