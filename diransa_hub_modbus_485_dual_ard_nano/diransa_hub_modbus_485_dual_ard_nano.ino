#include <RS485.h>    // https://github.com/RobTillaart/RS485
#include <MultiModbus.h>
#include <SoftwareSerial.h>

// Define your hardware pins and settings
#define NOVUS_BAUD_RATE 19200
#define SCADA_BAUD_RATE 9600
#define MODBUS_DEVICE_ID 1

// Pins for the two MAX485 modules
const uint8_t novus_tx_pin = 2;  // TX pin for the Novus devices
const uint8_t novus_rx_pin = 3;  // RX pin for the Novus devices
const uint8_t novus_en_pin = 5;  // Enable pin for the Novus MAX485

const uint8_t scada_tx_pin = 12; // TX pin for the SCADA system
const uint8_t scada_rx_pin = 13; // RX pin for the SCADA system
const uint8_t scada_en_pin = 11; // Enable pin for the SCADA MAX485

// List of Novus device IDs
const uint8_t novus_device_ids[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32};
const int novus_device_count = sizeof(novus_device_ids) / sizeof(novus_device_ids[0]);

SoftwareSerial novus_serial(novus_rx_pin, novus_tx_pin);
SoftwareSerial scada_serial(scada_rx_pin, scada_tx_pin);

//RS485 rs485(&sw_serial, sendPin);

RS485 modbus_novus(&novus_serial, novus_en_pin);
RS485 modbus_scada(&scada_serial, scada_en_pin);

// Array to store temperature data from Novus devices
int16_t novus_temperatures[novus_device_count];

void setup() {
  Serial.begin(115200);

  // Initialize the Modbus servers and configure registers
  if (!modbus_novus.begin(NOVUS_BAUD_RATE)) {
    Serial.println("Failed to start Modbus RTU Server (Novus)!");
    while (1);
  }

  if (!modbus_scada.begin(SCADA_BAUD_RATE)) {
    Serial.println("Failed to start Modbus RTU Client (SCADA)!");
    while (1);
  }

  // Configure input registers to hold temperature data from Novus devices
  modbus_novus.configureInputRegisters(0, novus_device_count);

  Serial.println("Modbus hub is ready!");
}

void loop() {
  // Poll the Novus Modbus server for temperature updates
  for (int i = 0; i < novus_device_count; i++) {
    modbus_novus.inputRegisterRead(novus_device_ids[i], 0, &novus_temperatures[i]);
  }

  // Update the SCADA system with the latest temperature data
  modbus_scada.inputRegisterWrite(MODBUS_DEVICE_ID, 0, novus_temperatures, novus_device_count);

  // Process any incoming Modbus requests
  modbus_novus.poll();
  modbus_scada.poll();

  // Add any other necessary logic, such as handling communication errors, etc.
}