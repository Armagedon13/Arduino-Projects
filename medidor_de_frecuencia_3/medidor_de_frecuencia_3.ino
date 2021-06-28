#include <Arduino.h>

#include <FreqCounter.h>
#include <TVout.h>
#include <video_gen.h>
#include <fontALL.h>
#include <font4x6.h>
#include <font6x8.h>
#include <font8x8.h>
#include <font8x8ext.h>
TVout TV;

void setup() {
  Serial.begin(57600);                    // connect to the serial port
  Serial.println("Frequency Counter");
    TV.begin(PAL,120,96); 
}

long int frq;

void loop() {

 FreqCounter::f_comp= 12;             // Set compensation to 12
 FreqCounter::start(100);            // Start counting with gatetime of 100ms
 while (FreqCounter::f_ready == 0)         // wait until counter ready
 
 frq=FreqCounter::f_freq;            // read result
 Serial.println(frq);                // print result
 TV.select_font(font4x6);
    TV.print(0,0,frq);
 delay(20);
}
