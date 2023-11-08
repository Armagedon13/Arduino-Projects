/**
  @file TempSensor.ino
  Modbus-Arduino Example - TempSensor (Modbus Serial)
    Copyright (C) 2023 Pascal JEAN aka epsilonrt
    Copyright (C) 2014 André Sarmento Barbosa
    https://github.com/epsilonrt/modbus-serial
*/

#include <ModbusSerial.h>

// Used Pins

const byte SlaveId = 77;
const int SensorPin = A1;
const int TxenPin = 2; // -1 disables the feature, change that if you are using an RS485 driver, this pin would be connected to the DE and /RE pins of the driver.
// Modbus Registers Offsets (0-9999)
const int SensorIreg = 0;
const int SensorIreg1 = 1;

#define MySerial Serial // define serial port used, Serial most of the time, or Serial1, Serial2 ... if available
const unsigned long Baudrate = 9600;

// ModbusSerial object
ModbusSerial mb (MySerial, SlaveId, TxenPin);

long ts;

void setup() {
  MySerial.begin (Baudrate, MB_PARITY_NONE); // works on all boards but the configuration is 8N1 which is incompatible with the MODBUS standard
  // prefer the line below instead if possible
  // MySerial.begin (Baudrate, MB_PARITY_EVEN);
  while (! MySerial)
    ;

  mb.config (Baudrate);
  mb.setAdditionalServerData ("TEMP_SENSOR"); // for Report Server ID function (0x11)


  // Add SensorIreg register - Use addIreg() for analog Inputs
  mb.addIreg (SensorIreg);
  mb.addIreg (SensorIreg1);
  
  pinMode(SensorPin, INPUT);

  ts = millis();
}

void loop() {
  
  // Call once inside loop() - all magic here
  mb.task();
  
  int PWM = map(analogRead(SensorPin), 207, 1050, 0, 2000); //ESCALAR LOS VALORES DEL POTENCIÓMETRO

  // Read each two seconds
  if (millis() > ts + 1000) {
    
    ts = millis();
    // Setting raw value (0-1024)
    mb.Ireg (SensorIreg, analogRead(SensorPin));
    mb.Ireg (SensorIreg1, PWM);
    
  }
}