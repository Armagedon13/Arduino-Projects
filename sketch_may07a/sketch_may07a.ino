// EmonLibrary examples openenergymonitor.org, Licence GNU GPL V3

#include "EmonLib.h" // Include Emon Library
EnergyMonitor emon1; // Create an instance

void setup()
{
Serial.begin(9600);

emon1.voltage(1, 105.0, 1.7); // Voltage: input pin, calibration, phase_shift
emon1.current(0, 66.6); // Current: input pin, calibration.
}

void loop()
{
emon1.calcVI(20,2000); // Calculate all. No.of half wavelengths (crossings), time-out
// emon1.serialprint(); // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)
//
//extract Real Power into variable
float apparentPower = emon1.apparentPower; //extract Apparent Power into variable
float powerFActor = emon1.powerFactor; //extract Power Factor into Variable
float supplyVoltage = emon1.Vrms*0.981702307; //extract Vrms into Variable
float Irms = (emon1.Irms-.2)*.59;
float realPower = supplyVoltage*Irms*powerFActor; //extract Irms into Variable
Serial.print (" AC ");
Serial.print (powerFActor);
Serial.print (" ");
//Serial.print (apparentPower);
//Serial.print (" W ");
Serial.print (realPower);
Serial.print (" W ");

Serial.print (supplyVoltage);
Serial.print (" V ");
Serial.print (Irms);
Serial.print (" A ");
Serial.println();
delay(1000);
}
