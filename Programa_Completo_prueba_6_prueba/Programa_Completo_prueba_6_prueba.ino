#include <Arduino.h>

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

/*int zOff = 150;
int xOff = 0;
int yOff = 0;
int cSize = 50;
int view_plane = 64;
float angle = PI/60;*/

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
int gamma = 0;

//variable parada motor
const int paradaMotor = 6;
int estadoParadaMotor;

//variable servo
const int servo = 10;
int estadoServo;

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

//variable alarma
int alarma = 0;

//RPM

 
//comienza y no se repite
void setup() 
{

//RPM
// reset Timer 1
  TCCR1A = 0;
  TCCR1B = 0;
  // Timer 1 - interrupt on overflow
  TIMSK1 = bit (TOIE1);   // enable Timer1 Interrupt
  // zero it
  TCNT1 = 0;  
  overflowCount = 0;  
  // start Timer 1
  TCCR1B =  bit (CS10);  //  no prescaling

  // set up for interrupts
  prepareForInterrupts ();  
  
  digitalWrite(buzzer,HIGH);
  digitalWrite(prenderMotor,HIGH);
  
//temperatura
  pinMode(analogPin, INPUT);

  //Global parameters calculation
  beta=(log(RT1/RT2))/((1/T1)-(1/T2));
  Rinf=R0*exp(-beta/T0);

//temporizador  
 temp1.temporizar(0,0,0,2,0);
 temp2.temporizar(0,0,0,2,0);
 temp3.temporizar(0,0,0,2,0);
 temp4.temporizar(0,0,0,2,0);
  
//Tiempo Reset
 timer.setTimeout(1000);               //setea el tiempo de reset de variables
 timer.restart();

 
//pantalla de inicio
 TV.begin(PAL,120,96);                 //declarar tipo de video
 TV.select_font(font8x8);              //seleccionar fuente y tamaño
 TV.print(0,10,"Equipo Vigia\nSAFE motor");     
 TV.select_font(font8x8);
 TV.print(0,40, "T.N.A");
 TV.print(0,55,"Powered by\nService Naval\nArgentina");
 TV.select_font(font4x6);              //seleccionar fuente y tamaño
 TV.print(0,85,"www.servicenaval.tk");
 TV.delay(5000);                      // delay cuenta el tiempo de cuanto esta en pantalla
    TV.clear_screen();                // pone la pantalla en negro
 


//Entrada pin analogica
// pinMode(pinTemp, INPUT); 
 
//Entrada y Salida pin digital 
 pinMode(8, INPUT);
 pinMode(6, INPUT);
 pinMode(4, INPUT);
 pinMode(5, INPUT);
 pinMode(3, INPUT);
 pinMode(2, INPUT);
 pinMode(13, INPUT);
 pinMode(10, OUTPUT);
 pinMode(11, OUTPUT);
 pinMode(12, OUTPUT);
  

}

