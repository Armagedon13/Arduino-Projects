/*
//BASED ON THE Arduino ANDN Adafruit LiquidCrystal libraries
//WITH MODIFICATION BY S. Simmons including:

1) All conditional complation directives and associated code have been removed
2) All constructors and functions code that suppoorts **8-BIT** PARALLEL  INTERFACAE to an LCD has been removed --
only I2C, SPI, and 4-BIT interfaces TO THE THE LCD MODULLE's INTEGRATED CONTROLLER are supported.

ote that 5x10 pixel mode is NOT SUPPORTED in UnoArduSim  and that
backlight operations are allowed but cause no visual change in UnoArduSim
*/

#include <Wire.h>
#include "Print.h"

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00 //S. Simmons: really means 'LCD_ENTRY_ADDRESS_DECREMEENT' as ithis sets 'I/D'=0
#define LCD_ENTRYLEFT 0x02 //S. Simmons: really means 'LCD_ENTRY_ADDRESS_INCREMEENT' as ithis sets 'I/D'=1
#define LCD_ENTRYSHIFTINCREMENT 0x01 //S. Simmons: really means 'LCD_ENTRY_ENABLE_DISPLAYSHIFT' as it sets 'S'=1
#define LCD_ENTRYSHIFTDECREMENT 0x00 //S. Simmons: really means 'LCD_ENTRY_DISABLE_DISPLAYSHIFT' as it sets 'S'=0

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10 
#define LCD_4BITMODE 0x00 
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04 //UNSUPPORTED INN UnoArduSim
#define LCD_5x8DOTS 0x00

class Native_LiquidCrystal : public Print 
{
	public:
	
	//4-BIT PARALLEL with or without a READ/WRITE pin
	Native_LiquidCrystal(uint8_t rs,  uint8_t rw, uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);
	Native_LiquidCrystal(uint8_t rs,  uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);
	void init(uint8_t rs, uint8_t rw, uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);
	
	//I2C Wire INTERFACE TO i2c port on serial "backpack" on LCD BOARD
	Native_LiquidCrystal(uint8_t i2cAddr);
	
	//SPI-like  INTERFACE TO INTEGRATED SPI INTERFCAE on LCD BOARD
	Native_LiquidCrystal(uint8_t data, uint8_t clock, uint8_t select, uint8_t rs);
	
	void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);
	
	void clear();
	void home();
	void noDisplay();
	void display();
	void noBlink();
	void blink();
	void noCursor();
	void cursor();
	void scrollDisplayLeft();
	void scrollDisplayRight();
	void leftToRight();
	void rightToLeft();
	void autoscroll();
	void noAutoscroll();
	
	void createChar(uint8_t, uint8_t[]);
	void setCursor(uint8_t, uint8_t); 
	virtual size_t write(uint8_t);
	void command(uint8_t);
	uint8_t receive();//READ NEXT RAM DATA_BYTE

	private:
	void send(uint8_t value, boolean mode);//SEND COMMAND OR RAM-DATA BYTE
	void write4bits(uint8_t);
	uint8_t read4bits();//ONE HALF OF A receive() BYTE
	void pulseEnable();//for 4-BIT PARALLEL BUS INTERFCAE WRITE
	
	uint8_t _rs_pin; // LOW: command.  HIGH: character.
	uint8_t _rw_pin; // ALWAYS SET TO 255 IN  UnoArduSim SINCE IO DEVICE DOES NOT ALLOW SPECIFICATION OF A R/W pin
	uint8_t _enable_pin; // activated by a HIGH pulse.
	uint8_t _data_pins[4];//ONLY 4 DATA BUITS (since only 4-BIT MODE IS USED BY THE BACKPACK FOR I2C or SPI INTERFCAE)
	
	uint8_t _displayfunction;
	uint8_t _displaycontrol;
	uint8_t _displaymode;
	uint8_t _initialized;
	uint8_t _numlines;
	uint8_t _currline;
	
	uint8_t _SPIclock;
	uint8_t _SPIdata;
	uint8_t _SPIselect;
	uint8_t _i2cAddr;
	
};

