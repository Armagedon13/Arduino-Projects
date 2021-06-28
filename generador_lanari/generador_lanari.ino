#include <Temporizador.h>
Temporizador temp;

int pote= A1; 
int caca = 2;



void setup() {
  pinMode(A1,INPUT);
  pinMode(caca,OUTPUT);
  temp.temporizar(0,0,0,5,0);
  Serial.begin(9600);
}

void loop() {
  int poteValue= analogRead(pote);
  int timer = map(poteValue,0,1023,1,6);
  temp.temporizar(0,0,0,timer,0);
  if(temp.completado()){
    digitalWrite(caca,HIGH);
  }
  else{
     digitalWrite(caca,LOW);
     temp.temporizar(0,0,0,timer,0);
  }
  //Serial.println(timer);
  Serial.println(digitalRead(caca));
}
