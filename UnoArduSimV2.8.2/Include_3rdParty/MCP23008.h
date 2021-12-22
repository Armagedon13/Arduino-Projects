/*************************************************** 
 Based on Adafruit_MCP23008.h but MODIFIED BY S. Simmons to remove 
 unneeded #includes and any conditional compilation code

  This is a library for the MCP23008 port expander in either I2C or SPI modes:
  1) In I2C mode the bus address can be any value between 0x20 and ox27 (defaults to 0x20)
  2) In SPI mode the, the HAEN bit is alwasy cleared, so bits A1,A0 of the control byte are ignored,
     but the upper nibble of teh control byte must be 0100.
 ****************************************************/

#define MCP23008_ADDRESS 0x20

// registers
#define MCP23008_IODIR 0x00
#define MCP23008_IPOL 0x01
#define MCP23008_GPINTEN 0x02
#define MCP23008_DEFVAL 0x03
#define MCP23008_INTCON 0x04
#define MCP23008_IOCON 0x05
#define MCP23008_GPPU 0x06
#define MCP23008_INTF 0x07
#define MCP23008_INTCAP 0x08
#define MCP23008_GPIO 0x09
#define MCP23008_OLAT 0x0A

#define MCP23008_OPCODE_WRITE 0x40
#define MCP23008_OPCODE_READ 0x41

//IOCON REGISTER BIT POSITIONS ROLES
#define MCP23008_INTPOL_MASK 0x02 //bit position 1 (INT pin active level)
#define MCP23008_ODR_MASK 0x04 //bit position 2 (open-drain INT pin output)
#define MCP23008_HAEN_MASK 0x08 //bit position 3 (ALWAYS IGNORED in MCP23008.cpp)
#define MCP23008_DSSLW_MASK 0x10 //bit position 4 ((ALWAYS IGNORED i in UnoArduSim)
#define MCP23008_SEQOP_MASK 0x20 //bit position 5 --SEQUNETIAL TREGISTER ADDRESSING (1=disabled, 0 = enabled)

//INTERFCAE MODES
#define MCP23008_SPI 	0
#define MCP23008_I2C 	1
#define MCP23008_SHIFT	2



class MCP23008 
{
	public:
	
	void begin();//I2C interface at default address 0x20, with no interrupts
	void begin(uint8_t addr_low, int INTpin=-1);//for I2C interface at bus (0x20 + addr_low\0,  with/without  interrupts
	void begin(int CSpin, int INTpin=-1);//for SPI interface, with/without  interrupts
	void begin(int CSpin, int SINpin, int SOUTpin, int CLKpin, int INTpin=-1);//for shitOut/IN interface, with/without  interrupts
	void initGPIOregisters();
	
	void enableInterrupt(int gpio_pin, int defval, bool open_drainINT, int active_INTlevel);
	void disableInterrupt(int gpio_pin);
	
	void pinModeGPIO(uint8_t p, uint8_t d);
	void digitalWriteGPIO(uint8_t p, uint8_t d);
	void pullUpGPIO(uint8_t p, uint8_t d);
	uint8_t digitalReadGPIO(uint8_t p);
	uint8_t readGPIO(void);
	void writeGPIO(uint8_t);
	
	uint8_t readReg(uint8_t reg_addr);
	void writeReg(uint8_t reg_addr, uint8_t data);
	
	private:
	
	uint8_t _interface;
	uint8_t _i2caddr_low;
	uint8_t _pinCS;
	uint8_t _pinSIN;
	uint8_t _pinSOUT;
	uint8_t _pinCLK;
	uint8_t _pinINT;
	
};

