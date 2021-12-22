/* 
	SA smple prorgam to demonstarte UnoArduSim
	Do Confgiure|'I/O' Devices and load 
	SampleProgram/myUnoDevsDemoProg2.txt before running

*/

#include "MusicalNoteData.h"
#include <Servo.h>

#define NUMLEDS 6

const int spkrPin1=8;
const int servoPins[]={9,10,11,12};

typedef struct
{
	int frequency;		//the note's frequency in Hz
	float duration; 	//a mutiplier for the note's duration
} Note;

const int notesF[] = {fC6,fA5,fP,fC6,fA6,fP,fC6,fA5,fC6,fA5,fC6,fA6,fP,fC6,fD6,fE6,
fP,fE6,fF6,fG6,fP,fD6,fC6,fD6,fF6,fAs6,fA6,fC7,0};

const float notesD[] = {N8,N8,N4,N8,N8,N4,N8,N8,N8,N8,N8,N8,N2,N8,N8,N8,N8,N8,N8,N8,N4,N8,N8,
N4,N8,N4,N4,N2,0 };

Servo my_servos[4];
int servo_index, LED_index, LED_pin=2, angle;


void setup()
{
	int i, k, notefreq;
	float dur;
	int beats; //beats per minutes (one beat = one quater-note)
	long wholenote, quarternote;//duration of whole and quarter note in millisecond tics
	int msecs; //duration in milliseconds 
	Note RingTones[50];
	
	for(k=0; k<=3;k++)
	{	my_servos[k].attach(servoPins[k], 500, 2500);
		my_servos[k].write(90);//START EACH at 90 degress
		pinMode(2 + k, OUTPUT);
	}
	delay(100);
	//make LED pins OUPUT
	for(k=0; k<=NUMLEDS-1;k++)
		pinMode(2 + k, OUTPUT);
	
	i = 0;
	do
	{
		RingTones[i].frequency = notesF[i];
		RingTones[i].duration = notesD[i];
		i= i+1;
	} while (notesD[i-1] != 0); //the last note has duration value 0
	
	beats = 160;
	//NOTE that long() cast of ONE of the TWO numeeric literals being multiplied
	//next is CRITICAL to avoid overflow of Arduino 'int' default literal sizing
	quarternote = (long(1000)*60)/beats;//how many milliseconds
	
	wholenote = 4*quarternote; //how many milliseconds
	
	i=0;
	servo_index = 0;
	LED_index = 0;
	digitalWrite(13, HIGH);//MOTORS ON AT 100%
	do
	{	notefreq = RingTones[i].frequency;
		dur = RingTones[i].duration;
		msecs = int(wholenote*dur);
		tone(spkrPin1, notefreq, msecs); //includes "rest note" of freq=0!!
		angle = my_servos[servo_index].read();
		if(angle==90)
			angle=45;
		else if(angle==45)
			angle=90;
		my_servos[0].write(angle);
		my_servos[1].write(180-angle);
		my_servos[2].write(angle);
		my_servos[3].write(180-angle);
		digitalWrite(LED_pin, HIGH);
		delay(msecs);//let tone finish
		digitalWrite(LED_pin, LOW);
		i = i+1;
		servo_index = 1-servo_index;//flip servo index
		LED_index = (LED_index+1)%NUMLEDS;//wrap back
		LED_pin = 2 + LED_index;
	} while (dur != 0); //dur = 0 flags end of song
	digitalWrite(13, LOW);//MOTORS OFF
	
}

void loop()
{
	delay(100);
}