/*************************************************** 
 CreatedBY S. Simmons 

  This is a library for the MAX7219 8-digit LED 8-segment dispay controller 

 ****************************************************/

#include <SPI.h>
#include "MAX7219.h"

byte MAX7219::m_B_Decode[16]=
{	//bits 0-6 are Seg A-G, bit 7 IS DP (set elsewhere)
	0x7E,//0
	0x30,//1
	0x6D,//2
	0x79,//3
	0x33,//4
	0x5B,//5
	0x5F,//6
	0x70,//7
	0x7F,//8
	0x7B,//9
	0x01,//-
	0x4F,//E
	0x37,//H
	0x0E,//L
	0x67,//P
	0x00,//BLANK
};


void MAX7219::begin(int CSpin )
{	//WILL USE SPI SUBSYSTEM FOR DATA TRANSFERS (so PINS ARE MOSI, MISO, SCK)
	_pinCS = CSpin;
	_interface = MAX7219_SPI;
	
	digitalWrite(_pinCS, HIGH);
	pinMode(_pinCS, OUTPUT);
	SPI.begin();//SETS UP MOSI, MISO, SCK pins
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);
	initRegisters();
	
}

void MAX7219::begin(int CSpin, int SINpin, int SOUTpin, int SCKpin) 
{	//WILL USE shigtOut() and shiftIN() FOR DATA TRANSFERS
	_pinCS = CSpin;
	_pinSIN = SINpin;
	_pinSOUT = SOUTpin;
	_pinCLK = SCKpin;
	_interface = MAX7219_SHIFT;
	
	digitalWrite(_pinCS, HIGH);
	pinMode(_pinCS, OUTPUT);
	//shiftOut/In WILL NOT WORK WITHOUT SETTING to OUTPUT!!
	pinMode(_pinCS, OUTPUT);
	pinMode(_pinSOUT, OUTPUT);
	pinMode(_pinCLK, OUTPUT);
	initRegisters();
}


void MAX7219::initRegisters()
{
	writeReg(MAX7219_SHUT, 0x01);//ENABLE SYSTEM
	_num_digits = 8;
	writeReg(MAX7219_SCAN, _num_digits-1);//SCAN DIGITS 0-7
	_dmodes = 0xFF;
	writeReg(MAX7219_DMODE, _dmodes);//YES TO B-DECODING ON EACH DIGIT
	writeReg(MAX7219_INTENS, 0x0F);//FULL INTENSITY
}

void MAX7219::clearDisplay()
{
	int k;
	
	for(k=0; k<=7; k++)
		writeDisplayDigitRaw(k, 0x00);//ALL SEGMNETS OFF
}


void MAX7219::displayOff()
{
	writeReg(MAX7219_SHUT, 0x00);//DISABLE SYSTEM SCANNING
	
}

void MAX7219::displayOn()
{
	writeReg(MAX7219_SHUT, 0x01);//ENABLE SYSTEM SCANNING
	
}

void MAX7219::chooseNumDigits(int num_digits)
{
	_num_digits = num_digits;
	if(_num_digits >8)
		_num_digits = 8;
	writeReg(MAX7219_SCAN, _num_digits-1);//SCAN LIMIT = _num_digits -1
}

void MAX7219::printInteger(long number)
{
	int position;
	int dec_digit;
	long div10_value;
	bool blank_digit;
	
	position = 0;//STAR WITH RIGHTMOST DISPLAY 7-SEGMENT DIGIT
	blank_digit = false;
	while(position < _num_digits)
	{	div10_value = number/10;
		dec_digit = number - 10*div10_value;
		if(blank_digit)
			writeDisplayDigitRaw(position++, 0x00);
		else
		{	writeDisplayDigitCoded(position++, dec_digit);
			number = div10_value;
			if(number==0)
				blank_digit = true;
		}
	}
}

void MAX7219::printFloat(float fvalue, int places)
{
	float fractional_part, div10_value;
	long right_of_decimal_point;
	long whole_part, temp_part;
	int position = 0, k, numdigits_wholepart;
	int dec_digit;
	byte segenst_lit_mask;
	bool blank_digit;
	
	whole_part = (long)(fvalue);
	fractional_part = fvalue - whole_part;
	numdigits_wholepart = 0;
	temp_part = whole_part;
	while(temp_part!=0)
	{	temp_part /= 10;
		numdigits_wholepart++;
	}
	if((places + numdigits_wholepart) > 8)
		places = 8-numdigits_wholepart;
	
	for(k=1; k<=places; k++)
		fractional_part =  fractional_part * 10;
	right_of_decimal_point = (long)(fractional_part);
	
	position = 0;//STAR WITH RIGHTMOST DISPLAY 7-SEGMENT DIGIT
	for(k=1; k<= places; k++)
	{	div10_value = right_of_decimal_point/10;
		dec_digit = right_of_decimal_point - 10*div10_value;
		writeDisplayDigitCoded(position++, dec_digit);
		right_of_decimal_point = div10_value;
	}	
	k=0;
	blank_digit = false;
	while(position < _num_digits)
	{	
		div10_value = whole_part/10;
		dec_digit = whole_part - 10*div10_value;
		if(k==0)
		{	segenst_lit_mask = m_B_Decode[dec_digit];
			segenst_lit_mask |= 0x80;//LIGHT DECIMAL POINTTOO ON LOWEST DIGIT OF WHOLE PART
			writeDisplayDigitRaw(position++, segenst_lit_mask);
		}
		else
		{	if(blank_digit)
				writeDisplayDigitRaw(position++, 0x00);
			else
				writeDisplayDigitCoded(position++, dec_digit);
		}
		whole_part = div10_value;
		if(whole_part==0)
			blank_digit = true;
		k++;	
	}
	
}

void MAX7219::writeDisplayDigitCoded(int digit_pos, byte dec_digit)
{
	digit_pos = digit_pos%8;
	_dmodes =  _dmodes | (1<<digit_pos);
	writeReg(MAX7219_DMODE,_dmodes);//YES TO B-DECODING ON THIS DIGIT
	writeReg(MAX7219_DIG0 + digit_pos, dec_digit);
	
}

void MAX7219::writeDisplayDigitRaw(int digit_pos, byte segments_lit)
{
	digit_pos = digit_pos%8;
	_dmodes = _dmodes & (~(1<<digit_pos));
	writeReg(MAX7219_DMODE, _dmodes);//NO TO B-DECODING ON THIS DIGIT
	writeReg(MAX7219_DIG0 + digit_pos, segments_lit);	
}


void MAX7219::writeReg(uint8_t reg_addr, uint8_t data) 
{	//Write a byte into one of the MAX7219 registers
	switch(_interface)	
	{	
		case MAX7219_SPI:			
			digitalWrite(_pinCS, LOW);//SELECT 
			SPI.transfer(reg_addr);
			SPI.transfer(data);
			digitalWrite(_pinCS, HIGH);//DE-SELECT 
			break;
		case MAX7219_SHIFT:
			digitalWrite(_pinCS, LOW);//SELECT 
			shiftOut(_pinSOUT, _pinCLK, MSBFIRST, reg_addr);
			shiftOut(_pinSOUT, _pinCLK, MSBFIRST, data);
			digitalWrite(_pinCS, HIGH);//DE-SELECT 
			break;
	}
	if(reg_addr==MAX7219_DMODE)
		_dmodes = data;
	else if(reg_addr==MAX7219_SCAN)
		_num_digits = data+1;
}
