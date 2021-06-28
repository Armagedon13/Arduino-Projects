// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
    Name:       programa curtis.ino
    Created:	2/12/2019 14:44:03
    Author:     Armagedon13
*/

// Define User Types below here or use a .h file
//
#include <avr/wdt.h>

#include <Temporizador.h>
Temporizador temp;
Temporizador temp1;
Temporizador temp2;

//setea el sentido de movimiento
const int entrada_sentido = A0;
//salida de 5v a p6
const int p6 = 5;
const int adelante = 2;
const int atras = 3;
const int paso = 11;
const int PDE = 12;
int seleccion = 0;
int contador;
int caca;
int pancho;


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
  pinMode(PDE, INPUT);

	//inicio
	digitalWrite(adelante, LOW);
	digitalWrite(atras, LOW);
	digitalWrite(p6, LOW);
  
  wdt_disable(); // Desactiva la función mientras se configura el tiempo en el que se reseteara
  

	//temporizadores
	temp.temporizar(0, 0, 0, 1, 0);
	temp1.temporizar(0, 0, 0, 1, 0);
	temp2.temporizar(0, 0, 0, 0, 500);
  delay(1000);
}

// Add the main program code into the continuous loop() function
void loop()
{
	int sentido_analog = analogRead(entrada_sentido);
	int paso_estado = digitalRead(paso);
  float sentido = sentido_analog * (5.0 / 1023.0);
  int PDE_estado = digitalRead(PDE);
	 
	if (sentido < 1 && caca == 1 && contador == 0) {
		digitalWrite(adelante, HIGH);
		Serial.println("adelante completado");
		digitalWrite(p6, HIGH);
		contador = 1;
		temp.temporizar(0, 0, 0, 1, 0);
		digitalWrite(13, HIGH);

	}
	if (sentido > 1 && caca == 1 && contador == 0) {
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
 if (PDE_estado==HIGH){
    pancho = 1;
   wdt_reset(); // Actualizar el watchdog para que no produzca un reinicio
  }
 if (PDE_estado == LOW && pancho == 1){
     wdt_enable(WDTO_1S); // Configuramos el contador de tiempo para que se reinicie en 2s
     pancho=0;
     }
 
	Serial.println(sentido);
}
