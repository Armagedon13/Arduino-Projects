#include <ModbusSlave.h>

// Explicitly set a stream to use the Serial port.
Modbus slave(Serial, 1, 8); // stream = Serial, slave id = 1, rs485 control-pin = 8

void setup() {
    // Register functions to call when a certain function code is received.
    // If there is no handler assigned to the function code a valid but empty message will be replied.
    slave.cbVector[CB_WRITE_COILS] = readAnalogIn;

    // Start the slave at a baudrate of 9600bps on the Serial port.
    Serial.begin(9600);
    slave.begin(9600);
}

void loop() {
    // Listen for modbus requests on the serial port.
    // When a request is received it's going to get validated.
    // And if there is a function registered to the received function code, this function will be executed.
    slave.poll();
}

// Handle Read Input Registers (FC=04).
uint8_t readAnalogIn(uint8_t fc, uint16_t address, uint16_t length) {
    // Write the result of analogRead() into the response buffer.
    for (int i = 0; i < length; i++) {
      slave.writeRegisterToBuffer(i, analogRead(address + i));
    }
    return STATUS_OK;
}