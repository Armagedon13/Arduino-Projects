//librerias primarias
#include <RBD_Timer.h>
#include <Temporizador.h>
#include <TVout.h>
#include <video_gen.h>
#include <fontALL.h>
#include <stdio.h>
#include <math.h>
#include <FreqCount.h>

//designar variables
TVout TV;
Temporizador temp1;
Temporizador temp2;
Temporizador temp3;
Temporizador temp4;
RBD::Timer timer;

//variables Temperatura
int analogPin=0;   // Which pin will be the input of the Voltage-Divider

float Vin=4.5;     // [V]        Supply voltage in the Voltage-Divider
float Raux=1000;  // [ohm]      Secondary resistor in the Voltage-Divider
float R0=3500;    // [ohm]      NTC nominal value at 25ºC
float T0=293.15;   // [K] (25ºC)

float Vout=0.0;    // [V]        Voltage given by the Voltage-Divider
float Rout=0.0;    // [ohm]      Current NTC resistance

float T1=273;      // [K]        Temperature at first testing point
float T2=373;      // [K]        Temperature at second testing point
float RT1=19750;   // [ohms]     Resistance at 273K (0ºC)
float RT2=2150;    // [ohms]     Resistance at 373K (100ºC)

float beta=0.0;    // [K]        Beta parameter
float Rinf=0.0;    // [ohm]      Rinf parameter
float TempK=0.0;   // [K]        Temperature output in Kelvin
float TempC=0.0;   // [ºC]       Temperature output in Celsius

//variable servo
int rox;
const byte servo = 10;

//variables de cambio a entero
int o;
float t;

//variable aceite
const byte aceite = 8;
int estadoAceite;

//variable pulsador iniciar motor
const byte pulsadorIniciarMotor = 2;
int estadoPulsadorIniciarMotor;

//variable reset alarmas
const byte resetAlarmas = 4;
int estadoResetAlarmas; 
int gamma;

//variable parada motor
const byte paradaMotor = 6;
int estadoParadaMotor;

//variable prender motor
const byte prenderMotor = 11;
int estadoPrenderMotor;

//variable Temperatura corte agua
const byte temperaturaCorteAgua = 13;
int estadoCorteAgua;

//variable buzzer
const byte buzzer = 12;
int estadoBuzzer;

//variable Nivel de agua
const byte nivelAgua = 3;
int estadoNivelAgua;

//variable Salida Alternador
const byte alternador = 15;
int estadoAlternador;

//Variable alarma
int alarma = 0;
int sonido = 0;

//RPM
unsigned long RPM;
unsigned long pulso_valor;
unsigned long pulso_valor_t;
unsigned long pulso_valor2;
bool habilitar_pulsos = false;
unsigned long pulsos_tiempo_f;
unsigned long pulsos_tiempo_i;

