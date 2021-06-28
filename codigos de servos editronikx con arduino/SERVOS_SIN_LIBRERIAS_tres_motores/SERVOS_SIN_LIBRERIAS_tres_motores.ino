int PIN_SERVO1=10;
int GRA1_SER1=0;
int GRA2_SER1=180;
/*///////////////////////////
int PIN_SERVO2=11;
int GRA1_SER2=100;
int GRA2_SER2=140;
//////////////////////////
int PIN_SERVO3=12;
int GRA1_SER3=120;
int GRA2_SER3=100;*/
float pausa;

void setup() {

}

void loop() {
  Servo1 ();
 /* Servo2 ();
  Servo3 ();*/
  
}
void moverServo1 (int pin, int angulo)
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
 ///////////////////////////////////////
/* void moverServo2 (int pin, int angulo)
{
   pinMode(pin,OUTPUT);
   pausa = angulo*2000.0/180.0 + 500;
   digitalWrite(pin, HIGH);
   delayMicroseconds(pausa);
   digitalWrite(pin, LOW);
   delayMicroseconds(25000-pausa);
   /////////////////
    lcd.setCursor(0,0);
    lcd.print("servo 2 ");
    lcd.print(GRA1_SER2);
    lcd.print("-");
    lcd.print(GRA2_SER2);
 }
 ///////////////////////
 ///////////////////////////////////////
 void moverServo3 (int pin, int angulo)
{
   pinMode(pin,OUTPUT);
   pausa = angulo*2000.0/180.0 + 500;
   digitalWrite(pin, HIGH);
   delayMicroseconds(pausa);
   digitalWrite(pin, LOW);
   delayMicroseconds(25000-pausa);
   /////////////////
    lcd.setCursor(0,0);
    lcd.print("servo 3 ");
    lcd.print(GRA1_SER3);
    lcd.print("-");
    lcd.print(GRA2_SER3);
 }*/
 ///////////////////////
 ////funiones de repetir
 void Servo1 ()
 {
    moverServo1(PIN_SERVO1, GRA1_SER1);
    delay(1000);

    moverServo1(PIN_SERVO1, GRA2_SER1);
    delay(1000);

 }
 //////////////////
 /*void Servo2 ()
 {
   for(int i=0;i<30;i++)
    {
    moverServo2(PIN_SERVO2, GRA1_SER2);
    }
    delay(1000);
    lcd.clear();
    for(int i=0;i<30;i++)
    {
    moverServo2(PIN_SERVO2, GRA2_SER2);
    }
    delay(1000);
    lcd.clear();
 }
 //////////////////
 void Servo3 ()
 {
   for(int i=0;i<30;i++)
    {
    moverServo3(PIN_SERVO3, GRA1_SER3);
    }
    delay(1000);
    lcd.clear();
    for(int i=0;i<30;i++)
    {
    moverServo3(PIN_SERVO3, GRA2_SER3);
    }
    delay(1000);
    lcd.clear();
 }*/
