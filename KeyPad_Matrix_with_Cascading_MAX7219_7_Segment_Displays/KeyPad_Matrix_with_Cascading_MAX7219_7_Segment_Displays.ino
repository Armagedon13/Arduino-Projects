#include "Arduino.h"
//-------------------------------------------------------------------------
// MAX7219 LED DIGIT output pin definitions
//-------------------------------------------------------------------------
#define MAX2719_DIN  2
#define MAX2719_CS  3
#define MAX2719_CLK 4
#define ledpin 13

// define max7219 registers - stops us using 'magic numbers' in our code
byte max7219_reg_noop = 0x00;
byte max7219_reg_digit0 = 0x01;
byte max7219_reg_digit1 = 0x02;
byte max7219_reg_digit2 = 0x03;
byte max7219_reg_digit3 = 0x04;
byte max7219_reg_digit4 = 0x05;
byte max7219_reg_digit5 = 0x06;
byte max7219_reg_digit6 = 0x07;
byte max7219_reg_digit7 = 0x08;
byte max7219_reg_decodeMode = 0x09;
byte max7219_reg_intensity = 0x0a;
byte max7219_reg_scanLimit = 0x0b;
byte max7219_reg_shutdown = 0x0c;
byte max7219_reg_displayTest = 0x0f;

//-------------------------------------------------------------------------
// Key Matrix set up
//-------------------------------------------------------------------------
#include "Keypad.h"
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = { { '1', '2', '3' }, { '4', '5', '6' }, { '7', '8', '9' }, { '*', '0', '#' } };

//-------------------------------------------------------------------------
// This determines your buffer size and various loops - change to suit!
//-------------------------------------------------------------------------
#define numberOfUnits 2
byte msgBuffer[numberOfUnits * 8]; // key press message buffer, set to spaces

//-------------------------------------------------------------------------
// Map the matrix pins to underlying Keypad object (to read the characters)
//-------------------------------------------------------------------------
byte rowPins[ROWS] = { 12, 11, 10, 9 }; //connect to the row pin outs of the keypad
byte colPins[COLS] = { 8, 7, 6 }; //connect to the column pin outs of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//-------------------------------------------------------------------------
// Controls the printing of messages in Serial Monitor window. If the specified
// pin is LOW then we are in debug, otherwise no serial monitor printing takes place
// Allows you to run 'live' without serial port slowing the whole thing down
//-------------------------------------------------------------------------
//#define A0 14 - either do this or what I have done below. NOt required for
// the standard Arduino IDE. Eclipse has a bug!
bool isDebug() {
	return digitalRead((unsigned char) A0) == LOW ? true : false;
}

// ----------------------------------------------------------------------------
// One size fits all Serial Monitor debugging messages. Only prints 'debug'
// messages to the serial window if we have set A0 (or whatever pin) to LOW
// ----------------------------------------------------------------------------
template<typename T>
void debugPrint(T printMe, bool newLine = false) {
	if (isDebug()) {
		if (newLine) {
			Serial.println(printMe);
		}
		else {
			Serial.print(printMe);
		}
		Serial.flush();
	}
}

//-------------------------------------------------------------------------
// SET UP      SET UP      SET UP      SET UP      SET UP      SET UP
//-------------------------------------------------------------------------
void setup() {
	initialise();

	// For test mode (all digits on) set to 0x01. Normally we want this off (0x00)
	printCmd(max7219_reg_displayTest, 0x0);

	// Set all digits off initially
	printCmd(max7219_reg_shutdown, 0x0);

	// Set brightness for the digits to high(er) level than default minimum (Intensity Register Format)
	printCmd(max7219_reg_intensity, 0x02);

	// Set decode mode for ALL digits to output actual ASCII chars rather than just
	// individual segments of a digit
	printCmd(max7219_reg_decodeMode, 0xFF);

	// Blank all the LED digits with a 'space' character to clear and random garbage
	clearBuffer();
	printBuffer();

	// Ensure ALL digits are displayed (Scan Limit Register)
	printCmd(max7219_reg_scanLimit, 0x07);

	// Turn display ON (boot up = shutdown display)
	printCmd(max7219_reg_shutdown, 0x01);
}

