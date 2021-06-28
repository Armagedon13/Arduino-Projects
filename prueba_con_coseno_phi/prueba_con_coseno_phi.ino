#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
#include <EmonLib.h>            


EnergyMonitor emon1;

const int contactor = 6;
long int contador;
long int contador2;
int Contrast=50;
float powerFactor;
float apparentPower;
float realPower;
float Vrms;
float Irms;

void setup() 
{
  Serial.begin(9600);
  lcd.begin(16, 2);
  emon1.voltage(2, 233.0, 8.1); // Voltage: input pin, calibration, phase_shift   emon1.voltage(2, 105.0, 1.7);
  emon1.current(1, 60.6);      // Current: input pin, calibration.     emon1.current(0, 320.0);
  pinMode(contactor, OUTPUT);
  digitalWrite(contactor, LOW);
  analogWrite(10,Contrast);
}

void loop() 
{
emon1.calcVI(20,2000);                    // Calculate all. No.of wavelengths, time-out
//emon1.serialprint();
    
realPower       = emon1.realPower;        //extract Real Power into variable
apparentPower   = emon1.apparentPower;    //extract Apparent Power into variable
powerFactor     = emon1.powerFactor;      //extract Power Factor into Variable
Vrms            = emon1.Vrms;             //extract Vrms into Variable
Irms            = emon1.Irms;             //extract Irms into Variable


  if(powerFactor<=0.85){                           //si el factor de potencia es menor a 0.85 se activa el contactor 
    contador++;
    contador2=0;
    Serial.println(contador);
    if(contador==3000){
    digitalWrite(contactor,HIGH);
    contador=0;
    }}
  if(powerFactor>=0.85){                           //si el factor de potencia es mayor a 0.85 se apaga el contactor
    contador2++;
    contador=0;
    Serial.println(contador2);
    if(contador2==12000){
    contador2=0;
    digitalWrite(contactor,LOW);
    }}

    
  if(powerFactor>=1.0){
    powerFactor=1.0;
    }
  if(powerFactor<=0.0){
    powerFactor=0.0;
    }
    
//escribe datos en pantalla
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("PF=");
   lcd.setCursor(4,0);
   lcd.print(powerFactor);
   lcd.print(" ");
   lcd.setCursor(0,1);
   lcd.print("Tension=");
   lcd.setCursor(10,1);
   lcd.print(Vrms);
   lcd.print(" ");
    
}