// repite los procesos
void loop() {

//pantalla de variables
 TV.select_font(font4x6);
 TV.print(0,0, "velocidad motor:");
 TV.print(0,15,"temperatura agua:");      
 TV.print(0,30,"temperatura corte agua:");
 TV.print(0,45,"nivel de agua:");
 TV.print(0,60,"presion corte aceite:"); 
 TV.print(0,75,"maxima velocidad motor:"); 
  

 
//incio del calculo de la temperatura
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
 TV.print(69,15,o); /*muestra de C°en tv*/  TV.print(82,15,"C"); //muestra de C° en tv*/


//alarma total

//sigue
if((estadoAceite == HIGH) || (estadoNivelAgua == HIGH) || (estadoCorteAgua == HIGH) || (o < 110)||  (estadoServo == LOW) /*|| (RPM < 100)*/){
 (alarma = 2);}

//error 
if((estadoAceite == LOW) || (estadoNivelAgua == LOW) || (estadoCorteAgua == LOW) || (o > 110) || (estadoServo == HIGH)){
  (alarma = 1);}

//Alarma aceite
estadoAceite = digitalRead(aceite);       // Comprobamos el estado
    
 if(estadoAceite == HIGH){                //Comprobar el pulsador
    TV.print(95, 60,"normal");
    temp1.temporizar(0,0,0,2,0);}
 else{
    if(temp1.completado()){
       TV.print(95, 60,"ALARMA");
       //digitalWrite(buzzer,LOW);
       //digitalWrite(servo,LOW);
        }
  } 

//Alarma Temperatura Agua 
if(o <= 110){
   TV.print(95,15,"normal");}
 else{
  //digitalWrite(buzzer,LOW);
  //digitalWrite(servo,HIGH);
   TV.print(95,15,"ALARMA");}

//Alarma nivel de Agua
estadoNivelAgua = digitalRead(nivelAgua);     // Comprobamos el estado
    
 if(estadoNivelAgua == HIGH){                //Comprobar el pulsador
    //digitalWrite(buzzer,HIGH);
    TV.print(95,45,"normal");
    temp2.temporizar(0,0,0,2,0);}
 else{
     if(temp2.completado()){
        TV.print(95,45,"ALARMA");
        //digitalWrite(buzzer,LOW);
        
     }
  }

//Alarma temperatura corte agua
estadoCorteAgua = digitalRead(temperaturaCorteAgua);     // Comprobamos el estado

  if(estadoCorteAgua == HIGH){
      TV.print(95,30,"normal"); 
        temp3.temporizar(0,0,0,2,0);}               //Comprobar el pulsador
  else{
    if(temp3.completado()){
      TV.print(95,30,"ALARMA");
      //digitalWrite(buzzer,LOW);
     }}


//encendido motor 
estadoPulsadorIniciarMotor = digitalRead(pulsadorIniciarMotor);
estadoPrenderMotor = digitalRead(prenderMotor);

if((estadoPulsadorIniciarMotor == LOW) && (alarma == 1)){
    digitalWrite(buzzer,LOW);  
    //digitalWrite(prenderMotor,HIGH);
    TV.print(0,85,"motor error");
  }

// si no hay problemas prende motor
if((estadoPulsadorIniciarMotor == LOW) && (alarma == 2)) {
    digitalWrite(prenderMotor,LOW);
     TV.print(60,85,"MOTOR ENCENDIENDO");}
     else{
      digitalWrite(prenderMotor,HIGH);
     }
    
    
//anti bucle arranque  
/*if(RPM > 500){
  (digitalWrite(prenderMotor,HIGH));
   digitalWrite(pulsadorIniciarMotor,HIGH);}*/


//parada motor
estadoParadaMotor = digitalRead(paradaMotor);

if(estadoParadaMotor == LOW){
  digitalWrite(prenderMotor,HIGH);}
  //digitalWrite(servo,HIGH);
  //digitalWrite(buzzer,LOW);}


//maxima velocidad motor
/*if(RPM > 2000){
   TV.print(95,75,"ALARMA");}
if(RPM < 2000){
  TV.print(95,75,"normal");}
if(RPM < 200){
  TV.print(90,0,"APAGADO");}*/

//cable temperatura cortado
if(o < (-10)){
  TV.print(0,85,"cable agua temperatura cortado");}


//buzzer apagado
estadoResetAlarmas = digitalRead(resetAlarmas);

if((estadoResetAlarmas == LOW) && (estadoBuzzer == LOW)){
  (gamma = 1);}
if((estadoResetAlarmas == LOW) && (gamma == 1) && (estadoBuzzer == LOW)){
  (gamma = 2);}
if((estadoBuzzer == LOW) && (gamma == 2)){
  digitalWrite(buzzer,HIGH);}


//set alarma corte
estadoServo = digitalRead(servo);

if((estadoAceite == HIGH) && (estadoNivelAgua == HIGH) && (estadoCorteAgua == HIGH) && (o < 110) && (estadoParadaMotor == HIGH)){
    temp4.temporizar(0,0,0,2,0);
    digitalWrite(servo,LOW);
    digitalWrite(buzzer,HIGH);
     (gamma = 0);}
  else{
    if(gamma == 0){
    if(temp4.completado()){
    digitalWrite(servo,HIGH);
    digitalWrite(buzzer,LOW);}}}


//RPM



//Refrescar pantalla
 //if(timer.onRestart()){
   // TV.clear_screen();}

  
}                                     //Termina el loop


