//
// File			DWEasyFlipFlop.cpp
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
#include "DWEasyFlipFlop.h"
#define STATUS_FLIP 1
#define STATUS_FLOP 0


DWEasyFlipFlop::DWEasyFlipFlop()
{
	_currStatus	=	STATUS_FLOP;
	_flipDelay	=	200;
	_flopDelay	=	600;
	_lastMillis	=	millis();
	_running	=	false;
	_flipEnable	=	true;
	_flopEnable	=	true;

	(void*)flipHandler;
	(void*)flopHandler;

}
DWEasyFlipFlop::~DWEasyFlipFlop()
{
	// TODO Auto-generated destructor stub
}
void DWEasyFlipFlop::start()
{
	_currStatus	=	STATUS_FLOP;
	_lastMillis	=	millis();
	_running	=	true;
}
void DWEasyFlipFlop::stop()
{
	_running	=	false;
}
void DWEasyFlipFlop::loop()
{
    if ( _running && (millis() > _lastMillis + _delay) )
    {
        _lastMillis =   millis();

        if (_currStatus==STATUS_FLIP)
        {
			if(flopHandler && _flopEnable)flopHandler();
			_currStatus	=	STATUS_FLOP;
			_delay		=	_flopDelay;
        }
        else
        {
			if(flipHandler && _flipEnable)flipHandler();
			_currStatus	=	STATUS_FLIP;
			_delay		=	_flipDelay;
        }
    }
}
void DWEasyFlipFlop::setFlipEnable(bool isEnable)
{
    _flipEnable      =   isEnable;
}
void DWEasyFlipFlop::setFlopEnable(bool isEnable)
{
    _flopEnable      =   isEnable;
}
void DWEasyFlipFlop::setFlipDelay(unsigned long delay)
{
    _flipDelay      =   delay;
}
unsigned long DWEasyFlipFlop::getFlipDelay()
{
	return _flipDelay;
}
void DWEasyFlipFlop::setFlopDelay(unsigned long delay)
{
    _flopDelay      =   delay;
}
unsigned long DWEasyFlipFlop::getFlopDelay()
{
	return _flopDelay;
}
//set function pointers
void DWEasyFlipFlop::setFlipHandler( callbackFunction theFunction)
{
    flipHandler   =   theFunction;
}
void DWEasyFlipFlop::setFlopHandler( callbackFunction theFunction)
{
    flopHandler   =   theFunction;
}


