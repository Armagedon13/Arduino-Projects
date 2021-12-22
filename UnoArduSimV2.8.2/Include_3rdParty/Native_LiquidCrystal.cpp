/*
//BASED ON THE Arduino ANDN Adafruit LiquidCrystal libraries
//WITH MODIFICATION BY S. Simmons including:

1) All conditional complation directives and associated code have been removed
2) All constructors and functions code that suppoorts **8-BIT** PARALLEL  INTERFACAE to an LCD has been removed --
only I2C, SPI, and 4-BIT interfaces TO THE THE LCD MODULLE's INTEGRATED CONTROLLER are supported.

ote that 5x10 pixel mode is NOT SUPPORTED in UnoArduSim  and that
backlight operations are bot supported
*/

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1 
//    S = 0; No shift 
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// Native_LiquidCrystal constructor is called).


#include "Native_LiquidCrystal.h"

Native_LiquidCrystal::Native_LiquidCrystal(uint8_t rs, uint8_t  rw, uint8_t enable,  uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{ 
	init(rs, rw, enable, d0, d1, d2, d3);
}

Native_LiquidCrystal::Native_LiquidCrystal(uint8_t rs, uint8_t enable,  uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{ 
	init(rs, 255, enable, d0, d1, d2, d3);
}

void Native_LiquidCrystal::init(uint8_t rs, uint8_t rw, uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
	_rs_pin = rs;
	_rw_pin = rw;
	_enable_pin = enable;
	
	_data_pins[0] = d0; // really D4 of LCD controller
	_data_pins[1] = d1; // really D5 of LCD controller
	_data_pins[2] = d2; // really D6 of LCD controller
	_data_pins[3] = d3; // really D7 of LCD controller
	
	_i2cAddr = 255;
	_SPIclock = _SPIdata = _SPIselect = 255;
	
	_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
	
}

Native_LiquidCrystal::Native_LiquidCrystal(uint8_t i2caddr) 
{
	_i2cAddr = i2caddr;
	_displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;
	
}


Native_LiquidCrystal::Native_LiquidCrystal(uint8_t data, uint8_t clock, uint8_t select, uint8_t rs) 
{
	_i2cAddr = 255;
	
	_displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;
	
	_SPIdata = data;
	_SPIclock = clock;
	_SPIselect = select; //the active-LOW select; its rising edge captures the received data on a write
	_rs_pin = rs;
}


void Native_LiquidCrystal::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) 
{
	
	//DEPENDS ON I2C, SPI, or 4-BIT PARALLEL INTERFACE
	if (_i2cAddr != 255) 
	{	//MIMIMAL SETUP -- LCD IS ALREADY IN 8-BIT MODE AND NEEDS NO FRONT-END INITIALIZATION
		Wire.begin();
		delayMicroseconds(50000); 
	} 
	else if (_SPIclock != 255) 
	{
		//MIMIMAL SETUP -- LCD IS ALREADY IN 8-BIT MODE AND NEEDS NO FRONT-END INITIALIZATION
		pinMode(_SPIdata, OUTPUT);
		pinMode(_SPIclock, OUTPUT);
		pinMode(_SPIselect, OUTPUT);
		pinMode(_rs_pin, OUTPUT);
		delayMicroseconds(50000); 
	} 
	else 
	{	//4-bit PARALLEL DATA INTERFCAE
		
		// Now we pull both RS and R/W low to begin commands
		pinMode(_rs_pin, OUTPUT);
		digitalWrite(_rs_pin, LOW);
		
		// we can save 1 pin by not using RW. Indicate by passing 255 instead of pin#
		if (_rw_pin != 255) //else user nust wire it so that it always LOW
			pinMode(_rw_pin, OUTPUT);
		
		pinMode(_enable_pin, OUTPUT);
		digitalWrite(_enable_pin, LOW);//ENABLE=FALSE to start
		
		for(int k=0; k<=3; k++)
			pinMode(_data_pins[k], OUTPUT);	
		
		// SEE PAGE 46 OF Hitachi44780 datah=sheet FOR INITIALIZATION SPECIFICATION!
		// according to datasheet, we need at least 40ms after power rises above 2.7V
		// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
		
		delayMicroseconds(50000); //need to wait at least 40 msecs after reset
		
		//according to the hitachi HD44780 datasheet figure 24, pg 46	
		//we start up in 8bit mode, try to set 4 bit mode
		write4bits(0x03);
		delayMicroseconds(4500); // wait min 4.1ms	
		// second try
		write4bits(0x03);
		delayMicroseconds(4500); // wait min 4.1ms	
		// third go!
		write4bits(0x03); 
		delayMicroseconds(150);
		// finally, set to 4-bit interface
		write4bits(0x02); 
	}
	
	_numlines = lines;
	if (lines > 1) 
		_displayfunction |= LCD_2LINE;
	_currline = 0;
	// for some 1 line displays you can select a 10 pixel high font
	if ((dotsize != 0) && (lines == 1))
		_displayfunction |= LCD_5x10DOTS;//eill cause ERROR MESSAGE in UnoArduSim as is UNSUPPORTED
	
	// set # lines, font size, etc.
	command(LCD_FUNCTIONSET | _displayfunction);  
	
	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;  
	display();
	
	// clear it off
	clear();
	
	//Initialize to default text direction (for romance languages)
	//S. Simmons: in next, LCD_ENTRYLEFT  really means 'LCD_ENTRY_ADDRESS_INCREMEENT', 
	//and LCD_ENTRYSHIFTDECREMENT really means 'LCD_ENTRY_ENABLE_DISPLAYSHIFT'
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	// set the entry mode
	command(LCD_ENTRYMODESET | _displaymode);
}

/********** high level commands, for the user! */
void Native_LiquidCrystal::clear()
{
	command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
}

void Native_LiquidCrystal::home()
{
	command(LCD_RETURNHOME);  // set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
}

void Native_LiquidCrystal::setCursor(uint8_t col, uint8_t row)
{
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if ( row > _numlines )
		row = _numlines-1;    // we count rows starting w/0
	
	command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void Native_LiquidCrystal::noDisplay() 
{
	_displaycontrol &= ~LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void Native_LiquidCrystal::display() 
{
	_displaycontrol |= LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void Native_LiquidCrystal::noCursor() 
{
	_displaycontrol &= ~LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void Native_LiquidCrystal::cursor() 
{
	_displaycontrol |= LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void Native_LiquidCrystal::noBlink() 
{
	_displaycontrol &= ~LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void Native_LiquidCrystal::blink() 
{
	_displaycontrol |= LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void Native_LiquidCrystal::scrollDisplayLeft(void) 
{
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void Native_LiquidCrystal::scrollDisplayRight(void) 
{
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void Native_LiquidCrystal::leftToRight(void) 
{	//S. Simmons: in next, LCD_ENTRYLEFT  really means 'LCD_ENTRY_ADDRESS_INCREMEENT', 
	
	_displaymode |= LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void Native_LiquidCrystal::rightToLeft(void) 
{	//S. Simmons: in next, LCD_ENTRYLEFT  really means 'LCD_ENTRY_ADDRESS_INCREMEENT', 
	
	_displaymode &= ~LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void Native_LiquidCrystal::autoscroll(void) 
{	//S. Simmons: in next  LCD_ENTRYSHIFTINCREMENT really means LCD_ENTRY_ENABLE_DISPLAYSHIFT
	
	_displaymode |= LCD_ENTRYSHIFTINCREMENT; 
	command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void Native_LiquidCrystal::noAutoscroll(void) 
{	//S. Simmons: in next  LCD_ENTRYSHIFTINCREMENT really means LCD_ENTRY_ENABLE_DISPLAYSHIFT
	
	_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;//LCD_ENTRYSHIFTINCREMENT should have been called LCD_ENTRYDISPLAYSHIFT (the 'S' BIT)
	command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations with custom characters
void Native_LiquidCrystal::createChar(uint8_t location, uint8_t charmap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	command(LCD_SETCGRAMADDR | (location << 3));
	for (int i=0; i<8; i++) 
		write(charmap[i]);
}

/*********** mid level commands, for sending data/cmds */

void Native_LiquidCrystal::command(uint8_t value) 
{
	send(value, LOW);
}


size_t Native_LiquidCrystal::write(uint8_t value) 
{	//DEFINES THE virtual FUNCTION USED FOR Print::write(uint8_t value)
	send(value, HIGH);
	return 1;
}

// write either command or data, with automatic 4/8-bit selection
void Native_LiquidCrystal::send(uint8_t value, boolean mode) 
{	
	if (_i2cAddr != 255)
	{	//I2C ON LCD
		Wire.beginTransmission(_i2cAddr);
		Wire.write(mode << 6);//SEND CONTROL BYTE WITH C0=0, A0 = mode
		Wire.write(value);	
		Wire.endTransmission();	
		delayMicroseconds(50);   // > 37us for LCD to process
	}
	else if  (_SPIclock != 255)
	{	//SPI INTERFACE ON LCD
		digitalWrite(_rs_pin, mode);
		digitalWrite(_SPIselect, LOW);
		shiftOut(_SPIdata, _SPIclock,  MSBFIRST,  value);		
		digitalWrite(_SPIselect, HIGH);		
		delayMicroseconds(50);   // > 37us for LCD to process
	}
	else
	{	//4-bit BUS INTERFACE TO LCD
		digitalWrite(_rs_pin, mode);	
		
		for(int k=0; k<=3; k++)
			pinMode(_data_pins[k], OUTPUT);	
		write4bits(value>>4);//DOES  > 37us for LCD to process
		write4bits(value);//DOES  > 37us for LCD to process
	}
}

// receive a data byte from the LCD (the byte at the current address counter)
uint8_t Native_LiquidCrystal::receive() 
{	//CAN BE USED ONLY FOR 4-bit BUS INTERFACE
	int k;
	uint8_t high_nibble,  data_byte;
	
	for(int k=0; k<=3; k++)
		pinMode(_data_pins[k], INPUT);	
	
	//4-bit BUS INTERFACE TO LCD TO READ NEXT RAM DATA BYTE
	digitalWrite(_rs_pin, HIGH);
	//  there MUST a RW pin indicated in order to READ
	if (_rw_pin == 255) 
		return 0xFF;//DUMMY INVALID RAD BYTE
	high_nibble =  read4bits();
	data_byte = high_nibble << 4;
	data_byte |= read4bits();//append low nibble
	return data_byte;
}


void Native_LiquidCrystal::write4bits(uint8_t nibble) 
{	//USED ONLY FOR 4-BIT BUS INTERFACE TO LCD TO DO HALF OF THE BYTE TRANSFER
	
	if(_rw_pin!=255)//else user nust must wire it so that it always LOW
		digitalWrite(_rw_pin, LOW);//MUST BE SET VEFORE E GOESHIGH
	for (int i = 0; i <= 3; i++) 
	{	pinMode(_data_pins[i], OUTPUT);
		digitalWrite(_data_pins[i], (nibble >> i) & 0x01);
	}
	pulseEnable();//LCD captures datsa and R/W on FALLING edge
	delayMicroseconds(50);   // ALLOW > 37us for LCD to process
}

uint8_t Native_LiquidCrystal::read4bits() 
{	//USED ONLY FOR 4-BIT BUS INTERFACE TO LCD TO DO HALF OF THE BYTE TRANSFER
	uint8_t nibble = 0x00;
	
	//ON ENTRY, _enable_pin will ALREADY be LOW
	digitalWrite(_rw_pin, HIGH);//MUST BE SET VEFORE enable GOES HIGH
	digitalWrite(_enable_pin, HIGH);
	for (int i = 3; i >=0; i--)
	{	nibble = nibble << 1;
		pinMode(_data_pins[i], INPUT);
		nibble |= digitalRead(_data_pins[i]);
	}
	digitalWrite(_enable_pin, LOW);//BACK TO enable=FALSE
	return nibble;
	
}

void Native_LiquidCrystal::pulseEnable(void) 
{	//USED ONLY FOR 4-BIT BUS INTERFACE TO LCD Ifor WRITEING COMMAND OR DATA
	
	//ON ENTRY, _enable_pin, will ALREADY be LOW
	delayMicroseconds(1);    
	digitalWrite(_enable_pin, HIGH);
	delayMicroseconds(1);    // enable pulse must be >450ns
	digitalWrite(_enable_pin, LOW);
}
