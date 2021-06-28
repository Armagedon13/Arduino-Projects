#include <Arduino.h>

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
int lex                               = 1;
const byte pin2 = 2;
int estadoBoton;

void setup() {
  pinMode(servo, OUTPUT);
  pinMode(pin2, INPUT);
}

void loop() {
estadoBoton = digitalRead(pin2);

if(estadoBoton == HIGH){
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


if(estadoBoton == LOW){
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

}
