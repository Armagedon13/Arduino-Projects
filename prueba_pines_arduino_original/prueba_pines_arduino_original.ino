int led = 13;


void setup(){
  Serial.begin(9600);
pinMode(led, OUTPUT); // Pin 12 como Salida.
}

//Ahora le decimos que hacer con esa salida.

void loop(){
  Serial.println(led);
digitalWrite(led, HIGH); // Pone la salida a ON
delay(500);  // Espera un segundo
digitalWrite(led, LOW);  // Pone la salida a OFF
delay(1000);            // Espera un segundo



}

//Fin del programa
