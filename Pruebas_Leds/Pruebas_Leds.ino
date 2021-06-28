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
unsigned long herz;
const int cosa = 2;
void setup () 
{ 
Serial.begin(9600); 
TV.begin(PAL,120,96);                 //declarar tipo de video
 TV.select_font(font8x8);              //seleccionar fuente y tamaño 
 TV.clear_screen();
 pinMode(2,INPUT);
} 

void loop () 
{ 
duracion1 = pulseIn (cosa, HIGH,100000000); 
duracion2 = pulseIn (cosa, LOW,100000000); 
Serial.println(); 
Serial.print("periodo 1: "); 
Serial.print(duracion1); 
Serial.print("ms"); 
Serial.println(); 
Serial.print("periodo 2: "); 
Serial.print(duracion2); 
Serial.print("ms"); 
//Serial.println((duracion1+duracion2)/1000);// periodo 
//Serial.println((duracion1)/1000);// periodo 
//Serial.println((duracion2)/1000);// periodo 
Serial.println(); 
Serial.print("frecuencia: "); 
Serial.print(1000000/(duracion1+duracion2)); //frecuencia 
Serial.print("Hz"); 
Serial.println(); 
delay(1000); 
TV.print(0,0,duracion1);
TV.print(0,30,duracion2);
herz = 1000000/(duracion1+duracion2);
TV.print(0,80,herz);
} 
