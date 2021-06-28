#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
#include <EmonLib.h>            


EnergyMonitor emon1;

int Contrast=70;
float PF;
float supplyVoltage;



void setup() 
{
  Serial.begin(9600);
  lcd.begin(16, 2);
  emon1.voltage(1, 231.0, 1.7); // Voltage: input pin, calibration, phase_shift   emon1.voltage(2, 105.0, 1.7);
  emon1.current(0, 111.1);      // Current: input pin, calibration.     emon1.current(0, 320.0);
  analogWrite(10,Contrast);
}

void loop() 
{
emon1.calcVI(20,2000);                    // Calculate all. No.of wavelengths, time-out
emon1.serialprint();

PF              = emon1.powerFactor;      //extract Power Factor into Variable
supplyVoltage   = emon1.Vrms;             //extract Vrms into Variable

//escribe datos en pantalla
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("PF=");
   lcd.setCursor(4,0);
   lcd.print(PF);
   lcd.print(" ");
   lcd.setCursor(0,1);
   lcd.print("Tension=");
   lcd.setCursor(10,1);
   lcd.print(supplyVoltage);
   lcd.print(" ");
    
}
