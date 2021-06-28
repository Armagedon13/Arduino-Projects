#include <Arduino.h>

//libreria TV
#include <TVout.h>
#include <video_gen.h>
#include <font4x6.h>
#include <font6x8.h>
#include <font8x8.h>
#include <font8x8ext.h>
#include <fontALL.h>

//Cargo la librería math.h y defino B y R0 (necesarios para leer la temperatura)
#include <math.h>
const int B = 1000;                // B valor del termistor
const int R0 = 100000;            // R0 = 100k el mio es de 5k

const int pinTemp = A1;     // Grove - Sensor de temperature conectado a A0

float temp, temp_med = 0; // Temperatura en ºC
float acum = 0, Vout; // variables secundarias
float res = 0.010; // en V/ºC
int i, num_muestras = 100;

int o;
float t;

TVout TV; //tv set up

void setup()
  { 
     //comienzo tv
     Serial.begin(9600);               // señal serial
     pinMode(pinTemp, INPUT);          //entrada pin analogica sensor
     TV.begin(PAL,120,96);             //tipo de tv
  TV.select_font(font8x8);             //seleccionar fuente y tamaño
 TV.print(9,44,"Service Naval");       // comando printin pone lo escrito arriba sin modificar
 TV.delay(2500);                       //tiempo en pantalla
 TV.clear_screen();                    //borra estado actual de la pantalla
     
  }
void loop()
{
  //incio del calculo de la temperatura

  acum = 0;
  temp_med = 0;
  //Calculamos primero para el Lm35C
  for (i = 0; i < num_muestras; i++) {
    Vout = analogRead(pinTemp) * 0.00488758553275; // step * V/step = V
    temp = Vout / res; // V/(V/ºC) = ºC
    acum += temp;
    temp_med = acum / num_muestras;
    delay(10); // Al tener 10 milisegundos y tener 100 muestras tardara 1 segundo en hacer el bucle.

  }
  
 Serial.print(o);
 Serial.println(" ºC");   //Fin del calculo de la tempertura
 t = temp_med;
 o = (int) t; // ahora i es 3
  TV.println(0,70,o); //muestra de C° en tv
}

