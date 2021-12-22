/*************************************************** 
  Based on Adafruit_LEDBackpack LIBRARY 
-- modified by S. Simmons to
a) remove the need for Adafruit_GFX 
b) remove all conditional compilation
 ************************************************/
#define LED_ON 1
#define LED_OFF 0

#define LED_RED 1
#define LED_YELLOW 2
#define LED_GREEN 3

#define HT16K33_BLINK_CMD 0x80
#define HT16K33_BLINK_DISPLAYON 0x01
#define HT16K33_BLINK_OFF 0
#define HT16K33_BLINK_2HZ  1
#define HT16K33_BLINK_1HZ  2
#define HT16K33_BLINK_HALFHZ  3

#define HT16K33_CMD_BRIGHTNESS 0xE0

#define SEVENSEG_DIGITS 5


// this is the raw HT16K33 controller
class Adafruit_LEDBackpack
{
	public:
	Adafruit_LEDBackpack(void);
	void begin(uint8_t _addr = 0x70);
	void setBrightness(uint8_t b);
	void blinkRate(uint8_t b);
	void writeDisplay(void);
	void clear(void);
	
	void _swap_int16_t(int16_t a, int16_t b) {	int16_t t = a; a = b; b = t; }
	
	uint16_t displaybuffer[8]; 
	
	protected:
	uint8_t rotation;
	uint8_t inverted;
	uint8_t i2c_addr;
	
	void setRotation(uint8_t r) {	rotation = (r & 3); }//FOR MATRIX LED DISPLAYS ONLY
	uint8_t getRotation( ) {	return rotation; }//FOR MATRIX LED DISPLAYS ONLY
	void invertDisplay(boolean i) {	inverted = i; }//FOR MATRIX LED DISPLAYS ONLY
};

class Adafruit_AlphaNum4 : public Adafruit_LEDBackpack 
{
	public:
	Adafruit_AlphaNum4(void);
	
	void writeDigitRaw(uint8_t n, uint16_t bitmask);
	void writeDigitAscii(uint8_t n, uint8_t ascii, boolean dot = false);
	
	private:
};

class Adafruit_24bargraph : public Adafruit_LEDBackpack 
{
	public:
	Adafruit_24bargraph(void);
	
	void setBar(uint8_t bar, uint8_t color);
	
	private:
};


class Adafruit_8x16matrix : public Adafruit_LEDBackpack
{
	public:
	Adafruit_8x16matrix(void);
	
	void drawPixel(int16_t x, int16_t y, uint16_t color);
	
};

class Adafruit_8x16minimatrix : public Adafruit_LEDBackpack
{
	public:
	Adafruit_8x16minimatrix(void);
	void drawPixel(int16_t x, int16_t y, uint16_t color);
};

class Adafruit_8x8matrix : public Adafruit_LEDBackpack
{
	public:
	Adafruit_8x8matrix(void);
	void drawPixel(int16_t x, int16_t y, uint16_t color);
};

class Adafruit_BicolorMatrix : public Adafruit_LEDBackpack
{
	public:
	Adafruit_BicolorMatrix(void);
	void drawPixel(int16_t x, int16_t y, uint16_t color);
};


class Adafruit_7segment : public Adafruit_LEDBackpack 
{
	public:
	Adafruit_7segment(void);
	size_t write(uint8_t c);
	
	void print(char c, int base = 0);
	void print(unsigned char, int base = 0);
	void print(int, int base = DEC);
	void print(unsigned int, int base = DEC);
	void print(long, int base = DEC);
	void print(unsigned long, int base = DEC);
	void print(double, int base = 2);
	void println(char c, int base = 0);
	void println(unsigned char, int base = 0);
	void println(int, int base = DEC);
	void println(unsigned int, int base = DEC);
	void println(long, int base = DEC);
	void println(unsigned long, int base = DEC);
	void println(double, int places = 2);
	void println(void);
	
	void writeDigitRaw(uint8_t x, uint8_t bitmask);
	void writeDigitNum(uint8_t x, uint8_t num, boolean dot = false);
	void drawColon(boolean state);
	void printNumber(long, uint8_t places = 2);
	void printFloat(double, uint8_t places = 2, uint8_t base = DEC);
	void printError(void);
	
	void writeColon(void);
	
	private:
	uint8_t position;
};

