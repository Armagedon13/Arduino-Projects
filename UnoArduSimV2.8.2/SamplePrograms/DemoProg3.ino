/* 
	SA smple prorgam to demonstarte UnoArduSim
	Do Confgiure|'I/O' Devices and load 
	SampleProgram/myUnoDevsDemoProg3.txt before running

*/
int count;
volatile int numpulses;

const int pinTRIG = 2;
const int pin1SHOT1 = 3;
const int pinPUSH = 4;
const int pin1SHOT2 = 5;

void OneShotPulse();

void setup()
{
	count=0;
	numpulses=0;
	pinMode(pinTRIG, OUTPUT);
	attachInterrupt(0, OneShotPulse, FALLING);
}

void loop()
{
	delay(100);
	if(count%10==0)
		digitalWrite(pinTRIG, HIGH);//rising TRIGGER EDGE
	else
		digitalWrite(pinTRIG, LOW);//reset TRIGGER LEVEL
	count=count+1;
}

void OneShotPulse()
{	numpulses++;
}
