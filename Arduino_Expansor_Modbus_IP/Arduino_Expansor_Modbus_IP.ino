/*
  Modbus-Arduino Example - Lamp (Modbus IP)
  Copyright by André Sarmento Barbosa
  http://github.com/andresarmento/modbus-arduino
*/
 
#include <SPI.h>
#include <Ethernet.h>
#include <Modbus.h>
#include <ModbusIP.h>

int Input1;
int Input2;
int Output1;
int Output2;

//Modbus Registers Offsets (0-9999)
const int DIGITAL1_COIL = 50;
const int DIGITAL2_COIL = 60;
const int DIGITAL3_COIL = 70;
const int DIGITAL4_HREG = 80;
const int DIGITAL5_HREG = 90;
const int DIGITAL6_COIL = 100;
const int DIGITAL7_COIL = 110;
const int DIGITAL8_COIL = 120;
const int DIGITAL9_COIL = 130;
const int DIGITAL10_COIL = 140;
const int DIGITAL11_COIL = 150;
const int DIGITAL12_COIL = 160;
const int DIGITAL13_COIL = 170;
const int DIGITAL14_COIL = 180;


//Used Pins
const int digitalPin1 = 2;
const int digitalPin2 = 3;
const int digitalPin3 = 4;
const int digitalPin4 = 5; //PWM
const int digitalPin5 = 6; //PWM
const int digitalPin6 = 7;
const int digitalPin7 = 8;
const int digitalPin8 = 9;
const int digitalPin9 = A0;
const int digitalPin10 = A1;
const int digitalPin11 = A2;
const int digitalPin12 = A3;
const int digitalPin13 = A4;
const int digitalPin14 = A5;

//ModbusIP object
ModbusIP mb;

void setup() {
    // The media access control (ethernet hardware) address for the shield
    byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
    // The IP address for the shield
    byte ip[] = { 192, 168, 0, 120 };
    //Config Modbus IP
    mb.config(mac, ip);
    //Set Pin mode
    pinMode(digitalPin1, OUTPUT);
    pinMode(digitalPin2, OUTPUT);
    pinMode(digitalPin3, OUTPUT);
    pinMode(digitalPin4, OUTPUT);
    pinMode(digitalPin5, OUTPUT);
    pinMode(digitalPin6, OUTPUT);
    pinMode(digitalPin7, OUTPUT);
    pinMode(digitalPin8, OUTPUT);
    pinMode(digitalPin9, OUTPUT);
    pinMode(digitalPin10, OUTPUT);
    pinMode(digitalPin11, OUTPUT);
    pinMode(digitalPin12, OUTPUT);
    pinMode(digitalPin13, OUTPUT);
    pinMode(digitalPin14, OUTPUT);
    
    // Add LAMP1_COIL register - Use addCoil() for digital outputs
    mb.addCoil(DIGITAL1_COIL);
    mb.addCoil(DIGITAL2_COIL);
    mb.addCoil(DIGITAL3_COIL);
    mb.addHreg(DIGITAL4_HREG);
    mb.addHreg(DIGITAL5_HREG);
    mb.addCoil(DIGITAL6_COIL);
    mb.addCoil(DIGITAL7_COIL);
    mb.addCoil(DIGITAL8_COIL);
    mb.addCoil(DIGITAL9_COIL);
    mb.addCoil(DIGITAL10_COIL);
    mb.addCoil(DIGITAL11_COIL);
    mb.addCoil(DIGITAL12_COIL);
    mb.addCoil(DIGITAL13_COIL);
    mb.addCoil(DIGITAL14_COIL);
    
}

void loop() {
   //Call once inside loop() - all magic here
   mb.task();
   Input1= mb.Hreg(DIGITAL4_HREG);
   Output1=map(Input1)
   //Attach Pin to Output_COIL or IREG register
   digitalWrite(digitalPin1, mb.Coil(DIGITAL1_COIL));
   digitalWrite(digitalPin2, mb.Coil(DIGITAL2_COIL));
   digitalWrite(digitalPin3, mb.Coil(DIGITAL3_COIL));
   analogWrite(digitalPin4, mb.Hreg(DIGITAL4_HREG));
   analogWrite(digitalPin5, mb.Hreg(DIGITAL5_HREG));
   digitalWrite(digitalPin6, mb.Coil(DIGITAL6_COIL));
   digitalWrite(digitalPin7, mb.Coil(DIGITAL7_COIL));
   digitalWrite(digitalPin8, mb.Coil(DIGITAL8_COIL));
   digitalWrite(digitalPin9, mb.Coil(DIGITAL9_COIL));
   digitalWrite(digitalPin10, mb.Coil(DIGITAL10_COIL));
   digitalWrite(digitalPin11, mb.Coil(DIGITAL11_COIL));
   digitalWrite(digitalPin12, mb.Coil(DIGITAL12_COIL));
   digitalWrite(digitalPin13, mb.Coil(DIGITAL13_COIL));
   digitalWrite(digitalPin14, mb.Coil(DIGITAL14_COIL));
   
}
