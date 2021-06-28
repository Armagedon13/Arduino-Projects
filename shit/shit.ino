/*
    Name:       shit.ino
    Created:  25/11/2019 11:14:27
    Author:     Armagedon13
*/

// Define User Types below here or use a .h file
#include <Temporizador.h>
Temporizador temp;
Temporizador temp1;

//setea el freno
const int salida_freno = 2;
//setea el sentido de movimiento
const int entrada_sentido = 8;
//salida de 5v a p6
const int p6 = 6;
const int adelante = 3;
const int atras = 4;
const int paso = 9;
int seleccion = 0;
int contador;

// Define Function Prototypes that use User Types below here or use a .h file
//


// Define Functions below here or use other .ino or cpp files
//

// The setup() function runs once each time the micro-controller starts
void setup()
{
  Serial.begin(9600);
  pinMode(salida_freno, OUTPUT);
  pinMode(entrada_sentido, INPUT);
  pinMode(p6, OUTPUT);
  pinMode(adelante, OUTPUT);
  pinMode(atras, OUTPUT);
  pinMode(paso, INPUT_PULLUP);

  temp.temporizar(0, 0, 0, 1, 0);
  temp1.temporizar(0, 0, 0, 1, 0);
}

// Add the main program code into the continuous loop() function
void loop()
{
  int sentido = digitalRead(entrada_sentido);
  int p6_estado = digitalRead(p6);
  int paso_estado = digitalRead(paso);

  if (sentido == HIGH && paso_estado == LOW && contador == 0) {
	  digitalWrite(adelante, HIGH);
	  //digitalWrite(atras, LOW);
	  Serial.println("adelante completado");
	  digitalWrite(p6, HIGH);
	  contador = 1;
	  temp.temporizar(0, 0, 0, 1, 0);

  }
  if (sentido==LOW && paso_estado == LOW && contador == 0) {
	  //digitalWrite(adelante, LOW);
	  digitalWrite(atras, HIGH);
	  Serial.println("atras completado");
	  digitalWrite(p6, HIGH);
	  contador = 2;
	  temp1.temporizar(0, 0, 0, 1, 0);
  }
  if (paso_estado == HIGH) {
	  digitalWrite(adelante, LOW);
	  digitalWrite(atras, LOW);
	  digitalWrite(p6, LOW);
	  contador = 0;
	  

  }
  if (contador == 1) {
	  if(temp.completado()) {
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
  //Serial.println(paso_estado);
}