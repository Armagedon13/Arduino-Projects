#include <Temporizador.h>
Temporizador temp;
Temporizador temp2;
Temporizador temp3;
Temporizador temp4;

int entrada = 2;
int led = 3;
int estadoPulsador;
bool paso1 = false;
bool paso2 = false;
int contador;

void setup() {
  
pinMode(entrada,INPUT);
pinMode(led, OUTPUT);

temp.temporizar(0,0,0,0,8);
temp2.temporizar(0,0,0,0,100);
temp3.temporizar(0,0,0,0,8);
temp4.temporizar(0,0,0,0,100);

}

void loop() {

estadoPulsador = digitalRead(entrada);

  if (estadoPulsador == LOW && paso1 == false){
    temp.temporizar(0,0,0,0,8);
    contador = 1;}
    if(contador == 1){
      if(temp.completado()){
          digitalWrite(led, HIGH);
          temp2.temporizar(0,0,0,0,100);
          contador = 2;}}
          if(contador == 2){
            if(temp2.completado()){
            digitalWrite(led, LOW);
          paso1=true;
          paso2=true;
          contador = 3;
  }}

  if (estadoPulsador == HIGH && paso2==true){ 
    temp3.temporizar(0,0,0,0,8);
    contador = 4;}
    if(contador == 4){
      if(temp3.completado()){
          digitalWrite(led, HIGH);
          temp4.temporizar(0,0,0,0,100);
          contador = 5;}}
          if(contador == 5){
            if(temp4.completado()){
            digitalWrite(led, LOW);
          paso1=false;
          paso2=false;
          contador = 5;    
  }}


}



