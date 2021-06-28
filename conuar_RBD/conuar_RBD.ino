#include <RBD_Timer.h>

RBD::Timer timer;
RBD::Timer timer1;
RBD::Timer timer2;
RBD::Timer timer3;
RBD::Timer timer4;
RBD::Timer timer5;
RBD::Timer timer6;
RBD::Timer timer7;


//inicio Sistema
const byte inicioSistema = 17;
int estadoInicioSistema;

//apagado Sistema
const byte apagadoSistema = 18;
int estadoApagadoSistema;

//parada Emergencia
const byte paradaEmergencia = 19;
int estadoPdE;

//cierre Interruptor Generador 2
const byte cierreInterruptorGE2 = 16;
int estadoCierreInterruptorGE2;

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
const byte pilotoLuminosoCierreInterruptorGE2 = 7;
int estadoPilotoCierreINTGE2;

//piloto luminoso sistema encendido
const byte pilotoLuminosoSistemaEncendido = 8;
int estadoPilotoSistemaEncendido;

//piloto luminoso parada de emergencia activada
const byte pilotoLuminosoPdEActivada = 9;
int estadoPilotoPdEActivada;

//encendido solenoide GE1 Aire
const byte encendidoSolenoideGE1Aire = 10;
int estadoEncendidoSolenoideGE1Aire;

//encendido solenide GE2 Aire
const byte encendidoSolenoideGE2Aire = 11;
int estadoEncendidoSolenoideGE2Aire;

//apagado generador 1
const byte apagadoGE1 = 12;
int estadoApagadoGE1;

//apagado generador 2
const byte apagadoGE2 = 14;
int estadoApagadoGE2;

//contactor auxiliar
const byte contactorAuxiliar = 15;
int estadoContactorAuxiliar;

int caca = 0;

//se repite una vez
void setup(){
  Serial.begin(115200);
pinMode(inicioSistema, INPUT);
pinMode(apagadoSistema, INPUT);
pinMode(paradaEmergencia, INPUT);
pinMode(cierreInterruptorGE2, INPUT);
pinMode(aperturaInterruptorPrincipal, OUTPUT);
pinMode(aperturaInterruptorGE1, OUTPUT);
pinMode(aperturaInterruptorGE2, OUTPUT);
pinMode(pilotoLuminosoCierreInterruptorGE1, OUTPUT);
pinMode(pilotoLuminosoCierreInterruptorGE2, OUTPUT);
pinMode(pilotoLuminosoSistemaEncendido, OUTPUT);
pinMode(pilotoLuminosoPdEActivada, OUTPUT);
pinMode(encendidoSolenoideGE1Aire, OUTPUT);
pinMode(encendidoSolenoideGE2Aire, OUTPUT);
pinMode(apagadoGE1, OUTPUT);
pinMode(apagadoGE2, OUTPUT);
pinMode(contactorAuxiliar, OUTPUT);

//estado encendido RELE
digitalWrite(aperturaInterruptorPrincipal,HIGH);
digitalWrite(pilotoLuminosoSistemaEncendido,HIGH);  
digitalWrite(encendidoSolenoideGE1Aire,HIGH);
digitalWrite(encendidoSolenoideGE2Aire,HIGH);
digitalWrite(aperturaInterruptorGE1,HIGH);
digitalWrite(aperturaInterruptorGE2,HIGH);
digitalWrite(pilotoLuminosoCierreInterruptorGE1,HIGH);
digitalWrite(pilotoLuminosoCierreInterruptorGE2,HIGH);
digitalWrite(pilotoLuminosoPdEActivada,HIGH);
digitalWrite(apagadoGE1,HIGH);
digitalWrite(apagadoGE2,HIGH);
digitalWrite(contactorAuxiliar,HIGH);


timer3.setTimeout(6000);
timer4.setTimeout(6000);
timer5.setTimeout(8000);
timer6.setTimeout(8000);
timer7.setTimeout(10000);



}

//ser repite infinitamente
void loop() {

//estados Botones--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 estadoInicioSistema = digitalRead(inicioSistema);
 estadoApagadoSistema = digitalRead(apagadoSistema);
 estadoPdE = digitalRead(paradaEmergencia);
 estadoCierreInterruptorGE2 = digitalRead(cierreInterruptorGE2);
 estadoINTprincipal = digitalRead(aperturaInterruptorPrincipal);
 estadoAperturaINTGE1 = digitalRead(aperturaInterruptorGE1);
 estadoAperturaINTGE2 = digitalRead(aperturaInterruptorGE2);
 estadoPilotoCierreINTGE1 = digitalRead(pilotoLuminosoCierreInterruptorGE1);
 estadoPilotoCierreINTGE2 = digitalRead(pilotoLuminosoCierreInterruptorGE2);
 estadoPilotoSistemaEncendido = digitalRead(pilotoLuminosoSistemaEncendido);
 estadoPilotoPdEActivada = digitalRead(pilotoLuminosoPdEActivada);
 estadoEncendidoSolenoideGE1Aire = digitalRead(encendidoSolenoideGE1Aire);
 estadoEncendidoSolenoideGE2Aire = digitalRead(encendidoSolenoideGE2Aire);
 estadoApagadoGE1 = digitalRead(apagadoGE1);
 estadoApagadoGE2 = digitalRead(apagadoGE2);
 estadoContactorAuxiliar = digitalRead(contactorAuxiliar);

if(estadoInicioSistema == HIGH){
timer.setTimeout(1000);
  if(timer.onExpired()){
    digitalWrite(aperturaInterruptorPrincipal,LOW);
    Serial.println("pasaron dos segundos");
    (INTprincipal = 1);
  }}
  
 else{
  timer.restart();
  digitalWrite(aperturaInterruptorPrincipal,HIGH);
 }    

//encendido piloto luminoso sistema
if(INTprincipal == 1){
timer1.setTimeout(2000);
 if(timer1.onExpired()){
   digitalWrite(pilotoLuminosoSistemaEncendido,LOW);
    Serial.println("piloto prendido");   
    
    INTprincipal =0;
    caca=1;
 }}
 if(caca == 1){
 timer2.setTimeout(4000);
if(timer2.onExpired()){
 digitalWrite(pilotoLuminosoSistemaEncendido,HIGH);
 Serial.println("piloto apagado");   
 caca=0;}}





}
