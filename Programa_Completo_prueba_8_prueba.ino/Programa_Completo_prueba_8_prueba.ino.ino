//librerias primarias
#include <TVout.h>
#include <video_gen.h>
#include <fontALL.h>
#include <font4x6.h>
#include <font6x8.h>
#include <font8x8.h>
#include <font8x8ext.h>
#include <RBD_Timer.h>
#include <Temporizador.h> 


//designar variables
TVout TV;
Temporizador temp1;
Temporizador temp2;
Temporizador temp3;
Temporizador temp4;
RBD::Timer timer;

//variables Temperatura
int analogPin;   // Which pin will be the input of the Voltage-Divider

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

//variables de cambio a entero
int o;
float t;

//variable aceite
const int aceite = 8;
int estadoAceite;

//variable pulsador iniciar motor
const int pulsadorIniciarMotor = 5;
int estadoPulsadorIniciarMotor;

//variable reset alarmas
const int resetAlarmas = 4;
int estadoResetAlarmas; 
int gamma;

//variable parada motor
const int paradaMotor = 6;
int estadoParadaMotor;

//variable servo
int lex = 0;
unsigned long tiempo_inicio           = 0;    
unsigned long tiempo_fin              = 0;    
unsigned long tiempo_encendido        = 0; 
unsigned long tiempo_posicion_90_on   = 0;
unsigned long tiempo_posicion_90_off  = 0;
unsigned long tiempo_posicion_0_on    = 0;
unsigned long tiempo_posicion_0_off   = 0;
float tiempo_total                    = 0;    
bool habilitador                      = false;
bool habilitador2                     = false;
const int servo                       = 10;

//variable prender motor
const int prenderMotor = 11;
int estadoPrenderMotor;

//variable Temperatura corte agua
const int temperaturaCorteAgua = 13;
int estadoCorteAgua;

//variable buzzer
const int buzzer = 12;
int estadoBuzzer;

//variable Nivel de agua
const int nivelAgua = 3;
int estadoNivelAgua;

//variable Salida Alternador
const int alternador = 15;
int estadoAlternador;

//variable alarma
int alarma = 0;

//RPM
unsigned long duracion1; 
unsigned long duracion2; 
int RPM;
int frecuencia;

