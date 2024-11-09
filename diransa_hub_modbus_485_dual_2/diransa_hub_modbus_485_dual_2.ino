#include <ModbusRTU.h>
#include <HardwareSerial.h>

// Define the Modbus RTU master and slave serial ports
//HardwareSerial Master(PA9, PA10); // TX, RX for Master
HardwareSerial Slave(PB6, PB7);  // TX, RX for Slave

ModbusRTU mb_master;
ModbusRTU mb_slave;

// Modbus registers for temperature data
uint16_t temperatures[32];

bool cbWrite(Modbus::ResultCode event, uint16_t transactionId, void* data) {
  // Callback function for Modbus write requests
  Serial.printf("Request result: 0x%02X\n", event);
  return true;
}

void setup() {
  // Initialize serial ports
  Serial1.begin(9600, SERIAL_8N1);
  Slave.begin(9600, SERIAL_8N1);

  // Initialize Modbus RTU master and slave
  mb_master.begin(&Serial1);
  mb_slave.begin(&Slave);
  mb_master.master();
  mb_slave.slave(1); // Slave ID 1

  // Add holding registers for temperature data
  for (int i = 0; i < 32; i++) {
    mb_slave.addHreg(i, temperatures[i]);
  }
}

void loop() {
  // Poll temperature sensors on Modbus RTU master side
  for (int i = 0; i < 32; i++) {
    mb_master.readHreg(i + 1, 0, &temperatures[i], 1, cbWrite);
  }

  // Update holding registers on Modbus RTU slave side
  mb_slave.task();

  delay(100);
}