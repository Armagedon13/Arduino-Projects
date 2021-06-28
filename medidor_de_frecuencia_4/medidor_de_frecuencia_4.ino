#include <Arduino.h>

/*
 * Optical Tachometer
 *
 * Uses an IR LED and IR phototransistor to implement an optical tachometer.
 * The IR LED is connected to pin 13 and ran continually.
 * Pin 2 (interrupt 0) is connected across the IR detector.
 *
 * Code based on: www.instructables.com/id/Arduino-Based-Optical-Tachometer/
 * Coded by: arduinoprojects101.com
 */
             // IR LED connected to digital pin 13
volatile byte rpmcount;
unsigned int rpm;
unsigned long timeold;

// include the library code:

// initialize the library with the numbers of the interface pins


void rpm_fun()
 {
   //Each rotation, this interrupt function is run twice, so take that into consideration for 
   //calculating RPM
   //Update count
      rpmcount++;
 }

void setup()
 {
   

   //Interrupt 0 is digital pin 2, so that is where the IR detector is connected
   //Triggers on FALLING (change from HIGH to LOW)
   attachInterrupt(0, rpm_fun, FALLING);

   //Turn on IR LED
Serial.begin(9600);
   rpmcount = 0;
   rpm = 0;
   timeold = 0;
 }

 void loop()
 {
   //Update RPM every second
   delay(1000);
   //Don't process interrupts during calculations
   detachInterrupt(0);
   //Note that this would be 60*1000/(millis() - timeold)*rpmcount if the interrupt
   //happened once per revolution instead of twice. Other multiples could be used
   //for multi-bladed propellers or fans
   rpm = 60*1000/(millis() - timeold)*rpmcount;
   timeold = millis();
   rpmcount = 0;

   //Print out result to lcd
   Serial.println(rpm);

   //Restart the interrupt processing
   attachInterrupt(0, rpm_fun, FALLING);
  }
