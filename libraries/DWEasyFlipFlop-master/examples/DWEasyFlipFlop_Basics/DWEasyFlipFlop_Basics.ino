//
// File			DWEasyFlipFlop_Basics.ino
//
//
// Details		DWEasyFlipFlop library helps to manage two action in defined time structure without using "delay"
//			You can 2 callBack function with desired interval e.g. function1 (flip) lit the led with a function during 5secondes
//			Then function2 (flop) turn off the led and lit another led or do something else.
//                      Flip is first function will be called at startup
//                      Flop is seconde function will be called after flip delay. 
//                      And everything will continue in infinity loop.
//
// Project	 	DejaWorks Easy FlipFlop
// Developed with Eclipse Arduino IDE
//
// Author		Trevor D. BEYDAG
// 				dejaWorks
//
// Date			06/03/15 17:10
// Version		0.2
//
// Copyright	Trevor D. BEYDAG 2015
// License	    MIT
//
//
// Include DejaWorks Easy Flip Flop library
#include <DWEasyFlipFlop.h>

// Create an instance of DW Easy FlipFlop
DWEasyFlipFlop myFlipFlop;

void setup()
{
// Configure Pin 6,7 for output. (if your leds on different ports you can modify this)
	pinMode(PIN6,OUTPUT);
	pinMode(PIN7,OUTPUT);

// Set Flip cycle time in millisecondes 
	myFlipFlop.setFlipDelay(200);
// Set Flip call back function (the function will be called after desigred ms delay)
	myFlipFlop.setFlipHandler(flip);

// Set Flop cycle time in millisecondes 
	myFlipFlop.setFlopDelay(800);
// Set Flop call back function (the function will be called after desigred ms delay)
	myFlipFlop.setFlopHandler(flop);
// Start flip flop	
	myFlipFlop.start();	

}
void loop()
{
// FlipFlop Engine should be called in loop 
	myFlipFlop.loop();
}
void flip()
{
// Any action desigred should be put in this function  
// In this examle led green turn on and red turn off
	digitalWrite(PIN6,HIGH);
	digitalWrite(PIN7,LOW);
}
void flop()
{
// Any action desigred should be put in this function  
// In this examle led green turn off and red turn on
	digitalWrite(PIN6,LOW);
	digitalWrite(PIN7,HIGH);
}
