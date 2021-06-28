// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
    Name:       programa curtis.ino
    Created:	2/12/2019 14:44:03
    Author:     Armagedon13
*/

// Define User Types below here or use a .h file
//
#include <Temporizador.h>
Temporizador temp;
Temporizador temp1;
Temporizador temp2;

//setea el sentido de movimiento
const int entrada_sentido = 10;
//salida de 5v a p6
const int p6 = 5;
const int adelante = 2;
const int atras = 3;
const int paso = 11;
//const int PDE = 12;
int seleccion = 0;
int contador;
int caca;
//int pancho;


// Define Function Prototypes that use User Types below here or use a .h file
//


// Define Functions below here or use other .ino or cpp files
//

// The setup() function runs once each time the micro-controller starts
void setup()
{
	Serial.begin(9600);
	pinMode(entrada_sentido, INPUT);
	pinMode(p6, OUTPUT);
	pinMode(adelante, OUTPUT);
	pinMode(atras, OUTPUT);
	pinMode(paso, INPUT);
	pinMode(13, OUTPUT);
  //pinMode(PDE, INPUT_PULLUP);

	//inicio
	digitalWrite(adelante, LOW);
	digitalWrite(atras, LOW);
	digitalWrite(p6, LOW);

	//temporizadores
	temp.temporizar(0, 0, 0, 1, 0);
	temp1.temporizar(0, 0, 0, 1, 0);
	temp2.temporizar(0, 0, 0, 0, 500);
  delay(1000);
}

// Add the main program code into the continuous loop() function
void loop()
{
	int sentido = digitalRead(entrada_sentido);
	int paso_estado = digitalRead(paso);
 // int PDE_estado = digitalRead(PDE);
	 
	if (sentido == LOW && caca == 1 && contador == 0) {
		digitalWrite(adelante, HIGH);
		Serial.println("adelante completado");
		digitalWrite(p6, HIGH);
		contador = 1;
		temp.temporizar(0, 0, 0, 1, 0);
		digitalWrite(13, HIGH);

	}
	if (sentido == HIGH && caca == 1 && contador == 0) {
		digitalWrite(atras, HIGH);
		Serial.println("atras completado");
		digitalWrite(p6, HIGH);
		contador = 2;
		temp1.temporizar(0, 0, 0, 1, 0);
		digitalWrite(13, LOW);
	}
	if (paso_estado == LOW) {
		digitalWrite(adelante, HIGH);
		digitalWrite(atras, HIGH);
		digitalWrite(p6, LOW);
		contador = 0;
		Serial.println("apagado");
		caca = 0;
		temp2.temporizar(0, 0, 0, 0, 500);

	}
	if (paso_estado == HIGH) {
		if (temp2.completado()) {
			caca = 1;
		}
	}

	if (contador == 1) {
		if (temp.completado()) {
			digitalWrite(adelante, LOW);
			Serial.println("adelante encendido");
			contador = 3;
		}
	}
	if (contador == 2) {
		if (temp1.completado()) {
			digitalWrite(atras, LOW);
			Serial.println("atras encendido");
			contador = 3;
		}
	}
 /*if (PDE_estado==LOW){
    pancho = 1;
  }
 if (PDE_estado == HIGH && pancho == 1){
     digitalWrite(adelante, LOW);
     digitalWrite(atras, LOW);
     digitalWrite(p6, LOW);
     pancho=0;
     }*/
 
	Serial.println(caca);
}
