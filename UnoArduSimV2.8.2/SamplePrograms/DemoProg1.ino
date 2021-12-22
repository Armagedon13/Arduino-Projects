
/* 
	Sample Program to show some of the IO devices in action
	Do Confgiure|'I/O' Devices and load (if not already auro-loaded)
	SampleProgram/myUnoDevsDemoProg1.txt before running
	
	The DC Motor is driven by Pulser for fun; and has its encoder
	output tics counted by an interrupt routine -- the count
	is used by loop() to flip the motor direction periodically
		
	Stepper pin 3 signal is also fed to a Piezo speaker so you
	can also "hear" the stepper motor stepping 	Pressing pushbutton 
	on pin 5 will reverse the stepping direction of stepper1.

	Slide Analog Slider on pin A0 to control the drection of 
	servo1's motion.
	
	Double-click on the Serial (SERIAL) I/O device to see an
	expanded view of its received ( println()'d) characters.
	
	Left-click on Uno pin 2, 3 and 4 to bring up a view of the
	DC motor encoder output plus the two stepper1 control signal
	activity, and RIGHT-click on pin A2 to see the FuncGen's 
	output waveform.

	Drag the main toolbar's IO Values Scaler slider control to
	change the pulse width on the PULSER's pin 6 (which drives
	the DC motor PWM inout, and so controls its speed)
*/

#include <SPI.h>
#include <Stepper.h>
#include <SoftwareSerial.h>
#include <Servo.h>

#define TICSPERREV 16 //16 optical encoder tics per wheel revolution

const int spiSS = 10;//slave slect for SPI Slave
const int stprP1 = 3;//stepper pin P1
const int stprP2 = 4;//stepper pin P2
const int pushPin = 5;//to control stepper direction
const int dirPin = 7;//controls DC motor direction 
const int spkrPin = 8;//Piezo speaker for tone
const int srvPin = 9;//servo comtrol pulses

Stepper stepper1(60, stprP1,stprP2);//60 steps per revolution
Servo servo1;

char backval;
byte count;
volatile byte tics;
byte digital_level;
int analog_level;
byte numchars;
int angle=90;

void wheelTic();//interrupt routine prototype

void setup()
{
	count=0;
	//set up SPI
	SPI.setClockDivider(SPI_CLOCK_DIV32);
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE1);//Slave Mode MUST agree 
	SPI.begin();//BEFORE driving slave SS pin LOW 
	//pinMode(spiSS, OUTPUT);//optional --ALREADY DONE BY SPI.begin()
	digitalWrite(spiSS, LOW);//enable attached SPISlave

	pinMode(dirPin, OUTPUT);//SO CAN DRIVE LED (internal PULLUP is TOO WEAK)
	digitalWrite(dirPin, HIGH);//init	
	Serial.begin(300);

	//stepper1.setSPeed(10);//TYPO PREVENTS MATCH
	stepper1.setSpeed(10);//10 RPM
	//correct foir Stepper.h omission of first step initialization
	//make sure stepper is in reference 12-o'clock position 
	//pinMode(stprP1, OUTPUT);//optional --ALREADY DONE BY Stepper CONSTRUCTOR ABOVE
	//pinMode(stprP2, OUTPUT);//optional --ALREADY DONE BY Stepper CONSTRUCTOR ABOVE
	digitalWrite(stprP1, LOW); //for step#1
	digitalWrite(stprP2, HIGH);//for step#1
	delay(10);//for stepper to move to 12-o'clock	
	servo1.attach(srvPin);//attach servo1

	attachInterrupt(0,wheelTic, CHANGE);//ANY-edge interrupts on pin 2
	interrupts();//enable interrupt
	tics = 0;//initialize interrupt tics counter
}

void loop()
{
	//send count value to SPISLave device as a character byte
	backval = SPI.transfer(count);

	//print count on sser1
	numchars=Serial.println(count);

	//advance servo1 angle
	if(analogRead(A0) < 511)	
	{	angle = angle + 20;
		if(angle>180)
		{	tone(spkrPin, 880, 50);//Beep
			angle = 0;
		}
	}
	else
	{	angle = angle - 20;
		if(angle<0)
		{	tone(spkrPin, 880, 50);//Beep
			angle = 180;
		}
	}
	servo1.write(angle);

	//advance stepper1 by 10 steps
	if(digitalRead(pushPin))	
		stepper1.step(10);//step FORWARD
	else
		stepper1.step(-10);//step in REVERSE

	//check anlaog level on A2
	analog_level = analogRead(A2);

	//advance loop count
	count=count+1;

	//check wheel encoder tics to reverse around every four revolutions
	if(tics >= 4*TICSPERREV)
	{	//four full wheel revolutions
		tics = 0;//reset counter
		digital_level = digitalRead(dirPin);
		digitalWrite(dirPin, 1-digital_level);//toggles pin level
	}

	delay(233);//some loop delay() as normal
}

void wheelTic()
{
	tics++;//for use in loop()
}

