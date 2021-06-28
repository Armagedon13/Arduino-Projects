

float pausa;

void setup() {
  
}

void loop() {
  
  moverServo(10, 0);
  delay(1000);

  moverServo(10, 180);
  delay(1000);

 
  
}
void moverServo (int pin, int angulo)///moverServo(10, 180);
{
  for(int i=0;i<30;i++)
  {
   pinMode(pin,OUTPUT);
   pausa = angulo*2000.0/180.0 + 500;
   digitalWrite(pin, HIGH);
   delayMicroseconds(pausa);
   digitalWrite(pin, LOW);
   delayMicroseconds(25000-pausa);
  }
  
}
