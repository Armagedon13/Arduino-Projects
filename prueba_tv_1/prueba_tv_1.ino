#include <Arduino.h>

//librerias primarias
#include <TVout.h>
#include <video_gen.h>
#include <fontALL.h>
#include <font4x6.h>
#include <font6x8.h>
#include <font8x8.h>
#include <font8x8ext.h>

//designar variables
TVout TV;

int zOff = 150;
int xOff = 0;
int yOff = 0;
int cSize = 50;
int view_plane = 64;
float angle = PI/60;


//comienza y no se repite
void setup() {
  //pantalla de inicio
 TV.begin(PAL,120,96);                 //declarar tipo de video
 TV.select_font(font8x8);              //seleccionar fuente y tamaño
 TV.print(0,0,"Equipo vigia\npuesta marcha motor");     
 TV.select_font(font8x8);
 TV.print(0,40, "T.N.A");
 TV.print(0,80,"Power by\nService Naval\nArgentina");
 TV.delay(10000);                     // delay cuenta el tiempo de cuanto esta en pantalla
    TV.clear_screen();                // pone la pantalla en negro
 
 //pantalla de variables
 TV.select_font(font4x6);
 TV.print(0,0,"velocidad motor:");
 TV.print(0,10,"temperatura agua:");      
 TV.print(0,20,"corte agua:");
 TV.print(0,30,"temperatura corte agua:");
 TV.print(0,40,"presion corte aceite:"); 
 TV.print(0,50,"maxima velocidad motor:"); 
 //
 
 
  

}
// repite los procesos
void loop() {

 
}
