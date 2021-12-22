/*************************************************** 
 Based on Adafruit_MCP23008.h but MODIFIED BY S. Simmons to remove 
 unneeded #includes and any conditional compilation code

  This is a library for the MCP23008 port expander in either I2C or SPI modes:
  1) In I2C mode the bus address can be any value between 0x20 and ox27 (defaults to 0x20)
  2) In SPI mode the, the HAEN bit is alwasy cleared, so bits A1,A0 of the control byte are ignored,
     but the upper nibble of teh control byte must be 0100.
 ****************************************************/

#include <SPI.h>
#include <Wire.h>
#include "MCP23008.h"

void MCP23008::begin(void) 
{
	begin(0);//I@C INTERFACE USING DEFAULT 2C ADDRESS = 0x20
}

void MCP23008::begin(uint8_t addr_low, int INTpin) 
{
	addr_low &= 7;//ONLY LOWEST 3 BITS MATTER
	_i2caddr_low = addr_low;
	_pinINT = INTpin;
	_interface = MCP23008_I2C;
	
	Wire.begin();
	initGPIOregisters();	
}

void MCP23008::begin(int CSpin, int INTpin) 
{	//WILL USE SPI SUBSYSTEM FOR DATA TRANSFERS (so PINS ARE MOSI, MISO, SCK)
	_pinCS = CSpin;
	_interface = MCP23008_SPI;
	
	digitalWrite(_pinCS, HIGH);
	pinMode(_pinCS, OUTPUT);
	SPI.begin();//SETS UP MOSI, MISO, SCK pins
	initGPIOregisters();
	
}

void MCP23008::begin(int CSpin, int SINpin, int SOUTpin, int SCKpin, int INTpin) 
{	//WILL USE shigtOut() and shiftIN() FOR DATA TRANSFERS
	_pinCS = CSpin;
	_pinSIN = SINpin;
	_pinSOUT = SOUTpin;
	_pinCLK = SCKpin;
	_pinINT= INTpin;
	_interface = MCP23008_SHIFT;
	
	digitalWrite(_pinCS, HIGH);
	pinMode(_pinCS, OUTPUT);
	//shiftOut/In WILL NOT WORK WITHOUT SETTING to OUTPUT!!
	pinMode(_pinCS, OUTPUT);
	pinMode(_pinSOUT, OUTPUT);
	pinMode(_pinCLK, OUTPUT);
	initGPIOregisters();	
}


void MCP23008::initGPIOregisters()
{
	writeReg(MCP23008_IODIR, 0xFF);//ALL GPIO INOUTS
	writeReg(MCP23008_IPOL, 0x00);//NO INPUT LEVEL INVERSION
	writeReg(MCP23008_DEFVAL, 0x00);//DEFAULT COMPARE LEVELS
	writeReg(MCP23008_INTCON, 0x00);//CHANGES COMARED TO PREVIOUS PIN VLAUE
	writeReg(MCP23008_GPINTEN, 0x00);//ALL PIN-CHANGE INTERRUPTS DISABLED
	writeReg(MCP23008_IOCON, 0x40);//SEQOP disbaled, SPI addressing disabled, Open-Drain INT disbaled, INT active LOW
	writeReg(MCP23008_GPPU, 0x00);//ALL INTERNAL PULLUPS DISABLED
	writeReg(MCP23008_OLAT, 0x00);//GPIO OUTPUT LATCH 
}

void MCP23008::enableInterrupt(int gpio_pin, int defval, bool open_drainINT, int active_INTlevel)
{	//IF gpio_pin==-1, INTERRUPT WILL BE ESTABLISHED ON ALL 8 GPIO PINS, AND defval must be either  -1 OR A FULL byte VALUE
	//IF gpio_pin>=0, INTERRUPT WILL BE ESTABLISHED ON THAT ONE GPIO PIN, AND defval must be either -1,0,or 1  orFOR THAT PINb
	
	byte iocon_value = 0x00;//GENERAL SETUP
	byte defval_value = 0x00;//DEFVAL REGISTER
	byte gpint_value = 0x00;//ENABLES
	
	if(_pinINT==-1)
		return;//SKIP
	
	iocon_value |= (open_drainINT? 1:0)  << MCP23008_ODR_MASK;
	iocon_value |= active_INTlevel <<  MCP23008_INTPOL_MASK;
	writeReg(MCP23008_IOCON, iocon_value);
	if(gpio_pin==-1)
	{	//ESTABLISH ON ALL 8 GPIO PINS
		if(defval==-1)
			writeReg(MCP23008_INTCON, 0x00);//CHNAGE IS w.r.t TO PREVIOUS PIN LEVEL
		else
		{	writeReg(MCP23008_DEFVAL, defval);//RECORD EFVAL REGISTER BITS
			writeReg(MCP23008_INTCON, 0xFF);//SET CHANGE TO BE w.r.t DEFVAL BITSL
		}	
		writeReg(MCP23008_GPINTEN, 0xFF);//ENABLE ALL 8 INTERRUPT-ON-CHNAGE's
	}
	else
	{	//ENABLE ON JUST THE ONE SPECIFIC PIN
		
		if(defval==-1)
			writeReg(MCP23008_INTCON, 0x00);//CHNAGE IS w.r.t TO PREVIOUS PIN LEVEL
		else
		{	writeReg(MCP23008_DEFVAL, readReg(MCP23008_DEFVAL) | (defval<< gpio_pin));//SET USER defval AT THAT ONE POSITION
			writeReg(MCP23008_INTCON, readReg(MCP23008_INTCON) | (1 << gpio_pin));//SET CHANGE TO BE w.r.t USER defval LEVEL
		}
		writeReg(MCP23008_GPINTEN, readReg(MCP23008_INTCON) |(1 << gpio_pin));//ENABLE AT THAT ONE INTERRUPT-ON-CHNAGE
		
	}	
}

