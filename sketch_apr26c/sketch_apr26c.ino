int entrada = 2;
int led = 3;
bool estado_entrada=true;
bool estado_ant_entrada=true;


void setup() {
  
pinMode(entrada,INPUT);
pinMode(led, OUTPUT);

}

void loop() {

estado_entrada=digitalRead(entrada);


// Aqui cuando  presionas el led enciende y apaga una sola vez
  if (estado_entrada != estado_ant_entrada)     
    if(estado_entrada==false){                   
  delay(100);
  digitalWrite(led, LOW);
  delay(500);
  estado_ant_entrada=estado_entrada;
  }
    
 //cuando sueltas el pulsador el led prende y apaga una vez
if (estado_entrada != estado_ant_entrada) 
 if(estado_entrada==true){
  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);
  delay(500);
  estado_ant_entrada=estado_entrada; 
    }
}

