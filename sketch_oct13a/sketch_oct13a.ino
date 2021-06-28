int boton = 17;
int estadoBoton;
int rele12 = 14;

long previousMillis = 0; 
long intervalOn = 3000;           // medio segundo  ON
long intervalOff = 5000;         // cinco segundos OFF
int ledState = LOW;             // ledState used to set the LED


void setup() {
pinMode(boton, INPUT);
pinMode(rele12, OUTPUT);
}

void loop() {
  estadoBoton = digitalRead(boton);
  
unsigned long currentMillis = millis();
  if(estadoBoton == HIGH) {
    if(currentMillis - previousMillis > intervalOff) {
            previousMillis = currentMillis;   
            ledState = HIGH;}
} else {

          ledState = LOW;
    
}
  
 digitalWrite(rele12,ledState);


}