//-----------------------------------------------------------------
void setup(){
//pantalla de inicio
  TV.begin(PAL,120,96);                 
  TV.select_font(font8x8);              
  TV.print(0,10,"Equipo Vigia\nSAFE motor");     
  TV.select_font(font8x8);
  TV.print(0,40, "T.N.A");
  TV.print(0,55,"Powered by\nService Naval\nArgentina");
  TV.select_font(font4x6);              
  TV.print(0,85,"www.servicenaval.tk");
  //TV.delay(5000);                      
  TV.clear_screen();                

//Entrada y Salida pin digital 
  pinMode(8, INPUT);
  pinMode(6, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(3, INPUT);
  pinMode(2, INPUT);
  pinMode(13, INPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(10, OUTPUT);
  
//estado encendido rele
  digitalWrite(buzzer,HIGH);
  digitalWrite(prenderMotor,HIGH);
  digitalWrite(alternador,HIGH);

//temperatura
  pinMode(analogPin, INPUT);

  //Global parameters calculation
  beta=(log(RT1/RT2))/((1/T1)-(1/T2));
  Rinf=R0*exp(-beta/T0);
  
//Tiempo Reset
  timer.setTimeout(1000);               //setea el tiempo de reset de variables
  timer.restart();

//temporizador  
  temp1.temporizar(0,0,0,2,0);
  temp2.temporizar(0,0,0,2,0);
  temp3.temporizar(0,0,0,2,0);
  temp4.temporizar(0,0,0,2,0);

 
  
}
  
//------------------------------------------------
void loop(){
  
//incio del calculo de la temperatura
//Current NTC resistance value calculation (through Vout)
  Vout=Vin*((float)(analogRead(analogPin))/1024.0);
  Rout=(Raux*Vout/(Vin-Vout));

//Temperature calculation
  TempK=(beta/log(Rout/Rinf));
  TempC=TempK-273.15;

//pantalla de variables
  TV.select_font(font4x6);
  TV.print(0,0, "velocidad motor:");
  TV.print(0,15,"temperatura agua:");      
  TV.print(0,30,"temperatura corte agua:");
  TV.print(0,45,"nivel de agua:");
  TV.print(0,60,"presion corte aceite:"); 
  TV.print(0,75,"maxima velocidad motor:"); 

//estados de variables
  estadoNivelAgua = digitalRead(nivelAgua);
  estadoCorteAgua = digitalRead(temperaturaCorteAgua);    
  estadoPulsadorIniciarMotor = digitalRead(pulsadorIniciarMotor);
  estadoPrenderMotor = digitalRead(prenderMotor);
  estadoAlternador = digitalRead(alternador);
  estadoAceite = digitalRead(aceite);
  estadoResetAlarmas = digitalRead(resetAlarmas);  
  estadoParadaMotor = digitalRead(paradaMotor); 

//comvertir decimal a entero
  t =  TempC;
  o = (int) t; // ahora i es 3

//mostrar calculos en tv
  TV.print(69,15,o);     //muestra de C°en tv
  TV.print(82,15,"C");   //muestra de C° en tv

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

//buzzer apagado
if((estadoResetAlarmas == LOW) && (estadoBuzzer == LOW)){
  (gamma = 1);}
if((estadoResetAlarmas == LOW) && (gamma == 1) && (estadoBuzzer == LOW)){
  (gamma = 2);}
if((estadoBuzzer == LOW) && (gamma == 2)){
  digitalWrite(buzzer,HIGH);}

//set alarma corte
if((estadoAceite == LOW)  && (estadoCorteAgua == HIGH) && (o < 110) && (estadoParadaMotor == HIGH)){
    temp4.temporizar(0,0,0,2,0);
    digitalWrite(buzzer,HIGH);
    (gamma = 0);
    (lex = 1);
    }
  else{
    digitalWrite(alternador,HIGH);
    (lex = 2);
    if(gamma == 0){
      if(temp4.completado()){
        digitalWrite(buzzer,LOW);
        
    }}}

/*if((estadoNivelAgua == LOW) || (gamma == 0)){
   digitalWrite(buzzer,LOW);
  }*/

//alarma encendido motor
//sigue
if((estadoParadaMotor == HIGH) || (estadoCorteAgua == HIGH) || (o < 110) || (RPM < 100)){
 (alarma = 2);}

//error 
if((estadoParadaMotor == LOW) || (estadoCorteAgua == LOW) || (o > 110)) {
  (alarma = 1);}     
  
//encendido motor 
  if((estadoPulsadorIniciarMotor == LOW) && (alarma == 1)){
      digitalWrite(buzzer,LOW);
      TV.print(0,85,"motor error");
  }

// si no hay problemas prende motor
  if((estadoPulsadorIniciarMotor == LOW) && (alarma == 2)) {
    digitalWrite(alternador,LOW);
    digitalWrite(prenderMotor,LOW);
     TV.print(60,85,"MOTOR ENCENDIENDO");}
  else{
    digitalWrite(alternador,HIGH);
    digitalWrite(prenderMotor,HIGH);
     }

//Serv0
if(lex == 1){
 if(habilitador == false){
  tiempo_inicio=millis();
  tiempo_posicion_90_on = tiempo_inicio + 1,1;
  tiempo_posicion_90_off = tiempo_posicion_90_on + 18,9;
  digitalWrite(servo,HIGH);
  habilitador = true;}}

if(habilitador == true){
  if((millis() >= tiempo_posicion_90_on) && (millis() <= tiempo_posicion_90_off)) {
    digitalWrite(servo,LOW);}
  if(millis() > tiempo_posicion_90_off){
      digitalWrite(servo,HIGH);
      habilitador = false;}}


if(lex == 2){
  if(habilitador2 == false){
  tiempo_inicio=millis();
  tiempo_posicion_0_on = tiempo_inicio + 1,5;
  tiempo_posicion_0_off = tiempo_posicion_0_on + 18,5;
  digitalWrite(servo,HIGH);
  habilitador2 = true;}}

if(habilitador2 == true){
  if((millis() >= tiempo_posicion_0_on) && (millis() <= tiempo_posicion_0_off)) {
    digitalWrite(servo,LOW);}
  if(millis() > tiempo_posicion_0_off){
      digitalWrite(servo,HIGH);
      habilitador2 = false;}}

//cable temperatura cortado
  if(o < (-10)){
    TV.print(0,85,"cable agua temperatura cortado");}

//RPM
duracion1 = pulseIn (2, HIGH,100000); 
duracion2 = pulseIn (2, LOW,100000); 
frecuencia = (1000000/(duracion1+duracion2));
RPM = frecuencia * 60;
 TV.print(95,0,RPM);


//Refrescar pantalla
 if(timer.onRestart()){
    TV.clear_screen();}

//maxima velocidad motor
  if(RPM > 8000){
    TV.print(95,75,"ALARMA");}
  if(RPM < 8000){
    TV.print(95,75,"normal");}  
    
//alternador prendido rele y estadoAceite
if(RPM > 1000){ 
  digitalWrite(alternador,LOW);
   if(estadoAceite == LOW){                //Comprobar el pulsador
    TV.print(95, 60,"normal");
    temp1.temporizar(0,0,0,2,0);}
 else{
    if(temp1.completado()){
       TV.print(95, 60,"ALARMA");
    }}} 
 
 
  }
