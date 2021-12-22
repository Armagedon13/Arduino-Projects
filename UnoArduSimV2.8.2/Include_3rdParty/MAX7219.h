/*************************************************** 
 CreatedBY S. Simmons 

  This is a library for the MAX7219 8-digit LED 8-segment dispay controller 

 ****************************************************/

#define MAX7219_DIG0 0x01
#define MAX7219_DMODE 0x09
#define MAX7219_INTENS 0x0A
#define MAX7219_SCAN 0x0B
#define MAX7219_SHUT 0x0C

#define MAX7219_SPI 0
#define MAX7219_SHIFT 1

class MAX7219
{
	public:
	
	void begin(int CSpin);//for SPI interface,
	void begin(int CSpin, int SINpin, int SOUTpin, int CLKpin);//for shitOut/IN interface, with/without  interrupts
	void initRegisters();
	
	void displayOn();
	void displayOff();
	void clearDisplay();
	
	void chooseNumDigits(int num_digits);
	void printInteger(long number);
	void printFloat(float val, int places=2);
	void writeDisplayDigitCoded(int digit, byte B_value);
	void writeDisplayDigitRaw(int digit, byte segments_value);
	
	void writeReg(uint8_t reg_addr, uint8_t data);
	
	static byte m_B_Decode[16];
	
	private:
	
	uint8_t _pinCS;
	uint8_t _pinSIN;
	uint8_t _pinSOUT;
	uint8_t _pinCLK	;
	
	uint8_t _interface;
	uint8_t _num_digits;
	byte _dmodes;
};

