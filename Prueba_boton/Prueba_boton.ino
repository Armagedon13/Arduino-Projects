#include <Arduino.h>

const int pinLed =13; //EL PIN DEL LED
const int pinBtn =7;

int encender =0;
int anterior = 0;
int estado = 0;

void setup ()
{
  pinMode(pinLed, OUTPUT);
  pinMode(pinBtn, INPUT);
}

void loop()
{
  estado = digitalRead(pinBtn); // Comprobamos el estado
  if(estado && anterior ==0) //Comprobar el pulsador
  {
    encender = 1 - encender;// asignamos el estado contrario
    delay(30);
  }
  anterior = estado;
  if(encender)
  digitalWrite(pinLed, HIGH);
  else
  digitalWrite(pinLed, LOW);
}
