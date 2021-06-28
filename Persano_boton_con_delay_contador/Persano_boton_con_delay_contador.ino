int entrada = 2;
int led = 3;
int estadoPulsador;
int contador;
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
          contador=1;
          if(contador==1){
          digitalWrite(led, LOW);
          delay(300);
          paso1=true;
          paso2=true;
          contador=2;}
      
  }

  if (estadoPulsador == HIGH && paso2==true){ 
  	if(contador==2){
          digitalWrite(led, HIGH);
          delay(500);
          contador=3;
          if(contador==3){
          digitalWrite(led, LOW);
          delay(300);
          paso1=false;
          paso2=false;
          contador=0;}}
          
          
  } 


}



