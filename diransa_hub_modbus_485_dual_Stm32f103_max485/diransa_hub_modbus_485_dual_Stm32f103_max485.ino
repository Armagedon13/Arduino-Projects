/*
Hub modbus rtu Armagedon13 2024 - Asynchronous Slave Reading
Improved version following library example
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
HardwareSerial Serial2(PA3, PA2);
#define MODBUS_SERIAL_SLAVE Serial2
const int8_t dePin_slave = PB13;
ModbusRTUSlave modbus_slave(MODBUS_SERIAL_SLAVE, dePin_slave);
#define MODBUS_BAUD_SLAVE 9600
#define MODBUS_CONFIG_SLAVE SERIAL_8N1
#define MODBUS_SLAVE_ID 19  // ID for SCADA communication 

// Modbus Slave Configuration
const uint8_t REGISTERS_PER_SLAVE = 2;  // Number of holding registers to read from each slave
const uint8_t SLAVE_IDS[] = {225, 19, 20, 21, 22, 23, 24, 25, 26, 27, 160, 166, 162, 164, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 159, 165, 161, 163};  
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0])) 
const uint8_t NUM_SLAVES = ARRAY_SIZE(SLAVE_IDS); 

// Holding Registers Buffer
uint16_t holdingRegisters[NUM_SLAVES * REGISTERS_PER_SLAVE];
const uint16_t numHoldingRegisters = NUM_SLAVES * REGISTERS_PER_SLAVE;

// Async State Tracking
enum SlaveReadState {
    IDLE,
    READING_REGISTERS
};

SlaveReadState currentState = IDLE;
uint8_t currentSlaveIndex = 0;

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

void processNextSlave() {
    // If we've read all slaves, reset to the beginning
    if (currentSlaveIndex >= NUM_SLAVES) {
        currentSlaveIndex = 0;
        currentState = IDLE;
        return;
    }

    // Read holding registers for the current slave
    uint8_t error = modbus_master.readHoldingRegisters(
        SLAVE_IDS[currentSlaveIndex], 
        0, 
        holdingRegisters + currentSlaveIndex * REGISTERS_PER_SLAVE, 
        REGISTERS_PER_SLAVE
    );

    // Log the transaction
    printLog(SLAVE_IDS[currentSlaveIndex], 0x03, 0, REGISTERS_PER_SLAVE, error);

    // Move to the next slave
    currentSlaveIndex++;
}

void setup() {
    // Initialize Serial for debugging
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\nModbus RTU Multi-Slave Bridge (Async)");
    Serial.print("Total Configured Slaves: ");
    Serial.println(NUM_SLAVES);
    
    // Initialize Master Serial and Modbus
    MODBUS_SERIAL_MASTER.begin(MODBUS_BAUD_MASTER, MODBUS_CONFIG_MASTER);
    modbus_master.begin(MODBUS_BAUD_MASTER);
    modbus_master.setTimeout(300);

    // Initialize Slave Serial and Modbus
    MODBUS_SERIAL_SLAVE.begin(MODBUS_BAUD_SLAVE, MODBUS_CONFIG_SLAVE);
    modbus_slave.begin(MODBUS_SLAVE_ID, MODBUS_BAUD_SLAVE);
    
    // Initialize holding registers to 0
    memset(holdingRegisters, 0, sizeof(holdingRegisters));

    // Configure holding registers for slave
    modbus_slave.configureHoldingRegisters(holdingRegisters, numHoldingRegisters);
    
    delay(1000);
}

void loop() {
    // State machine for reading slaves
    switch (currentState) {
        case IDLE:
            // Start reading slaves
            currentSlaveIndex = 0;
            currentState = READING_REGISTERS;
            break;
        
        case READING_REGISTERS:
            processNextSlave();
            break;
    }
    
    // Process incoming Modbus Slave requests
    modbus_slave.poll();
    
    // Small delay to prevent overwhelming the bus
    //delay(100);
}