void MCP23008::disableInterrupt(int gpio_pin)
{	//IF gpio_pin==-1, INTERRUPT WILL BE DIABLED  ON ALL 8 GPIO PINS
	//IF gpio_pin>=0, INTERRUPT WILL BE ESTABLISHED ON THAT ONE GPIO PIN
	if(gpio_pin==-1)
		writeReg(MCP23008_GPINTEN, 0x00);
	else
		writeReg(MCP23008_GPINTEN, readReg(MCP23008_GPINTEN) & ~(1 << gpio_pin));
	
}

void MCP23008::pinModeGPIO(uint8_t p, uint8_t d) 
{	//Set direction of one of the 8 GPIO pins
	uint8_t iodir;
	
	// only 8 bits!
	if (p > 7)
		return;
	
	iodir = readReg(MCP23008_IODIR);
	
	// set the pin and direction
	if (d == INPUT) 
		iodir |= 1 << p; 
	else 
		iodir &= ~(1 << p);
	
	// write the new IODIR
	writeReg(MCP23008_IODIR, iodir);
}

uint8_t MCP23008::readGPIO(void) 
{	// read the current GPIO input
	writeReg(MCP23008_IODIR, 0xFF);//ALL INPUUTS
	return readReg(MCP23008_GPIO);
}

void MCP23008::writeGPIO(uint8_t gpio) 
{
	writeReg(MCP23008_IODIR, 0x00);//ALL OUTPUUTS
	writeReg(MCP23008_OLAT, gpio);
}


void MCP23008::digitalWriteGPIO(uint8_t p, uint8_t d) 
{	//WRite a level to on eof the 8 GPIO pins
	uint8_t gpio;
	
	// only 8 bits!
	if (p > 7)
		return;
	
	// read the current GPIO output latches
	gpio = readGPIO();
	
	// set the pin and direction
	if (d == HIGH) 
		gpio |= 1 << p; 
	else 
		gpio &= ~(1 << p);
	
	// write the new GPIO
	writeGPIO(gpio);
}


void MCP23008::pullUpGPIO(uint8_t p, uint8_t d) 
{	//Enable pullup resistor on one of  the 8 GPIO pins
	uint8_t gppu;
	
	// only 8 bits!
	if (p > 7)
		return;
	
	gppu = readReg(MCP23008_GPPU);
	// set the pullup
	if (d == HIGH) 
		gppu |= 1 << p; 
	else 
		gppu &= ~(1 << p);
	
	// write the new GPIO
	writeReg(MCP23008_GPPU, gppu);
}


uint8_t MCP23008::digitalReadGPIO(uint8_t p) 
{	//Read the level on one of the 8 GPIO pins
	// only 8 bits!
	if (p > 7)
		return 0;
	
	// read the current GPIO
	return (readGPIO() >> p) & 0x1;
}

uint8_t MCP23008::readReg(uint8_t reg_addr) 
{	//Read the byte in one of the MCP23008 registers
	byte bval;
	
	switch(_interface)
	{	case MCP23008_I2C:
			Wire.beginTransmission(MCP23008_ADDRESS | _i2caddr_low);
			Wire.write(reg_addr);	
			Wire.endTransmission();
			Wire.requestFrom(MCP23008_ADDRESS | _i2caddr_low, 1);
			bval=Wire.read();
			return bval;
		case MCP23008_SPI:
			digitalWrite(_pinCS, LOW);//SELECT 
			SPI.transfer(MCP23008_OPCODE_READ);
			SPI.transfer(reg_addr);
			bval = SPI.transfer(0xFF);
			digitalWrite(_pinCS, HIGH);//DE-SELECT 
			return bval;
		case MCP23008_SHIFT:
			digitalWrite(_pinCS, LOW);//SELECT 
			shiftOut(_pinSOUT, _pinCLK, MSBFIRST, MCP23008_OPCODE_READ);
			shiftOut(_pinSOUT, _pinCLK, MSBFIRST, reg_addr);
			bval = shiftIn(_pinSIN, _pinCLK, MSBFIRST);
			digitalWrite(_pinCS, HIGH);//DE-SELECT 
			return bval;
	}
}


void MCP23008::writeReg(uint8_t reg_addr, uint8_t data) 
{	//Write a byte into one of the MCP23008 registers
	switch(_interface)
	{	case MCP23008_I2C:
			Wire.beginTransmission(MCP23008_ADDRESS | _i2caddr_low);
			Wire.write(reg_addr);
			Wire.write(data);
			Wire.endTransmission();
			break;
		case MCP23008_SPI:			
			digitalWrite(_pinCS, LOW);//SELECT 
			SPI.transfer(MCP23008_OPCODE_WRITE);
			SPI.transfer(reg_addr);
			SPI.transfer(data);
			digitalWrite(_pinCS, HIGH);//DE-SELECT 
			break;
		case MCP23008_SHIFT:
			digitalWrite(_pinCS, LOW);//SELECT 
			shiftOut(_pinSOUT, _pinCLK, MSBFIRST, MCP23008_OPCODE_WRITE);
			shiftOut(_pinSOUT, _pinCLK, MSBFIRST, reg_addr);
			shiftOut(_pinSOUT, _pinCLK, MSBFIRST, data);
			digitalWrite(_pinCS, HIGH);//DE-SELECT 
			break;
	}
}
