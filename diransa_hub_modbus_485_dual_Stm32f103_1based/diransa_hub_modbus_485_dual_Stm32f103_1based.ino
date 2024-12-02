/*
Hub modbus rtu Armagedon13 2024
This program is made with the purpose to read all slaves and convert it to tags with 1 address id
it read all slaves in the line and send it to the master node

Slaves ------------ Master Module ------------ Stm32f103 ------------ Slave Module ------------ Scada

!WARNING to make it work you need to apart the slaves to the main modbus line so it generate a sub Red.

this program uses CMB27 modbus rtu and follow the same license:
https://github.com/CMB27/ModbusRTUMaster
https://github.com/CMB27/ModbusRTUSlave
*/

#include <HardwareSerial.h>
#include <ModbusRTUMaster.h>
#include <ModbusRTUSlave.h>

// Master Configuration
#define MODBUS_SERIAL_MASTER Serial1
ModbusRTUMaster modbus_master(MODBUS_SERIAL_MASTER);
#define MODBUS_BAUD_MASTER 9600
#define MODBUS_CONFIG_MASTER SERIAL_8N1

// Slave Configuration
HardwareSerial Serial2(PA3, PA2);
#define MODBUS_SERIAL_SLAVE Serial2
ModbusRTUSlave modbus_slave(MODBUS_SERIAL_SLAVE);
#define MODBUS_BAUD_SLAVE 9600
#define MODBUS_CONFIG_SLAVE SERIAL_8N1
#define MODBUS_SLAVE_ID 19  // ID for SCADA communication 

// Modbus Slave Configuration
const uint8_t REGISTERS_PER_SLAVE = 2;  // Number of holding registers to read from each slave
const uint8_t SLAVE_IDS[] = {225, 19, 20, 21, 22, 23, 24, 25, 26, 27, 160, 166, 162, 164, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 159, 165, 161, 163};  // Slave IDs to read from
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0])) // Gets the size of the list
const uint8_t NUM_SLAVES = ARRAY_SIZE(SLAVE_IDS); // Total of id's

// Holding Registers Buffer (0-based for internal storage)
uint16_t holdingRegisters[NUM_SLAVES * REGISTERS_PER_SLAVE];
const uint16_t numInternalRegisters = NUM_SLAVES * REGISTERS_PER_SLAVE;

// 1-Based Output Registers Buffer for SCADA (56 tags)
uint16_t scadaRegisters[NUM_SLAVES * REGISTERS_PER_SLAVE * 2 + 1] = {0};
const uint16_t numScadaRegisters = NUM_SLAVES * REGISTERS_PER_SLAVE * 2 + 1;

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

// Convert 0-based internal registers to 1-based SCADA registers
void mapRegistersToScada() {
    // Clear previous SCADA registers
    memset(scadaRegisters, 0, sizeof(scadaRegisters));
    
    // Map 0-based input registers to 1-based SCADA registers
    for (uint8_t i = 0; i < NUM_SLAVES; i++) {
        for (uint8_t j = 0; j < REGISTERS_PER_SLAVE; j++) {
            // Calculate 1-based indices
            uint16_t internalIndex = i * REGISTERS_PER_SLAVE + j;
            uint16_t scadaIndex1 = i * (REGISTERS_PER_SLAVE * 2) + j * 2 + 1;
            uint16_t scadaIndex2 = i * (REGISTERS_PER_SLAVE * 2) + j * 2 + 2;
            
            // Store the original 16-bit register value in two consecutive 1-based registers
            scadaRegisters[scadaIndex1] = (holdingRegisters[internalIndex] >> 8) & 0xFF;    // High byte
            scadaRegisters[scadaIndex2] = holdingRegisters[internalIndex] & 0xFF;          // Low byte
        }
    }
}

bool readAllSlaveRegisters() {
    bool allSuccessful = true;
    
    for (uint8_t i = 0; i < NUM_SLAVES; i++) {
        uint8_t error = modbus_master.readHoldingRegisters(SLAVE_IDS[i], 0, holdingRegisters + i*REGISTERS_PER_SLAVE, REGISTERS_PER_SLAVE);
        if (error != 0) {
            printLog(SLAVE_IDS[i], 0x03, 0, REGISTERS_PER_SLAVE, error);
            allSuccessful = false;
        } else {
            printLog(SLAVE_IDS[i], 0x03, 0, REGISTERS_PER_SLAVE, 0);
        }
    }
    
    // Convert internal registers to 1-based SCADA registers
    mapRegistersToScada();
    
    return allSuccessful;
}

void setup() {
    // Initialize Serial for debugging
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\nModbus RTU Multi-Slave Bridge (1-Based SCADA Output)");