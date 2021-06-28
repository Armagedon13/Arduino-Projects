int entrada = 6;
int led = 13;
int estadoPulsador;
bool paso1 = false;
bool paso2 = false;

void setup() {
Serial.begin(9600);
  
pinMode(entrada,INPUT_PULLUP);
pinMode(led, OUTPUT);

}

void loop() {

estadoPulsador = digitalRead(entrada);

  if (estadoPulsador == HIGH && paso1 == false){
          digitalWrite(led, HIGH);
          delay(500);
          digitalWrite(led, LOW);
          delay(500);
          paso1=true;
          paso2=true;
      
  }

  if (estadoPulsador == LOW && paso2==true){ 
          digitalWrite(led, HIGH);
          delay(500);
          digitalWrite(led, LOW);
          delay(500);
          paso1=false;
          paso2=false;
          
          
  } 


}
