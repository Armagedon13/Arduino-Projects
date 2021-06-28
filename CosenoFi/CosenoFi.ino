#include <math.h>

const byte pin = 2;

float angle;

float angle_max = 0;
bool status, statusAnt = false;
unsigned long start;
volatile unsigned long result;
bool flag = false;

//float positivo;


void setup()
{
 Serial.begin(9600);
 pinMode(pin, INPUT);
 // attachInterrupt(digitalPinToInterrupt(interruptPin), blink, CHANGE);
 
 attachInterrupt(INT0, isr, CHANGE );  // INT0 es pin2 INT1 es pin 3
}

void isr()
{
  status = digitalRead(pin);
  if (status && !statusAnt) { // flanco 0 a 1
     start = micros();        // inicio la cuenta en microsegundos
  }
  if (!status && statusAnt) { // flanco 1 a 0
      result = micros()-start;
      flag = true;
  }
  statusAnt = status;
  
}
void loop()
{
  
   if (flag) {
      detachInterrupt(INT0);
      Serial.println(result);
      flag = false;
      attachInterrupt(INT0, isr, CHANGE );  // INT0 es pin2 INT1 es pin 3
   }
   delay(5000);
}
