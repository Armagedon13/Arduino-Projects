#include <Arduino.h>

#include <Time.h>
#include <TimeLib.h>

int pinElectrovalvula1=7;
int estadoEV1;
time_t  T0, T1 ;
 
 
void setup()
{
  Serial.begin(9600);
  setTime(19,56,00,6,28,2017); //aqui me arroja el error:
  // Inicializamos el modulo de EVs
  pinMode(pinElectrovalvula1,OUTPUT);
  estadoEV1=1; //cerrada
  digitalWrite(pinElectrovalvula1,estadoEV1);
}
