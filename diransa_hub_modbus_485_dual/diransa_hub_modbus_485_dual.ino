#include <ModbusRtu.h>
#include <HardwareSerial.h>

//HardwareSerial Serial1(PA10, PA9);
HardwareSerial Serial2(PB11, PB10);

// Data array to store temperatures from all Novus devices
uint16_t novusData[32];  // Array to store temperatures from 32 devices
uint16_t scadaData[32];  // Array to share with SCADA

// Master state machine variables
uint8_t masterState;
unsigned long waitTime;

// Create Modbus master instance on Serial1 (PA9/PA10) for Novus devices
// Using pin PB12 for DE/RE control
Modbus master(0, Serial1, PB12);  // ID=0 for master

// Create Modbus slave instance on Serial3 (PB10/PB11) for SCADA
// Using pin PB13 for DE/RE control
Modbus slave(1, Serial2, PB13);   // ID=1 for slave, using Serial3 for PB10/PB11

// Telegram structure for master queries
modbus_t telegram;

void setup() {
  // Initialize both serial ports
  Serial1.begin(9600, SERIAL_8N1);  // Baud rate for Novus devices - PA9 (TX) and PA10 (RX)
  Serial2.begin(9600, SERIAL_8N1);  // Baud rate for SCADA - PB10 (TX) and PB11 (RX)
  
  // Start both Modbus instances
  master.start(); 
  slave.start();
  
  // Set timeout for master queries
  master.setTimeOut(100);
  
  // Initialize state machine
  masterState = 0;
  waitTime = millis() + 1000;
  
  // Initialize data arrays
  for(uint8_t i = 0; i < 32; i++) {
    novusData[i] = 0;
    scadaData[i] = 0;
  }
}

uint8_t currentDevice = 0;  // Keep track of which Novus device we're polling

void loop() {
  // Handle master state machine to read Novus devices
  switch(masterState) {
    case 0: // Wait state
      if(millis() > waitTime) {
        masterState++;
      }
      break;
      
    case 1: // Send query to current Novus device
      telegram.u8id = currentDevice + 1;  // Novus addresses start from 1
      telegram.u8fct = 3;                 // Function 3: Read holding registers
      telegram.u16RegAdd = 0;             // Register address for temperature
      telegram.u16CoilsNo = 1;            // Read 1 register (temperature)
      telegram.au16reg = &novusData[currentDevice];  // Store in array
      
      master.query(telegram);
      masterState++;
      break;
      
    case 2: // Wait for response
      master.poll();
      if(master.getState() == COM_IDLE) {
        // Copy temperature to SCADA data array
        scadaData[currentDevice] = novusData[currentDevice];
        
        // Move to next device
        currentDevice = (currentDevice + 1) % 32;
        
        // Reset state machine
        masterState = 0;
        waitTime = millis() + 50;  // 50ms delay between queries
      }
      break;
  }
  
  // Handle slave communications with SCADA
  // Make all temperatures available in registers 0-31
  slave.poll(scadaData, 32);
}