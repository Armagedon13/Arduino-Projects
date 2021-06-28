//
// File			DWEasyFlipFlop.h
//
// Class library header
//
// Details		DWEasyFlipFlop library helps to manage two action in defined time structure without using "delay"
//				You can 2 callBack function with desired interval e.g. function1 (flip) lit the led with a function during 5secondes
//				Then function2 (flop) turn off the led and lit another led or do something else.
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
#ifndef DWEASYFLIPFLOP_H_
#define DWEASYFLIPFLOP_H_
#include "Arduino.h"
#ifdef __cplusplus
extern "C" {
#endif
//-----------------------------------------------
	typedef void (*callbackFunction)(void);
//-----------------------------------------------
#ifdef __cplusplus
} // extern "C"
#endif
class DWEasyFlipFlop
{
	public:
		DWEasyFlipFlop();
		virtual ~DWEasyFlipFlop();

		void loop();
		void setFlipDelay	(unsigned long delay);
		void setFlopDelay	(unsigned long delay);
		unsigned long getFlipDelay	();
		unsigned long getFlopDelay	();
		void setFlipHandler	( callbackFunction theFunction);
		void setFlopHandler	( callbackFunction theFunction);
		void setFlipEnable(bool isEnable);
		void setFlopEnable(bool isEnable);
		void stop();
		void start();

	private:
		bool			_running;
		bool			_currStatus;
		unsigned long 	_lastMillis;
		unsigned long  	_delay;
		unsigned long  	_flipDelay;
		unsigned long  	_flopDelay;
		bool 			_flipEnable;
		bool 			_flopEnable;
		void (*flipHandler)(void);
		void (*flopHandler)(void);
};

#endif /* DWEASYFLIPFLOP_H_ */
