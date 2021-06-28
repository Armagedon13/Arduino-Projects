#include <Arduino.h>

// Author: Silverjoda
// 4/8/2015
// Target: Arduino
 
// Code uses external interrupt to count the incoming pulses.
#include <TVout.h>
#include <video_gen.h>
#include <fontALL.h>
#include <font4x6.h>
#include <font6x8.h>
#include <font8x8.h>
#include <font8x8ext.h>
TVout TV;
 
volatile int counter = 0;
 
void setup() {
  TV.begin(PAL,120,96); 
  Serial.begin(9600);
  attachInterrupt(0,count,RISING);
}
 
void loop() {
  delay(999); // Delay almost 1 second.  
  Serial.print(counter*60); // Counter/2 * 60 seconds.
  Serial.println("rpm.");
  counter = 0;
   TV.select_font(font8x8);
  TV.print(0,0,counter);
}
 
void count()
{
 counter++;
}
