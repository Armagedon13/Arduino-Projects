#include <Arduino.h>
#include <ModbusSerial.h>

ModbusSerial mb (Serial, 10);

const uint8_t NbOfIsts = 24;
const uint16_t FirstIsts = 197;
const uint32_t IstsValues = 0xB5DBAC;

void setup() {
  
#ifdef ESP32
  Serial.begin (38400, SERIAL_8E1);
#else
  Serial.begin (38400);
#endif
  mb.config (38400);

  for (uint8_t i = 0; i < NbOfIsts; i++) {

    mb.addIsts (FirstIsts -1 + i, (IstsValues & (1 << i)) != 0);
  }
}

void loop() {

  mb.task();
}

