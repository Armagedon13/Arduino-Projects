#include <Arduino.h>

#include <stdio.h>
#include <math.h>

int analogPin=0;   // Which pin will be the input of the Voltage-Divider

float Vin=5.0;     // [V]        Supply voltage in the Voltage-Divider
float Raux=10000;  // [ohm]      Secondary resistor in the Voltage-Divider
float R0=10000;    // [ohm]      NTC nominal value at 25ºC
float T0=293.15;   // [K] (25ºC)

float Vout=0.0;    // [V]        Voltage given by the Voltage-Divider
float Rout=0.0;    // [ohm]      Current NTC resistance

float T1=273;      // [K]        Temperature at first testing point
float T2=373;      // [K]        Temperature at second testing point
float RT1=19750;   // [ohms]     Resistance at 273K (0ºC)
float RT2=2150;    // [ohms]     Resistance at 373K (100ºC)

float beta=0.0;    // [K]        Beta parameter
float Rinf=0.0;    // [ohm]      Rinf parameter
float TempK=0.0;   // [K]        Temperature output in Kelvin
float TempC=0.0;   // [ºC]       Temperature output in Celsius

void setup() {
  //Serial port configuration
  Serial.begin(9600);

  //Input pin configuration
  pinMode(analogPin, INPUT);

  //Global parameters calculation
  beta=(log(RT1/RT2))/((1/T1)-(1/T2));
  Rinf=R0*exp(-beta/T0);
}

void loop()
{

  //Current NTC resistance value calculation (through Vout)
  Vout=Vin*((float)(analogRead(analogPin))/1024.0);
  Rout=(Raux*Vout/(Vin-Vout));

  //Temperature calculation
  TempK=(beta/log(Rout/Rinf));
  TempC=TempK-273.15;

  //Data population
  Serial.print(TempC);
  Serial.print(" ºC (");
  Serial.print(TempK);
  Serial.println(" K)");

  delay(1000);
}
