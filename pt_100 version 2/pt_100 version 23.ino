
#include <ResponsiveAnalogRead.h>

int sensorPin1=A0;

float milivolts;

float celsius;

int value;

 ResponsiveAnalogRead analogOne(A0, true);

void setup() {
	Serial.begin(9600);
pinMode(A0,INPUT);

}

void loop() {

    value = analogOne.getValue();
	milivolts = (value / 1023.0) * 5000;
	celsius = milivolts / 10; 

Serial.println(milivolts);

}