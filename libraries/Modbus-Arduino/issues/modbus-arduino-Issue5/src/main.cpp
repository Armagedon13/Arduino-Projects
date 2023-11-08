#include <ModbusSerial.h>

// Used Pins
const byte LedPin = LED_BUILTIN;
const byte TxenPin = -1;    // -1 disables the feature, change that if you are using an RS485 driver, this pin would be connected to the DE and /RE pins of the driver.

#define MySerial Serial1  // define serial port used, Serial most of the time, or Serial1, Serial2 ... if available
const unsigned long Baudrate = 38400;

const byte SlaveId = 10;

const byte FirstReg = 0;
const byte Lamp1Coil = FirstReg;
const byte NofRegs = 32;

ModbusSerial mb(MySerial, SlaveId, TxenPin);

void setup() {

  // MySerial.begin(Baudrate);  // works on all boards but the configuration is 8N1 which is incompatible with the MODBUS standard
  // prefer the line below instead if possible
  MySerial.begin (Baudrate, MB_PARITY_EVEN);

  mb.config(Baudrate);

  for (byte i = 0; i < NofRegs; i++) {

    mb.addHreg(FirstReg + i, i);
    mb.addIreg(FirstReg + i, i);
    mb.addCoil(FirstReg + i, i % 2);
    mb.addIsts(FirstReg + i, (i + 1) % 2);
  }

  pinMode(LedPin, OUTPUT);
}

void loop() {

  mb.task();
  digitalWrite(LedPin, !mb.Coil(Lamp1Coil));
}
