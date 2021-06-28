int entrada = 2;
int led = 3;
int entero=1;
bool estadoPulsador;

void setup() {
Serial.begin(9600);
  
pinMode(entrada,INPUT);
pinMode(led, OUTPUT);

}

void loop() {
  Serial.println(entero);

  estadoPulsador = digitalRead(entrada);

  if ( estadoPulsador == HIGH){
      if (entero == 1){
          digitalWrite(led, HIGH);
          delay2(200);
          digitalWrite(led, LOW);
          delay2(500);
          entero = 2;
      }
  }

  if (estadoPulsador == LOW){ 
      if (entero == 2){
          digitalWrite(led, HIGH);
          delay2(200);
          digitalWrite(led, LOW);
          delay2(500);
          entero = 1;
      }     
  } 
}

void delay2(int time){
  for (int i; i< time/100; i++) {
      estadoPulsador = digitalRead(entrada);
      delay(100);
  }
}
