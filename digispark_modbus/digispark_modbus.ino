#include <ModbusRtu.h>
#define ldrpin 3
#define TXEN  2
#include <SoftwareSerialParity.h>

int ldrvalue = 0;

// data array for modbus network sharing
uint16_t au16data[16] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


//adres van de slave instellen
SoftwareSerialParity mySerial(0, 1); // RX, TX
Modbus slave(35, mySerial, TXEN);

void setup() {
  mySerial.begin(9600,EVEN);
  pinMode(ldrpin,INPUT);
  slave.start();
}

void loop() {
  slave.poll( au16data, 16 );
    ldrvalue = analogRead(ldrpin);
    au16data[1] = ldrvalue;
  }