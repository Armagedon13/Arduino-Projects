/*
//BASED ON THE (DFRobot) LiquidCystal_I2C library
//BUT MODIFIED BY S. Simmons to remove all conditional complation directives and associated code

Note that 5x10 pixel mode is NOT SUPPORTED in UnoArduSim  (and cuases an ERROR MESSAGE) and that
backlight operations are allowed bu cause no visual change in UnoArduSim
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
#define LCD_5x10DOTS 0x04//UNSUPPORTED IN UnoArduSim
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En B00000100  // Enable bit
#define Rw B00000010  // Read/Write bit
#define Rs B00000001  // Register select bit

class LiquidCrystal_I2C: public Print 
{
	public:
	LiquidCrystal_I2C(uint8_t lcd_Addr,uint8_t lcd_cols,uint8_t lcd_rows);
	void begin(uint8_t cols, uint8_t rows,  uint8_t dotsize);
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
	void printLeft();
	void printRight();
	void leftToRight();
	void rightToLeft();
	void shiftIncrement();
	void shiftDecrement();
	void noBacklight();
	void backlight();
	void autoscroll();
	void noAutoscroll(); 
	void createChar(uint8_t, uint8_t[]);
	void setCursor(uint8_t, uint8_t); 
	virtual size_t write(uint8_t);
	void command(uint8_t);
	void init();
	
	////compatibility API function aliases
	void blink_on();						// alias for blink()
	void blink_off();       					// alias for noBlink()
	void cursor_on();      	 					// alias for cursor()
	void cursor_off();      					// alias for noCursor()
	void setBacklight(uint8_t new_val);				// alias for backlight() and nobacklight()
	void load_custom_character(uint8_t char_num, uint8_t *rows);	// alias for createChar()
	void printstr(const char[]);
	
	private:
	void init_priv();
	void send(uint8_t, uint8_t);
	void write4bits(uint8_t);
	void expanderWrite(uint8_t);
	void pulseEnable(uint8_t);
	uint8_t _Addr;
	uint8_t _displayfunction;
	uint8_t _displaycontrol;
	uint8_t _displaymode;
	uint8_t _numlines;
	uint8_t _cols;
	uint8_t _rows;
	uint8_t _backlightval;
};

