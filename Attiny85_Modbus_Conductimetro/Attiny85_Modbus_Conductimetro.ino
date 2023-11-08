#include "SoftwareSerial.h"
#include "SimpleModbusSlaveSoftwareSerial.h"

#define analogIn0  5  // Analog Read

//////////////// registers of your slave ///////////////////
enum {
  // just add or remove registers and your good to go...
  // The first register starts at address 0
  ANALOG0,
  TOTAL_ERRORS,
  // leave this one
  TOTAL_REGS_SIZE
  // total number of registers for function 3 and 16 share the same register array
};

unsigned int holdingRegs[TOTAL_REGS_SIZE];  // function 3 and 16 register array

////////////////////////////////////////////////////////////

#define RX 0            // Arduino defined pin (PB0, package pin #5)
#define TX 1            // Arduino defined pin (PB1, package pin #6)
#define RS485_EN 2      // pin to set transmission mode on RS485 chip (PB2, package pin #7)
#define BAUD_RATE 9600  // baud rate for serial communication
#define deviceID 77     // this device address

SoftwareSerial mySerial(RX, TX);

void setup() {
  /* parameters(
                  SoftwareSerial* comPort
                  long baudrate,
                  unsigned char ID,
                  unsigned char transmit enable pin,
                  unsigned int holding registers size)
       The transmit enable pin is used in half duplex communication to activate a MAX485 or similar
       to deactivate this mode use any value < 2 because 0 & 1 is reserved for Rx & Tx
    */

  modbus_configure(&mySerial, BAUD_RATE, deviceID, RS485_EN, TOTAL_REGS_SIZE);

  pinMode(analogIn0, INPUT);
}

void loop() {
  // modbus_update() is the only method used in loop(). It returns the total error
  // count since the slave started. You don't have to use it but it's useful
  // for fault finding by the modbus master.

  int PWM = map (analogIn0, 214, 1050, 0, 2000); //ESCALAR LOS VALORES DEL POTENCIÓMETRO

  holdingRegs[TOTAL_ERRORS] = modbus_update(holdingRegs);
  holdingRegs[ANALOG0] = PWM;
  delayMicroseconds(50);
  modbus_update(holdingRegs);
}