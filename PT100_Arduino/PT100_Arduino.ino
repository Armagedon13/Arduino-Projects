//librerias
#include <TVout.h>
#include <video_gen.h>
#include <font4x6.h>
#include <font6x8.h>
#include <font8x8.h>
#include <font8x8ext.h>
#include <fontALL.h>
#include <ResponsiveAnalogRead.h>
#include <RBD_Timer.h>

TVout TV;
RBD::Timer timer;

//decimal a entero
int i1;
float f1;

int i2;
float f2;

int i3;
float f3;

int i4;
float f4;

int i5;
float f5;

int i6;
float f6;

//pines
const int sensorPin= A0;
const int sensorPin1= A1;
const int sensorPin2= A2;
const int sensorPin3= A3;
const int sensorPin4= A4;
const int sensorPin5= A5;

//smooth
 ResponsiveAnalogRead analogOne(A0, false);
 
void setup()
{
  Serial.begin(9600);
  TV.begin(NTSC,120,96);
  
  //Tiempo Reset
  timer.setTimeout(1000);               //setea el tiempo de reset de variables
  timer.restart();
}
 
void loop()
{
  analogOne.update();
  TV.select_font(font4x6);
  TV.println(0,0,"Temperatura1");
  TV.println(0,25,"Temperatura2");
  TV.println(0,50,"Temperatura3");
  TV.println(70,0,"Temperatura4");
  TV.println(70,25,"Temperatura5");
  TV.println(70,50,"Temperatura6");

TV.select_font(font8x8ext);
//////////////////////////////////////////////////////////
  //int value = analogRead(sensorPin);
  int value = analogOne.getValue();
  float millivolts = (value / 1023.0) * 5000;
  float celsius = millivolts / 10; 
  
  Serial.print(celsius);
  Serial.println(" C");
  
  f1 = celsius; 
  i1 = (int) f1; 
   TV.println(0,10,i1);
  
/////////////////////////////////////////////////////////
  int value1 = analogRead(sensorPin1);
  float millivolts1 = (value1 / 1023.0) * 5000;
  float celsius1 = millivolts1 / 10; 
  Serial.print(celsius1);
  Serial.println(" C1");

  f2 = celsius1; 
  i2 = (int) f2; 
  
  TV.println(0,35,i2);

  
///////////////////////////////////////////////////////
    int value2 = analogRead(sensorPin2);
  float millivolts2 = (value2 / 1023.0) * 5000;
  float celsius2 = millivolts2 / 10; 
  Serial.print(celsius2);
  Serial.println(" C2");
  
  f3 = celsius2; 
  i3 = (int) f3; 

  TV.println(0,60,i3);

  
/////////////////////////////////////////////////
    int value3 = analogRead(sensorPin3);
  float millivolts3 = (value3 / 1023.0) * 5000;
  float celsius3 = millivolts3 / 10; 
  Serial.print(celsius3);
  Serial.println(" C3");

  f4 = celsius3; 
  i4 = (int) f4; 
    
  TV.println(70,10,i4);
  
//////////////////////////////////////////////////////
    int value4 = analogRead(sensorPin4);
  float millivolts4 = (value4 / 1023.0) * 5000;
  float celsius4 = millivolts4 / 10; 
  Serial.print(celsius4);
  Serial.println(" C4");

  f5 = celsius4; 
  i5 = (int) f5;
    
   TV.println(70,35,i5);
  
//////////////////////////////////////////////////////
    int value5 = analogRead(sensorPin5);
  float millivolts5 = (value5 / 1023.0) * 5000;
  float celsius5 = millivolts5 / 10; 
  Serial.print(celsius5);
  Serial.println(" C5");

  f6 = celsius5; 
  i6 = (int) f6;
    
  TV.println(70,60,i6);
/////////////////////////////////////////////////////

//Refrescar pantalla
 if(timer.onRestart()){
   TV.clear_screen();
   }
  
}