//-------------------------------------------------------------------------
// LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP
//-------------------------------------------------------------------------
void loop() {
	char key = keypad.getKey();
	if (key) // Check for a valid key.
	{
		switch (key) {

			case '*': // Clear the display
				digitalWrite(ledpin, LOW);
				clearBuffer();
				break;

			case '#': // Print out HELP a few times
				digitalWrite(ledpin, HIGH);
				helpBuffer();
				break;

			default: // Standard key press handler

				// What key was it?
				debugPrint("Key press: ");
				debugPrint(key, true);

				// Shift message buffer chars to left (scroll left)
				for (unsigned int cnt = 0; cnt < (sizeof msgBuffer) - 1; cnt++) {
					msgBuffer[cnt] = msgBuffer[cnt + 1];
					debugPrint(cnt);
					debugPrint(": ");
					debugPrint(msgBuffer[cnt], true);
				}

				// Store the key press in the buffer
				msgBuffer[sizeof(msgBuffer) - 1] = key;
		}

		// Output the buffer onto the digits.
		/*
		 *  Remember that the buffer goes from 0 to (say) 16 but the digits on the
		 *  MAX7219 start at 1 on the RIGHT moving to the highest digit on the LEFT,
		 *  which is the REVERSE of the buffer left-to-right layout.
		 */
		printBuffer();
	}
}

//-------------------------------------------------------------------------
// MAX7219 initialisation, telling it which pints are what etc
//-------------------------------------------------------------------------
void initialise() {
	// MAX7219
	digitalWrite(MAX2719_CS, HIGH);
	pinMode(MAX2719_DIN, OUTPUT);
	pinMode(MAX2719_CS, OUTPUT);
	pinMode(MAX2719_CLK, OUTPUT);

	// For KEYPAD matrix
	pinMode(ledpin, OUTPUT);
	digitalWrite(ledpin, LOW);
	Serial.begin(9600);

	// For debugging mode
	pinMode((unsigned char) A0, INPUT_PULLUP);
}

//-------------------------------------------------------------------------
// This is the routine that prints the value to the correct display and a
// NOP output to all others.
//-------------------------------------------------------------------------
void printDigit(byte digit, byte data) {
	// Allow writing to the MAX7219 displays
	digitalWrite(MAX2719_CS, LOW);

	// Let's see what is happening
	if (isDebug()) {
		Serial.print("Digit: ");
		Serial.print(digit, DEC);
		Serial.print(" Data: ");
		Serial.println(data, HEX);
	}

	// Pay attention. This is important. And tricky. But simple too.
	/* We must send the required digit value to the correct unit.
	 *
	 * We MUST send a value to ALL units though. The units that must
	 * do nothing are sent a NO-OP (no operation) command. Then they
	 * are happy and do nothing. The unit to which we want to send the
	 * value gets it in the normal manner.
	 *
	 * The tricky bit is remembering that units are numbered from the
	 * RIGHT. So if you have four units they are numbered thus:
	 *
	 * --- UNIT 4 ---- UNIT 3 ----- UNIT 2 ----- UNIT 1----
	 *
	 * In the above scenario if we want to send a value to UNIT 2 we
	 * send the following sequence:
	 *
	 * NOP		(received by UNIT 1 - the last, rightmost unit)
	 * VALUE	(received by UNIT 2)
	 * NOP		(received by UNIT 3)
	 * NOP		(received by UNIT 4 - the first, leftmost unit)
	 *
	 * Confused? I'm not surprised. Re-read. Digest. Re-read. Understand!
	 */

	// Calculate the unit where the digit is on.
	/* In our example if we have four units, that's 32 digits. We are
	 * targeting UNIT 2 (second from the RIGHT) so that's digit number
	 * 9 thru 16. Let's pick digit 12 for our example.
	 *
	 * (12 - 1) / 8 = 1 (we ignore the remainder).
	 *
	 * This is a zero based array.
	 * Thus the answer is value from 0 (unit 1) to 3 (unit 4).
	 *
	 * Additionally we need to work out the digit within the unit,
	 * known as the offset.
	 *
	 * So if we have received data for digit 12 (as above) we must
	 * calculate that 12 is, in fact, the 4th digit in unit 2. We do this
	 * by subtracting
	 * 			targetUnit value (1 - 2nd unit, zero based!) * number of digits (8)
	 * from the incoming digit number. So simple. Yeah, right.
	 */
	int unitTarget = (digit - 1) / 8;
	int offset = unitTarget * 8;
	debugPrint("Target Unit: "); debugPrint(unitTarget, true);

	// Send out a command to EACH unit - if it is our desired unit send
	// the actual value otherwise send a NOOP (no operation command)
	for (int unit = 0; unit < numberOfUnits; unit++) {

		// parameters: shiftOut(dataPin, clockPin, bitOrder, value)
		if (unit == unitTarget) {
			// Send out two bytes (16 bit)
			shiftOut(MAX2719_DIN, MAX2719_CLK, MSBFIRST, digit - offset);
			shiftOut(MAX2719_DIN, MAX2719_CLK, MSBFIRST, data);
		}
		else {
			// Send out a NOOP plus 0 for data
			shiftOut(MAX2719_DIN, MAX2719_CLK, MSBFIRST, max7219_reg_noop);
			shiftOut(MAX2719_DIN, MAX2719_CLK, MSBFIRST, 0);
		}
	}

	// Stop writing to the displays (data now gets sent to all units)
	digitalWrite(MAX2719_CS, HIGH);
}

