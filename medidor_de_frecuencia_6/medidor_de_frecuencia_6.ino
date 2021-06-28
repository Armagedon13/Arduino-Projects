#include <Arduino.h>

#include <TVout.h>
#include <video_gen.h>
#include <fontALL.h>
#include <font4x6.h>
#include <font6x8.h>
#include <font8x8.h>
#include <font8x8ext.h>
TVout TV;

int input=2;

int high_time;
int low_time;
float time_period;
float frequency;

void setup()
{
   TV.begin(PAL,120,96);
pinMode(input,INPUT);
}
void loop()
{



high_time=pulseIn(input,HIGH);
low_time=pulseIn(input,LOW);

 
time_period=high_time+low_time;
time_period=time_period/1000;
frequency=1000/time_period;
TV.select_font(font4x6);
TV.print(0,0,frequency);
}
