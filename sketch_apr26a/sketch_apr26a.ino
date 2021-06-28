int entrada = 2;
int led = 3;
int estadoPulsador;
bool paso1 = false;
bool paso2 = false;

int periodo = 1000;  // tiempo para que el led se encienda
byte temp = 0; //estado del temporizador, 1= activo 0= inactivo
unsigned long tiempoAnterior = 0;  //guarda tiempo de referencia para comparar

void setup() {
Serial.begin(9600);
  
pinMode(entrada,INPUT_PULLUP);
pinMode(led, OUTPUT);

digitalWrite(led, LOW);

}

void loop() {

estadoPulsador = digitalRead(entrada);

  if (estadoPulsador == HIGH && paso1 == false){
      tiempoAnterior=millis();  //guarda el tiempo actual como referencia
      temp = 1;  //indica que esta activo el temporizador
          digitalWrite(led, HIGH);
         if((millis()>tiempoAnterior+periodo)&&temp==1){
          digitalWrite(led, LOW);}
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



