/*
  Modbus-Arduino Example - Lamp (Modbus Serial)
  Copyright by André Sarmento Barbosa
  http://github.com/andresarmento/modbus-arduino
*/
 
#include <Modbus.h>
#include <ModbusSerial.h>

// Modbus Registers Offsets (0-9999)
const int LAMP1_COIL = 100; 
// Used Pins
const int ledPin = 13;

// ModbusSerial object
ModbusSerial mb;

void setup() {
    // Config Modbus Serial (port, speed, byte format) 
    mb.config(&Serial, 38400, MB_PARITY_EVEN);
    // Set the Slave ID (1-247)
    mb.setSlaveId(10);
    // mb.setAdditionalServerData ("LAMP"); // for Report Server ID function
    
    // Set ledPin mode
    pinMode(ledPin, OUTPUT);
    // Add LAMP1_COIL register - Use addCoil() for digital outputs
    mb.addCoil(LAMP1_COIL);
}

void loop() {
   // Call once inside loop() - all magic here
   mb.task();
   
   // Attach ledPin to LAMP1_COIL register     
   digitalWrite(ledPin, mb.Coil(LAMP1_COIL));
}