//-------------------------------------------------------------------------
// Print a command to ALL MAX7219s
//-------------------------------------------------------------------------
void printCmd(byte cmd, byte data) {

	// Monitor the commands being sent
	if (isDebug()) {
		Serial.print("Command: ");
		Serial.print(cmd, HEX);
		Serial.print(" Data: ");
		Serial.println(data, HEX);
	}

	// Allow writing to the MAX7219 displays
	digitalWrite(MAX2719_CS, LOW);

	// Send out two bytes (16 bit) to each unit
	//
	// parameters: shiftOut(dataPin, clockPin, bitOrder, value)
	//
	for (int cnt = 0; cnt < numberOfUnits; cnt++) {
		shiftOut(MAX2719_DIN, MAX2719_CLK, MSBFIRST, cmd);
		shiftOut(MAX2719_DIN, MAX2719_CLK, MSBFIRST, data);
	}

	// Stop writing to the displays (data now gets sent)
	digitalWrite(MAX2719_CS, HIGH);
}

//-------------------------------------------------------------------------
// Send spaces to the buffer (clear any existing digits there)
//-------------------------------------------------------------------------
void clearBuffer() {

	// Clear the message buffer with space characters
	for (unsigned int cnt = 0; cnt < sizeof msgBuffer; cnt++) {
		msgBuffer[cnt] = 0x0F;
	}
}

//-------------------------------------------------------------------------
// Print out the word HELP to all digits in all units. Well, why not?
//-------------------------------------------------------------------------
void helpBuffer() {
	// H-E-L-P  see data sheet table 5 Code B Font to see how these are derived
	byte help[] = { 0xC, 0xB, 0xD, 0xE };

	int bcnt = 0;
	for (unsigned int loop = 0; loop < (numberOfUnits * 2); loop++) {
		for (unsigned int hcnt = 0; hcnt < 4; hcnt++) {
			msgBuffer[bcnt] = help[hcnt];
			bcnt++;
		}
	}
}

//-------------------------------------------------------------------------
// This is where we print the buffer (left-to-right) to the digits (right to left)
//-------------------------------------------------------------------------
void printBuffer() {
	// Output all characters in double for loop (one increasing, other decreasing)
	for (unsigned int cnt = 1, bcnt = (sizeof msgBuffer) - 1; cnt <= numberOfUnits * 8; cnt++, bcnt--) {
		printDigit(cnt, msgBuffer[bcnt]);
	}
}
