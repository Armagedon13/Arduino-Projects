#include <Arduino.h>

#include <RBD_Timer.h>
#include <font4x6.h>
#include <font6x8.h>
#include <font8x8.h>
#include <font8x8ext.h>
#include <fontALL.h>
#include <TVout.h>
#include <video_gen.h>
RBD::Timer timer;
TVout TV;
// read RPM and calculate average every then readings.
const int numreadings = 10;
int readings[numreadings];
unsigned long average = 0;
int index = 0;
unsigned long total; 

volatile int rpmcount = 0;//see http://arduino.cc/en/Reference/Volatile 
unsigned long rpm = 0;
unsigned long lastmillis = 0;

void setup(){
  timer.setTimeout(5000);
  timer.restart();
  TV.begin(PAL,120,96);
  TV.select_font(font8x8);             //seleccionar fuente y tamaño
 TV.print(9,44,"Service Naval");     // comando printin pone lo escrito arriba sin modificar
 TV.delay(2500);
 TV.clear_screen();
 Serial.begin(9600); 
 attachInterrupt(0, rpm_fan, FALLING);
 Serial.begin(9600);
}

void loop(){
 
  
 if (millis() - lastmillis >= 100){  /*Uptade every one second, this will be equal to reading frecuency (Hz).*/
 
 detachInterrupt(0);    //Disable interrupt when calculating
 total = 0;  
 readings[index] = rpmcount * 1;  /* Convert frecuency to RPM, note: this works for one interruption per full rotation. For two interrups per full rotation use rpmcount * 30.*/
 
 for (int x=0; x<=9; x++){
   total = total + readings[x];
 }
 
 average = total / numreadings;
 rpm = average;
 
 rpmcount = 0; // Restart the RPM counter
 index++;
 if(index >= numreadings){
  index=0; 
 } 
 
 
if (millis() > 100){  // wait for RPMs average to get stable

 Serial.print(" RPM = ");
 Serial.println(rpm);
}
 
 lastmillis = millis(); // Uptade lasmillis
  attachInterrupt(0, rpm_fan, FALLING); //enable interrupt
  }
  if(timer.onRestart()){
  TV.clear_screen();
  }
  TV.print( 9, 44, rpm);
}



void rpm_fan(){ /* this code will be executed every time the interrupt 0 (pin2) gets low.*/
  rpmcount++;
}
