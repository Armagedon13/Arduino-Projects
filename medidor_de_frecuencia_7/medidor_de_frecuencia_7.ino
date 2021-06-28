#include <Arduino.h>

#include <TVout.h>
#include <video_gen.h>
#include <fontALL.h>
#include <font4x6.h>
#include <font6x8.h>
#include <font8x8.h>
#include <font8x8ext.h>
TVout TV;

unsigned long duracion1; 
unsigned long duracion2; 
int rpm;
int frecuencia;
void setup () 
{ 
Serial.begin(9600);
TV.begin(PAL,120,96);  
} 

void loop () 
{ 
duracion1 = pulseIn (2, HIGH,100000000); 
duracion2 = pulseIn (2, LOW,100000000); 
frecuencia = (1000000/(duracion1+duracion2));
rpm = frecuencia * 60;
//Serial.println((duracion1+duracion2)/1000);// periodo 
//Serial.println((duracion1)/1000);// periodo 
//Serial.println((duracion2)/1000);// periodo 
//Serial.print(1000000/(duracion1+duracion2)); //frecuencia 
Serial.println(); 
Serial.print("frecuencia: "); 
Serial.print(1000000/(duracion1+duracion2)); //frecuencia 
Serial.print("Hz"); 
Serial.println(); 
TV.select_font(font4x6);
TV.print(0,0,rpm);
delay(1000); 
} 

