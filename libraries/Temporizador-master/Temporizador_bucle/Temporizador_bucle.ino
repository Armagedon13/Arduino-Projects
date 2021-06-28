// Temporizador  "Ejemplo de uso".
//Creado por Mandragora Tools 2016
//Especial agradecimiento a Alfaville, Lucario448  y surbyte
//web: http://mandragoratools.xyz

// "En este ejemplo usamos la libreria para repetir una accion determinada al transcurrir el tiempo indicado."

//Este archivo es de dominio publico, puedes usarlo cuando y como quieras.

#include <Temporizador.h>

Temporizador temp; //-----> Declaramos una instancia  del temporizador

void setup() {

pinMode(13,OUTPUT); //-----> Declaramos el pin 13 como salida

temp.temporizar(0,0,0,1,0);//-----> Iniciamos el temporizador por primera ves en este caso a un segundo

}

void loop() {

//comprobamos si el tiempo definido ha transcurrido
if(temp.completado()){ 
  digitalWrite(13,!digitalRead(13)); //-----> En este caso al transcurrir el tiempo indicado invertimos el estado del pin 13
  temp.temporizar(0,0,0,5,0); //-----> volvemos a iniciar el temporizador para que se repita de nuevo en modo bucle cada 5 segundos
}


}
