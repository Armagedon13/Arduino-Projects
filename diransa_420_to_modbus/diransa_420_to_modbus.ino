/*
  Modbus-Arduino Example - TempSensor (Modbus Serial)
  Copyright by André Sarmento Barbosa
  http://github.com/andresarmento/modbus-arduino
*/
 
#include <Modbus.h>
#include <ModbusSerial.h>


//Modbus Registers Offsets (0-9999)
const int SENSOR_IREG = 0; 
//Used Pins
const int sensorPin = A0;
const int Pin1 = 15;
const int Pin2 = 16;
const int Pin3 = 17;
const int Pin4 = 18;
const int Pin5 = 19;

// ModbusSerial object
ModbusSerial mb;

long ts;
int Pot;
int PWM;

void setup() {
  //set pins to low
  pinMode(Pin1, OUTPUT);
  pinMode(Pin2, OUTPUT);
  pinMode(Pin3, OUTPUT);
  pinMode(Pin4, OUTPUT);
  pinMode(Pin5, OUTPUT);
  digitalWrite(Pin1, LOW);
  digitalWrite(Pin2, LOW);
  digitalWrite(Pin3, LOW);
  digitalWrite(Pin4, LOW);
  digitalWrite(Pin5, LOW);
  // Config Modbus Serial (port, speed, byte format) 
  mb.config(&Serial, 9600, SERIAL_8E1, 13); // mb.config(&Serial, 9600, SERIAL_8N1, 13);
  // Set the Slave ID (1-247)
  mb.setSlaveId(35);
  // Add SENSOR_IREG register - Use addIreg() for analog Inputs
  mb.addIreg(SENSOR_IREG);
  // 
  ts = millis();
}

void loop() {
   //Call once inside loop() - all magic here
  mb.task();

  Pot = analogRead(sensorPin);

  /*
  value: the number to map.
  fromLow: the lower bound of the value’s current range.
  fromHigh: the upper bound of the value’s current range.
  toLow: the lower bound of the value’s target range.
  toHigh: the upper bound of the value’s target range.
  */
  // map(value, fromLow, fromHigh, toLow, toHigh)

  PWM = map (Pot, 214, 1050, 0, 67); //ESCALAR LOS VALORES DEL POTENCIÓMETRO


  //Read each two seconds
  if (millis() > ts + 2000) {
      ts = millis();
      //Setting raw value (0-1024)
      mb.Ireg(SENSOR_IREG, PWM);
  } 
}
