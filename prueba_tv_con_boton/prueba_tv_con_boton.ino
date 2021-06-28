#include <Arduino.h>

//librerias primarias
#include <TVout.h>
#include <video_gen.h>
#include <fontALL.h>
#include <font4x6.h>
#include <font6x8.h>
#include <font8x8.h>
#include <font8x8ext.h>
#include <Temporizador.h>

//designar variables
TVout TV;

Temporizador temp;

int zOff = 150;
int xOff = 0;
int yOff = 0;
int cSize = 50;
int view_plane = 64;
float angle = PI/60;

const int pinLed =13; //EL PIN DEL LED
const int pinBtn =2;

int encender =0;
int anterior = 0;
int estado = 0;

//comienza y no se repite
void setup() {
  
  pinMode(pinLed, OUTPUT);
  pinMode(pinBtn, INPUT);
 
  //pantalla
 TV.begin(PAL,120,96);                 //declarar tipo de video
 TV.select_font(font8x8);             //seleccionar fuente y tamaño
 TV.print(9,44,"Service Naval");     // comando printin pone lo escrito arriba sin modificar
 TV.delay(2500);                     // delay cuenta el tiempo de cuanto esta en pantalla
    TV.clear_screen();               // pone la pantalla en negro

 TV.select_font(font6x8);
 TV.println("Prueba numero 1");
 TV,delay(2500);  

  
  temp.temporizar(0,0,0,5,0);
}


void loop()
{
  estado = digitalRead(pinBtn); // Comprobamos el estado
  if(estado && anterior ==0) //Comprobar el pulsador
  {
    encender = 1 - encender;// asignamos el estado contrario
    delay(30);
  }
   TV.select_font(font4x6);
  anterior = estado;
  if(encender){
  digitalWrite(pinLed, HIGH);
   TV.print(95,45,"ALARMA");}
  else{
  digitalWrite(pinLed, LOW);
  TV.print(95,30,"ALARMA");}
}

