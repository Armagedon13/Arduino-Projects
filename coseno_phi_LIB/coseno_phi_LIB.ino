#include "EmonLib.h"             // Include Emon Library
EnergyMonitor emon1;             // Create an instance
float PF;

void setup()
{
  Serial.begin(9600);

  emon1.voltage(1, 230.0, 1.7);  // Voltage: input pin, calibration, phase_shift
  emon1.current(0, 320.0);       // Current: input pin, calibration.

}
void loop()
{
  emon1.calcVI(20,2000);         // Calculate all. No.of wavelengths, time-out
  emon1.serialprint();           // Print out all variables
  
 PF=emon1.powerFactor;
}
