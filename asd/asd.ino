// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
    Name:       asd.ino
    Created:	5/12/2019 11:34:10
    Author:     ARMAGEDON13\Armagedon13
*/

// Define User Types below here or use a .h file
//
const int entrada_sentido = 8;

// Define Function Prototypes that use User Types below here or use a .h file
//


// Define Functions below here or use other .ino or cpp files
//

// The setup() function runs once each time the micro-controller starts
void setup()
{
	pinMode(entrada_sentido, INPUT);
	pinMode(13, OUTPUT);
}

// Add the main program code into the continuous loop() function
void loop()
{
	int sentido = digitalRead(entrada_sentido);

	if (sentido == LOW) {
		digitalWrite(13, HIGH);

	}
	if (sentido == HIGH) {
		digitalWrite(13, LOW);
	}
}
