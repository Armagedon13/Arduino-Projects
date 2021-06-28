#include <Temporizador.h>

Temporizador temp;
Temporizador tempA;
Temporizador temp1;
Temporizador temp2;
Temporizador temp3;
Temporizador temp4;
Temporizador temp5;
Temporizador temp6;
Temporizador temp7;


Temporizador temp8;
Temporizador temp9;
Temporizador temp10;
Temporizador temp11;
Temporizador temp12;
Temporizador temp13;

//inicio Sistema
const byte inicioSistema = 17;
int estadoInicioSistema;

//apagado Sistema
const byte apagadoSistema = 19;
int estadoApagadoSistema;

//parada Emergencia
const byte paradaEmergencia = A6;
int estadoPdE;

//cierre Interruptor Generador 2
//const byte cierreInterruptorGE2 = 16;
//int estadoCierreInterruptorGE2;

//apertura INT principal
const byte aperturaInterruptorPrincipal = 2;
int estadoINTprincipal;
int INTprincipal = 0;

//apertura INT GE1
const byte aperturaInterruptorGE1 = 3;
int estadoAperturaINTGE1;

//cierre INT GE1
const byte cierreInterruptorGE1 = 4;
int estadoCierreINTGE1;

//apertura INT GE2
const byte aperturaInterruptorGE2 = 5;
int estadoAperturaINTGE2;

//Piloto luminoso cierre INT GE1
const byte pilotoLuminosoCierreInterruptorGE1 = 6;
int estadoPilotoCierreINTGE1;

//piloto lminoso cierre INT GE2
//const byte pilotoLuminosoCierreInterruptorGE2 = 7;
//int estadoPilotoCierreINTGE2;

//piloto luminoso sistema encendido
const byte pilotoLuminosoSistemaEncendido = 7;
int estadoPilotoSistemaEncendido;

//piloto luminoso parada de emergencia activada
const byte pilotoLuminosoPdEActivada = 8;
int estadoPilotoPdEActivada;

//encendido solenoide GE1 Aire
const byte encendidoSolenoideGE1Aire = 9;
int estadoEncendidoSolenoideGE1Aire;

//encendido solenide GE2 Aire
const byte encendidoSolenoideGE2Aire = 10;
int estadoEncendidoSolenoideGE2Aire;

//apagado generador 1
const byte apagadoGE1 = 14;
int estadoApagadoGE1;

//apagado generador 2
const byte apagadoGE2 = 15;
int estadoApagadoGE2;

//contactor auxiliar
//const byte contactorAuxiliar = 15;
//int estadoContactorAuxiliar;


int contador = 0;

//se repite una vez
void setup(){

Serial.begin(115200);
  
//declaracion de pines
pinMode(inicioSistema, INPUT);
pinMode(apagadoSistema, INPUT);
pinMode(paradaEmergencia, INPUT);
//pinMode(cierreInterruptorGE2, INPUT);
pinMode(aperturaInterruptorPrincipal, OUTPUT);
pinMode(aperturaInterruptorGE1, OUTPUT);
pinMode(aperturaInterruptorGE2, OUTPUT);
pinMode(pilotoLuminosoCierreInterruptorGE1, OUTPUT);
//pinMode(pilotoLuminosoCierreInterruptorGE2, OUTPUT);
pinMode(pilotoLuminosoSistemaEncendido, OUTPUT);
pinMode(pilotoLuminosoPdEActivada, OUTPUT);
pinMode(encendidoSolenoideGE1Aire, OUTPUT);
pinMode(encendidoSolenoideGE2Aire, OUTPUT);
pinMode(apagadoGE1, OUTPUT);
pinMode(apagadoGE2, OUTPUT);
//pinMode(contactorAuxiliar, OUTPUT);

//estado encendido RELE
digitalWrite(aperturaInterruptorPrincipal,HIGH);
digitalWrite(pilotoLuminosoSistemaEncendido,HIGH);  
digitalWrite(encendidoSolenoideGE1Aire,HIGH);
digitalWrite(encendidoSolenoideGE2Aire,HIGH);
digitalWrite(aperturaInterruptorGE1,HIGH);
//digitalWrite(aperturaInterruptorGE2,HIGH);
digitalWrite(pilotoLuminosoCierreInterruptorGE1,HIGH);
//digitalWrite(pilotoLuminosoCierreInterruptorGE2,HIGH);
digitalWrite(pilotoLuminosoPdEActivada,HIGH);
digitalWrite(apagadoGE1,HIGH);
digitalWrite(apagadoGE2,HIGH);
//digitalWrite(contactorAuxiliar,HIGH);

//temporizadores encendido
temp.temporizar(0,0,0,2,0);
tempA.temporizar(0,0,0,2,0);
temp1.temporizar(0,0,0,3,0);
temp2.temporizar(0,0,0,4,0);
temp3.temporizar(0,0,0,4,0);
temp4.temporizar(0,0,0,7,0);
temp5.temporizar(0,0,0,4,0);
temp6.temporizar(0,0,0,9,0);
temp7.temporizar(0,0,0,10,0);

//temporizadores apagado
temp8.temporizar(0,0,0,2,0);
temp9.temporizar(0,0,0,4,0);
temp10.temporizar(0,0,0,4,0);
temp11.temporizar(0,0,0,2,0);
temp12.temporizar(0,0,0,2,0);
temp13.temporizar(0,0,0,4,0);

}

