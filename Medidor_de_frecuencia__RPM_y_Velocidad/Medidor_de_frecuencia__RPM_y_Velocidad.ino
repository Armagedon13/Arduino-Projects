#include <Arduino.h>

#include <TVout.h>
#include <video_gen.h>
#include <fontALL.h>
#include <font4x6.h>
#include <font6x8.h>
#include <font8x8.h>
#include <font8x8ext.h>
TVout TV;

volatile int contador = 0;

double rpm = 0;

unsigned long lastmillis = 0;

void setup(){
  TV.begin(PAL,120,96);                 //declarar tipo de video
 
Serial.begin(9600);
 
attachInterrupt(0, rpm_fan, FALLING); // en Arduino UNO la interrupcion 0 es el pin 2
 
}
 
void loop(){
 
if (millis() - lastmillis == 1000){
 
detachInterrupt(0);    //deshabilita le interrupción para realizar los cálculos
 
 
 
rpm = ((float)contador / 0.016666);  // 0.166=1/60 0.016666
 
Serial.print("RPM =\t"); // imprime la palabra RPM y da un tab
Serial.print(rpm); // imprime el valor de rpm
 TV.select_font(font4x6);
 TV.print(0,0,rpm);


 
 
contador = 0; // pone en 0 el contador
 
lastmillis = millis();
 
attachInterrupt(0, rpm_fan, FALLING); //inicia la interrupción
 
}

}
 
void rpm_fan(){ /*este código funcionara cuando el valor de interrupción 0 (pin 2) sea bajo.*/
 
contador ++;
 
}
