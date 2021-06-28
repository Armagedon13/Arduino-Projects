#include <Arduino.h>

#include <font4x6.h>
#include <font6x8.h>
#include <font8x8.h>
#include <font8x8ext.h>
#include <fontALL.h>
#include <TVout.h>
#include <video_gen.h>

TVout TV;

// this constant won't change:
const int  buttonPin = 3;    // pin de entrada
const int ledPin = 13;       // pin salida led

// Variables will change:
int buttonPushCounter = 0;   // numero de botones presionados
int buttonState = 0;         // estado del boton
int lastButtonState = 0;     // estado anterior del boton

void setup() {
  TV.begin(PAL,120,96);
  TV.select_font(font8x8);             //seleccionar fuente y tamaño
 TV.print(9,44,"Service Naval");     // comando printin pone lo escrito arriba sin modificar
 TV.delay(2500);
 TV.clear_screen();
  // inicia el boton como salida
  pinMode(buttonPin, INPUT_PULLUP); //Resistencia de pullup interna
  // initialize the LED as an output:
  pinMode(ledPin, OUTPUT);
  // iniciar comunicacion serial:
  Serial.begin(9600);
 
}


void loop() {
  // lee la salida el boton
  buttonState = digitalRead(buttonPin);
  
  //compara la salida del boton con la ultima salida
  if (buttonState != lastButtonState) {
    // si el estado cambio, aumenta el mismo
    if (buttonState == HIGH) {
      // 
      buttonPushCounter++;
      Serial.print("number of button pushes:  ");
      Serial.println(buttonPushCounter);
    } 
    else {
      // 
      Serial.println("off"); 
    }
  }
  // guarda el ultimo cambio
  //para hacer el loop del programa
  lastButtonState = buttonState;

  
  
  // turns on the LED every four button pushes by 
  //esto no lo entendi muy bien
  // checking the modulo of the button push counter.
  // the modulo function gives you the remainder of 
  // the division of two numbers:
  if (buttonPushCounter % 4 == 0) {
    digitalWrite(ledPin, HIGH);
  } else {
   digitalWrite(ledPin, LOW);
  }
  //conteo a 10 y reset no sirve a mi proposito
if(buttonState){
  TV.println( 9, 44, buttonPushCounter);}
  if(buttonPushCounter == 10)
  (buttonPushCounter = lastButtonState);


}
  
