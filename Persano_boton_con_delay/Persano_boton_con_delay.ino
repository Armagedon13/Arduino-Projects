int entrada = 2;
int led = 3;
int estadoPulsador;
bool paso1 = false;
bool paso2 = false;

void setup() {
  
pinMode(entrada,INPUT_PULLUP);
pinMode(led, OUTPUT);

digitalWrite(led, LOW);

}

void loop() {

estadoPulsador = digitalRead(entrada);
digitalWrite(led, LOW);


  if (estadoPulsador == LOW && paso1 == false){
          digitalWrite(led, HIGH);
          delay(500);
          digitalWrite(led, LOW);
          delay(100);
          paso1=true;
          paso2=true;
      
  }

  if (estadoPulsador == HIGH && paso2==true){ 
          digitalWrite(led, HIGH);
          delay(500);
          digitalWrite(led, LOW);
          delay(100);
          paso1=false;
          paso2=false;
          
          
  } 


}



