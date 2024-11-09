/*
Hub modbus rtu Armagedon13 2024
This program is made with the purpouse to read all slaves and convert it to tags with 1 address id
it read all slaves in the line and send it to the master node

Slaves ------------ Master Module ------------ Stm32f103 ------------ Slave Module ------------ Scada

!WARNING to make it work you need to apart the slaves to the main modbus line so it generate a sub Red.

this program uses CMB27 modbus rtu and follow the same license:
https://github.com/CMB27/ModbusRTUMaster
https://github.com/CMB27/ModbusRTUSlave

MIT License

Copyright (c) 2022 Christopher Bulliner

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <HardwareSerial.h>
#include <ModbusRTUMaster.h>
#include <ModbusRTUSlave.h>

// Master Configuration
#define MODBUS_SERIAL_MASTER Serial1
const int8_t dePin_Master = PB12;
ModbusRTUMaster modbus_master(MODBUS_SERIAL_MASTER, dePin_Master);
#define MODBUS_BAUD_MASTER 9600
#define MODBUS_CONFIG_MASTER SERIAL_8N1

// Slave Configuration
HardwareSerial Serial3(PB11, PB10);
#define MODBUS_SERIAL_SLAVE Serial3
const int8_t dePin_slave = PB13;
const int8_t rePin_slave = PB14;
ModbusRTUSlave modbus_slave(MODBUS_SERIAL_SLAVE);
#define MODBUS_BAUD_SLAVE 9600
#define MODBUS_CONFIG_SLAVE SERIAL_8N1
#define MODBUS_SLAVE_ID 1  // ID for SCADA communication 

// Modbus Slave Configuration
const uint8_t NUM_SLAVES = 3;  // Number of Modbus slaves to read from, get the size of list later
const uint8_t REGISTERS_PER_SLAVE = 2;  // Number of holding registers to read from each slave
const uint8_t SLAVE_IDS[] = {2, 3, 4};  // Slave IDs to read from

// Holding Registers Buffer
uint16_t holdingRegisters[NUM_SLAVES * REGISTERS_PER_SLAVE];
const uint16_t numHoldingRegisters = NUM_SLAVES * REGISTERS_PER_SLAVE;

// Error Handling
unsigned long transactionCounter = 0;
unsigned long errorCounter = 0;
const char* errorStrings[] = {
    "success",
    "invalid id",
    "invalid buffer",
    "invalid quantity",
    "response timeout",
    "frame error",
    "crc error",
    "unknown comm error",
    "unexpected id",
    "exception response",
    "unexpected function code",
    "unexpected response length",
    "unexpected byte count",
    "unexpected address",
    "unexpected value",
    "unexpected quantity"
};

// Function to log Modbus transactions
void printLog(uint8_t unitId, uint8_t functionCode, uint16_t startingAddress, uint16_t quantity, uint8_t error) {
    transactionCounter++;
    if (error) errorCounter++;
    
    char string[128];
    sprintf(string, "%ld %ld %02X %02X %04X %04X %s", 
            transactionCounter, errorCounter, unitId, functionCode, 
            startingAddress, quantity, errorStrings[error]);
    Serial.print(string);
    
    if (error == MODBUS_RTU_MASTER_EXCEPTION_RESPONSE) {
        sprintf(string, ": %02X", modbus_master.getExceptionResponse());
        Serial.print(string);
    }
    Serial.println();
}

// Read holding registers from all configured slaves
bool readAllSlaveRegisters() {
    uint8_t error;
    uint16_t tempBuffer[REGISTERS_PER_SLAVE];
    
    for (uint8_t i = 0; i < NUM_SLAVES; i++) {
        // Read holding registers from each slave
        error = modbus_master.readHoldingRegisters(
            SLAVE_IDS[i],        // Slave ID
            0,                   // Start address
            tempBuffer,          // Buffer to store registers
            REGISTERS_PER_SLAVE  // Number of registers to read
        );
        
        // Check for errors and log them
        if (error != 0) {
            printLog(SLAVE_IDS[i], 0x03, 0, REGISTERS_PER_SLAVE, error);
            return false;
        }
        
        // Copy registers to consolidated buffer
        memcpy(
            &holdingRegisters[i * REGISTERS_PER_SLAVE], 
            tempBuffer, 
            REGISTERS_PER_SLAVE * sizeof(uint16_t)
        );

        // Log successful read
        printLog(SLAVE_IDS[i], 0x03, 0, REGISTERS_PER_SLAVE, 0);
    }
    
    return true;
}

void setup() {
    // Initialize Serial for debugging
    Serial.begin(115200);
    while(!Serial);
    Serial.println("\n\nModbus RTU Multi-Slave Bridge");
    
    // Initialize Master Serial and Modbus
    MODBUS_SERIAL_MASTER.begin(MODBUS_BAUD_MASTER, MODBUS_CONFIG_MASTER);
    modbus_master.begin(MODBUS_BAUD_MASTER);
    //modbus_master.setTimeout(300);
    
    // Initialize Slave Serial and Modbus
    MODBUS_SERIAL_SLAVE.begin(MODBUS_BAUD_SLAVE, MODBUS_CONFIG_SLAVE);
    modbus_slave.begin(MODBUS_SLAVE_ID, MODBUS_BAUD_SLAVE);
    
    // Initialize holding registers to 0
    memset(holdingRegisters, 0, sizeof(holdingRegisters));

    // Configure holding registers for slave
    modbus_slave.configureHoldingRegisters(holdingRegisters, numHoldingRegisters);
}

void loop() {
    // Read holding registers from all slaves
    readAllSlaveRegisters();
    
    // Process incoming Modbus Slave requests
    modbus_slave.poll();
    
    // Small delay to prevent overwhelming the bus
    delay(100);
}