//ser repite infinitamente
void loop() {

//estados Botones--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 estadoInicioSistema = digitalRead(inicioSistema);
 estadoApagadoSistema = digitalRead(apagadoSistema);
 estadoPdE = analogRead(paradaEmergencia);
 //estadoCierreInterruptorGE2 = digitalRead(cierreInterruptorGE2);
 estadoINTprincipal = digitalRead(aperturaInterruptorPrincipal);
 estadoAperturaINTGE1 = digitalRead(aperturaInterruptorGE1);
 estadoAperturaINTGE2 = digitalRead(aperturaInterruptorGE2);
 estadoPilotoCierreINTGE1 = digitalRead(pilotoLuminosoCierreInterruptorGE1);
 //estadoPilotoCierreINTGE2 = digitalRead(pilotoLuminosoCierreInterruptorGE2);
 estadoPilotoSistemaEncendido = digitalRead(pilotoLuminosoSistemaEncendido);
 estadoPilotoPdEActivada = digitalRead(pilotoLuminosoPdEActivada);
 estadoEncendidoSolenoideGE1Aire = digitalRead(encendidoSolenoideGE1Aire);
 estadoEncendidoSolenoideGE2Aire = digitalRead(encendidoSolenoideGE2Aire);
 estadoApagadoGE1 = digitalRead(apagadoGE1);
 estadoApagadoGE2 = digitalRead(apagadoGE2);
// estadoContactorAuxiliar = digitalRead(contactorAuxiliar);


//encendido general****************************************************************************************************************************************************************
 if(estadoInicioSistema == HIGH){
  if(temp.completado()){
    digitalWrite(aperturaInterruptorPrincipal,LOW);
    //digitalWrite(pilotoLuminosoSistemaEncendido,LOW);
    temp1.temporizar(0,0,0,3,0);
    tempA.temporizar(0,0,0,2,0);
    contador = 1;
    Serial.println("enciendo sistema");
  }}

if(tempA.completado()){
 //else{
  temp.temporizar(0,0,0,2,0);
  tempA.temporizar(0,0,0,2,0);
  digitalWrite(aperturaInterruptorPrincipal,HIGH);
 }  
 
//encendido piloto luminoso sistema-----------------------------------------------------------------------------------------------------------------
if(contador==1){
 if(temp1.completado()){
    digitalWrite(pilotoLuminosoSistemaEncendido,LOW);
    temp2.temporizar(0,0,0,4,0);
contador=2;
    Serial.println("piloto apagado");
    }}

//encendido solenoide aire GE1 ---------------------------------------------------------------------------------------------------------------------   
if(contador==2){
 if(temp2.completado()){      
  digitalWrite(encendidoSolenoideGE1Aire,LOW);
  temp3.temporizar(0,0,0,4,0);
  contador=3;
  Serial.println("solenoide aire GE1");
    }}
if(contador==3){ 
if(temp3.completado()){
   digitalWrite(encendidoSolenoideGE1Aire,HIGH);
    temp4.temporizar(0,0,0,7,0);
    contador =4;
    Serial.println("solenoide aire GE1 apagado");
}}

//encendido solenoide aire GE2------------------------------------------------------------------------------------------------------------------------
if(contador==4){
 if(temp4.completado()){      
  digitalWrite(encendidoSolenoideGE2Aire,LOW);
  temp5.temporizar(0,0,0,4,0);
  contador=5;
  Serial.println("solenoide aire GE2");
 }}

if(contador==5){ 
if(temp5.completado()){
   digitalWrite(encendidoSolenoideGE2Aire,HIGH);
   temp6.temporizar(0,0,0,9,0);
   contador=6;
   Serial.println("solenoide aire GE2 apagado");
   
}}

//cierre interruptor GE1-------------------------------------------------------------------------------------------------------------------------------
if(contador==6){
if(temp6.completado()){
 digitalWrite(aperturaInterruptorGE1,LOW);
 digitalWrite(pilotoLuminosoCierreInterruptorGE1,LOW);
 temp7.temporizar(0,0,0,10,0);
 contador=7;
 Serial.println("interruptor GE1");
}}

/*//cierre interruptor GE2--------------------------------------------------------------------------------------------------------------------------------
if(estadoCierreInterruptorGE2 == HIGH && contador == 7){
  digitalWrite(aperturaInterruptorGE2,LOW);
  //digitalWrite(pilotoLuminosoCierreInterruptorGE2,LOW);
  contador =8;
}*/


//apagado sistema********************************************************************************************************************************************************************
  
if(estadoApagadoSistema == HIGH){
temp8.temporizar(0,0,0,2,0);
contador=9;}
if(contador==9){
 if(temp8.completado()){
  digitalWrite(aperturaInterruptorGE2,HIGH);
  //digitalWrite(pilotoLuminosoCierreInterruptorGE2,HIGH);
  temp9.temporizar(0,0,0,4,0);
  contador = 10;
  
}}

if(contador == 10){
  if(temp9.completado()){
  digitalWrite(aperturaInterruptorGE1,HIGH); 
  digitalWrite(pilotoLuminosoCierreInterruptorGE1,HIGH);
  temp10.temporizar(0,0,0,4,0);
  contador = 11;
  }  
}

if(contador == 11){
  if(temp10.completado()){
    digitalWrite(apagadoGE2,LOW);
    temp11.temporizar(0,0,0,2,0);
  contador=12; 
  }
}
if(contador==12){
  if(temp11.completado()){
digitalWrite(apagadoGE1,LOW);
temp12.temporizar(0,0,0,2,0);
contador=13;
  }
}

if(contador==13){
  if(temp12.completado()){
    digitalWrite(pilotoLuminosoSistemaEncendido,HIGH);
    digitalWrite(apagadoGE1,HIGH);
    digitalWrite(apagadoGE2,HIGH);
    contador = 0;
  }
}

//apagado emergencia *********************************************************************************************************************************************************************
if(estadoPdE > 511){
   digitalWrite(aperturaInterruptorGE2,HIGH);
  //digitalWrite(pilotoLuminosoCierreInterruptorGE2,HIGH);
   digitalWrite(aperturaInterruptorGE1,HIGH); 
  digitalWrite(pilotoLuminosoCierreInterruptorGE1,HIGH);
  digitalWrite(pilotoLuminosoSistemaEncendido,HIGH);
    digitalWrite(apagadoGE1,LOW);
    digitalWrite(apagadoGE2,LOW);
    digitalWrite(pilotoLuminosoPdEActivada,LOW);
    temp13.temporizar(0,0,0,4,0);
    contador=14;
}
else{
  digitalWrite(pilotoLuminosoPdEActivada,HIGH);
}
if(contador==14){
  if(temp13.completado()){
   digitalWrite(apagadoGE1,HIGH);
    digitalWrite(apagadoGE2,HIGH);
    contador =0;
}}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Serial.println(contador);




}