//-----------------------------------------------------------------
void setup(){
  
//Entrada pin analogica
 pinMode(analogPin, INPUT); 
 
//Entrada y Salida pin digital 
  pinMode(8, INPUT);
  pinMode(6, INPUT);
  pinMode(4, INPUT);
  pinMode(3, INPUT);
  pinMode(2, INPUT);
  pinMode(13, INPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(10,OUTPUT);
  FreqCount.begin(2000);
  
//Global parameters calculation
  beta=(log(RT1/RT2))/((1/T1)-(1/T2));
  Rinf=R0*exp(-beta/T0);
     
//estado encendido rele
  digitalWrite(buzzer,HIGH);
  digitalWrite(prenderMotor,HIGH);
  digitalWrite(alternador,HIGH);
  
//Tiempo Reset
  timer.setTimeout(1000);               //setea el tiempo de reset de variables
  timer.restart();

//temporizador  
  temp1.temporizar(0,0,0,2,0);
  temp2.temporizar(0,0,0,2,0);
  temp3.temporizar(0,0,0,2,0);
  temp4.temporizar(0,0,0,2,0); 
  
//pantalla de inicio
  TV.begin(PAL,120,96); 
    
}
  
//--------------------------------------------------------------------------
void loop(){

//pantalla de variables
  TV.select_font(font4x6);
  TV.print(0,0, "velocidad motor:");
  TV.print(0,15,"temperatura agua:");      
  TV.print(0,30,"temperatura corte agua:");
  TV.print(0,45,"nivel de agua:");
  TV.print(0,60,"presion corte aceite:"); 
  TV.print(0,75,"maxima velocidad motor:");

//RPM
  if (FreqCount.available()) {
    unsigned long count = FreqCount.read();
    TV.print(90,0,count);}
  
//estados de variables
  estadoNivelAgua = digitalRead(nivelAgua);
  estadoCorteAgua = digitalRead(temperaturaCorteAgua);    
  estadoPulsadorIniciarMotor = digitalRead(pulsadorIniciarMotor);
  estadoPrenderMotor = digitalRead(prenderMotor);
  estadoAlternador = digitalRead(alternador);
  estadoAceite = digitalRead(aceite);
  estadoResetAlarmas = digitalRead(resetAlarmas);  
  estadoParadaMotor = digitalRead(paradaMotor); 
  estadoBuzzer = digitalRead(buzzer);
  
//Current NTC resistance value calculation (through Vout)
  Vout=Vin*((float)(analogRead(analogPin))/1024.0);
  Rout=(Raux*Vout/(Vin-Vout));

  //Temperature calculation
  TempK=(beta/log(Rout/Rinf));
  TempC=TempK-273.15;

//comvertir decimal a entero
 t =  TempC;
 o = (int) t; // ahora i es 3

//mostrar calculos en tv
//if(o > 10)
  TV.println(69,15,o);     //muestra de C°en tv
 // else{
     //TV.print(69,15,"12");
 // }
  //TV.print(82,15,"C");   //muestra de C° en tv

//Alarma Temperatura Agua 
  if(o <= 110){
   TV.print(95,15,"normal");}
  else{
   TV.print(95,15,"ALARMA");}

//nivel de agua
 if(estadoNivelAgua == HIGH){                //Comprobar el pulsador
    TV.print(95,45,"normal");
    temp2.temporizar(0,0,0,2,0);}
 else{
     if(temp2.completado()){
        TV.print(95,45,"ALARMA");}}

//Alarma temperatura corte agua
  if(estadoCorteAgua == HIGH){
      TV.print(95,30,"normal"); 
        temp3.temporizar(0,0,0,2,0);}            //Comprobar el pulsador
  else{
    if(temp3.completado()){
      TV.print(95,30,"ALARMA");
     }}
     
//Buzzer como sonido*************************************************************************************
  if(sonido == 1){
    digitalWrite(buzzer,LOW);}
  if(sonido == 2){
    digitalWrite(buzzer,HIGH);}
//********************************************************************************************************

//buzzer apagado
  if((estadoResetAlarmas == LOW) && (sonido == 1)){
    (gamma = 1);}
  if((estadoResetAlarmas == LOW) && (gamma == 1) && (sonido == 1)){
    (gamma = 2);}
  if((sonido == 1) && (gamma == 2)){
    (sonido = 2);}

//set alarma corte
  if((estadoAceite == LOW)  && (estadoCorteAgua == HIGH) && (o < 110) && (estadoParadaMotor == HIGH)){
      temp4.temporizar(0,0,0,2,0);
      (gamma = 0);
      (rox=1);
}
  else{
    (rox = 2);
    if(gamma == 0){
      if(temp4.completado()){
        (sonido = 1);       
    }}}

//alarma agua
  if(estadoNivelAgua == HIGH){
  }
  else{
    if(gamma == 0){
    (sonido = 1);
  }}
    

//alarma encendido motor
//sigue
if((estadoParadaMotor == HIGH) || (estadoCorteAgua == HIGH) || (o < 110) || (RPM < 100)){
 (alarma = 2);}

//error 
if((estadoParadaMotor == LOW) || (estadoCorteAgua == LOW) || (o > 110) || (RPM > 100)) {
  (alarma = 1);}     
  
//encendido motor 
  if((estadoPulsadorIniciarMotor == LOW) && (alarma == 1)){
      digitalWrite(buzzer,LOW);
      //TV.print(0,85,"motor error");
  }

// si no hay problemas prende motor
  if((estadoPulsadorIniciarMotor == LOW) && (alarma == 2)) {
    digitalWrite(prenderMotor,LOW);}
     //TV.print(60,85,"MOTOR ENCENDIENDO");}
  else{
    digitalWrite(prenderMotor,HIGH);
     }

//RPM

 
//maxima velocidad motor
  if(RPM > 8000){
    TV.print(95,75,"ALARMA");}
  if(RPM < 8000){
    TV.print(95,75,"normal");}
    
//alternador prendido rele y estadoAceite
if(RPM > 500 && (rox == 1)){ 
  digitalWrite(alternador,LOW);
   if(estadoAceite == LOW){                //Comprobar el pulsador
    TV.print(95, 60,"normal");
    temp1.temporizar(0,0,0,2,0);}
 else{
   digitalWrite(alternador,HIGH);
    if(temp1.completado()){
       TV.print(95, 60,"ALARMA");
    }}} 

//Servo***********************************************************************************************************************************************************
if(rox == 1){
  TV.print(0,85,"servo inactivo");
  for (int Hz =0; Hz < 50 ;Hz++){      // repetimos la instruccion 50 veces
  digitalWrite (servo,HIGH); 
  delayMicroseconds(1500);               // llevamos a 180°
  digitalWrite (servo,LOW);
  delay(18);
 }}

if(rox==2){
  digitalWrite(alternador,HIGH);
  TV.print(0,85,"servo activo Quitar PdE");
  for (int Hz =0; Hz < 50 ;Hz++){     // repetimos la instruccion 50 veces
digitalWrite (servo,HIGH);
delayMicroseconds(500);                // llevamos a 90°
digitalWrite (servo,LOW);
delay(19);
}}
//Servo***********************************************************************************************************************************************************

//Refrescar pantalla
 /*if(timer.onRestart()){
    TV.clear_screen();} */

    
  }
