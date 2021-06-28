#include <Arduino.h>

//librerias primarias
#include <TVout.h>
#include <video_gen.h>
#include <fontALL.h>
#include <font4x6.h>
#include <font6x8.h>
#include <font8x8.h>
#include <font8x8ext.h>
#include <math.h>
#include <RBD_Timer.h>
#include <Temporizador.h> 


//designar variables
TVout TV;
Temporizador temp1;
Temporizador temp2;
Temporizador temp3;
RBD::Timer timer;

/*int zOff = 150;
int xOff = 0;
int yOff = 0;
int cSize = 50;
int view_plane = 64;
float angle = PI/60;*/

//variables Temperatura
const int pinTemp = A0;

const int B = 1000;                 // B valor del termistor
const int R0 = 5000;                // R0 = 100k el mio es de 5k
float temp, temp_med = 0;           // Temperatura en ºC
float acum = 0, Vout;               // variables secundarias
float res = 0.010;                  // en V/ºC
int i, num_muestras = 100;

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

//variable parada motor
const int paradaMotor = 6;
int estadoParadaMotor;

//variable servo
const int servo = 10;
int estadoServo;

//variable prender motor
const int prenderMotor =11;
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

//RPM
unsigned long duracion1; 
unsigned long duracion2; 
unsigned long RPM;
 
 
//comienza y no se repite
void setup() 
{

//temporizador  
 temp1.temporizar(0,0,0,2,0);
 temp2.temporizar(0,0,0,2,0);
 temp3.temporizar(0,0,0,2,0);
 
  
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
 pinMode(pinTemp, INPUT); 
 
//Entrada y Salida pin digital 
 pinMode(8, INPUT);
 pinMode(6, INPUT);
 pinMode(4, INPUT);
 pinMode(5, INPUT);
 pinMode(3, INPUT);
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

//RPM
duracion1 = pulseIn (2, HIGH,100000000); 
duracion2 = pulseIn (2, LOW,100000000); 
RPM = 1000000/(duracion1+duracion2);
TV.print(95,0,RPM);
 
 
//incio del calculo de la temperatura
  acum = 0;
  temp_med = 0;
  //Calculamos primero para el Lm35C
  for (i = 0; i < num_muestras; i++) {
    Vout = analogRead(pinTemp) * 0.00488758553275;    // step * V/step = V
    temp = Vout / res;                                // V/(V/ºC) = ºC
    acum += temp;
    temp_med = acum / num_muestras;
    //delay(10);                                       // Al tener 10 milisegundos y tener 100 muestras tardara 1 segundo en hacer el bucle.
  }                                                  //Fin Calculo de temperatura

//comvertir decimal a entero
 t = temp_med;
 o = (int) t; // ahora i es 3

//Refrescar pantalla
 if(timer.onRestart()){
    TV.clear_screen();}
  
//mostrar calculos en tv
 TV.print(69,15,o); /*muestra de C°en tv*/  TV.print(82,15,"C"); //muestra de C° en tv*/


//reset alarmas
estadoResetAlarmas = digitalRead(resetAlarmas);
 if(estadoResetAlarmas == HIGH){
   digitalWrite(buzzer,LOW);
   digitalWrite(estadoNivelAgua,LOW);
   digitalWrite(aceite,LOW);
   digitalWrite(estadoCorteAgua,LOW);
   digitalWrite(servo,HIGH);
   }
   

//Alarma aceite
estadoAceite = digitalRead(aceite);       // Comprobamos el estado
    
 if(estadoAceite == HIGH){                //Comprobar el pulsador
    digitalWrite(aceite,LOW);
    TV.print(95, 60,"normal");
    temp1.temporizar(0,0,0,2,0);
    digitalWrite(buzzer,LOW);}
 else{
    if(temp1.completado()){
       digitalWrite(aceite,HIGH);
       TV.print(95, 60,"ALARMA");
       digitalWrite(buzzer,HIGH);
        }
  }    

//Alarma Agua Temperatura
 if(o <= 110){
   TV.print(95,15,"normal");}
 else{
   TV.print(95,15,"ALARMA");}

//Alarma Agua
estadoNivelAgua = digitalRead(nivelAgua);     // Comprobamos el estado
    
 if(estadoNivelAgua == HIGH){                //Comprobar el pulsador
    digitalWrite(estadoNivelAgua,LOW);
    TV.print(95,45,"normal");
    temp2.temporizar(0,0,0,2,0);;}
 else{
     if(temp2.completado()){
        digitalWrite(estadoNivelAgua,HIGH);
        TV.print(95,45,"ALARMA");
        digitalWrite(buzzer,HIGH);
     }
  }

//Alarma temperatura corte agua
estadoCorteAgua = digitalRead(temperaturaCorteAgua);     // Comprobamos el estado

  if(estadoCorteAgua == HIGH){                //Comprobar el pulsador
    digitalWrite(estadoCorteAgua,LOW);
    TV.print(95,30,"normal");
    temp3.temporizar(0,0,0,5,0);}
  else{
     if(temp3.completado()){
        digitalWrite(estadoCorteAgua,HIGH);
        digitalWrite(buzzer,HIGH);
          if(estadoCorteAgua = HIGH);{
          TV.print(95,30,"ALARMA");}
        
        }
  }

//servo
estadoServo = digitalRead(servo);
  if((estadoCorteAgua = LOW) || (estadoNivelAgua = LOW)){
    (estadoServo == LOW);}
  else{
  (estadoServo == HIGH);
  }

//encendido motor 
estadoPulsadorIniciarMotor = digitalRead(pulsadorIniciarMotor);
estadoPrenderMotor = digitalRead(prenderMotor);

if(estadoPulsadorIniciarMotor == HIGH){
  if((estadoCorteAgua = LOW) && (estadoNivelAgua = LOW) && (estadoServo = LOW) || (o > 110)){
    digitalWrite(buzzer,HIGH);
    digitalWrite(prenderMotor,LOW);
    digitalWrite(estadoPrenderMotor,LOW);
        TV.print(0,85,"motor error");}
 // else(digitalWrite(estadoArranqueMotor,HIGH));{
 //     if(estadoArranqueMotor = HIGH){
 //       TV.print(70,85,"MOTOR ARRANCANDO");}
  
  }
// si no hay problemas prende motor
  if(estadoPulsadorIniciarMotor == HIGH){
    if((estadoCorteAgua = HIGH) && (estadoNivelAgua = HIGH) && (estadoServo = HIGH) && (o < 110)){
     (digitalWrite(estadoPrenderMotor,HIGH));
        if(estadoPrenderMotor = HIGH);{
          digitalWrite(estadoPrenderMotor,HIGH);
           TV.print(70,85,"MOTOR ARRANCANDO");}
  }
}

//anti bucle arranque  
if(RPM > 100){
  (digitalWrite(estadoPulsadorIniciarMotor,LOW));}

//maxima velocidad motor alarma
if(RPM >100){
  TV.print(95,75,"normal");
}
else{
  TV.print(95,75,"ALARMA");
}

//parada motor
estadoParadaMotor = digitalRead(paradaMotor);

if(estadoParadaMotor == HIGH){
  digitalWrite(estadoPrenderMotor,LOW);
  digitalWrite(estadoServo,LOW);
  digitalWrite(buzzer,HIGH);}
else(digitalWrite(estadoServo,HIGH));



  
}                                     //Termina el loop


