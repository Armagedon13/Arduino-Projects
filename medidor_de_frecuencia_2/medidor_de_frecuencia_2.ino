#include <Arduino.h>

#include <TVout.h>
#include <video_gen.h>
#include <fontALL.h>
#include <font4x6.h>
#include <font6x8.h>
#include <font8x8.h>
#include <font8x8ext.h>
TVout TV;


volatile unsigned long pulseCount = 0UL;
unsigned long startT, endT, freq, pC;

void counter()
{ 
  pulseCount++;
}

void setup()
{
  Serial.begin(38400);
  Serial.println( "Frequency counter" );
  attachInterrupt(0, counter, RISING);
  TV.begin(PAL,120,96); 
  
}

void loop()
{
  noInterrupts();
  pC = pulseCount;
  interrupts();
  
  if ( pC == 1UL )
  {
    startT = micros(); // start time very close to on a pulse edge
    // getting end time should have the same lag within a few cycles
  }

  // if pulses are 5/second, 10 seconds will take 50 pulses --- check the 5/sec!
  if ( pC >= 51UL ) // 50 pulses -after- pulse 1
  {
    endT = micros();

    endT -= startT; // end now holds elapsed micros

    // frequency = pulses/second, we have pulses and microseconds, 1000000 usecs/sec
    // Hz = pulses x 1000000 / microseconds   

    freq = pC * 1000000UL / endT;

    Serial.print( "\n count " );
    Serial.println( pC );
    Serial.print( " micros " );
    Serial.println( endT );
    Serial.print( " freq " );
    Serial.println(freq);
    TV.select_font(font4x6);
    TV.print(0,0,freq);
    delay(5000);
    noInterrupts();
    pulseCount = 0UL;
    interrupts();
  }
}
