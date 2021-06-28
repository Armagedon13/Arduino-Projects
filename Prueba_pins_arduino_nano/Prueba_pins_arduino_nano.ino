int boton = 17;
int estadoBoton;
int rele1 = 2;
int rele2 = 3;
int rele3 = 4;
int rele4 = 5;
int rele5 = 6;
int rele6 = 7;
int rele7 = 8;
int rele8 = 9;

int rele9 = 10;
int rele10 = 11;
int rele11 = 12;
int rele12 = 13;
int rele13 = 14;
int rele14 = 15;
int rele15 = 16;
int rele16 = 19;



void setup() {
pinMode(boton, INPUT);
pinMode(rele1, OUTPUT);
pinMode(rele2, OUTPUT);
pinMode(rele3, OUTPUT);
pinMode(rele4, OUTPUT);
pinMode(rele5, OUTPUT);
pinMode(rele6, OUTPUT);
pinMode(rele7, OUTPUT);
pinMode(rele8, OUTPUT);

pinMode(rele9, OUTPUT);
pinMode(rele10, OUTPUT);
pinMode(rele11, OUTPUT);
pinMode(13, OUTPUT);
pinMode(rele13, OUTPUT);
pinMode(rele14, OUTPUT);
pinMode(rele15, OUTPUT);
pinMode(rele16, OUTPUT);

 digitalWrite(13,HIGH);




}

void loop() {

 estadoBoton = digitalRead(boton);
 
 if(estadoBoton == HIGH){
  digitalWrite(rele1,LOW);
  digitalWrite(rele2,LOW);
  digitalWrite(rele3,LOW);
  digitalWrite(rele4,LOW);
  digitalWrite(rele5,LOW);
  digitalWrite(rele6,LOW);
  digitalWrite(rele7,LOW);
  digitalWrite(rele8,LOW);

  digitalWrite(rele9,LOW);
  digitalWrite(rele10,LOW);
  digitalWrite(rele11,LOW);
  digitalWrite(rele12,LOW);
  digitalWrite(rele13,LOW);
  digitalWrite(rele14,LOW);
  digitalWrite(rele15,LOW);
  digitalWrite(rele16,LOW);
  
 }
 
else{
  digitalWrite(rele1,HIGH);
  digitalWrite(rele2,HIGH);
  digitalWrite(rele3,HIGH);
  digitalWrite(rele4,HIGH);
  digitalWrite(rele5,HIGH);
  digitalWrite(rele6,HIGH);
  digitalWrite(rele7,HIGH);
  digitalWrite(rele8,HIGH);

  digitalWrite(rele9,HIGH);
  digitalWrite(rele10,HIGH);
  digitalWrite(rele11,HIGH);
  digitalWrite(rele12,HIGH);
  digitalWrite(rele13,HIGH);
  digitalWrite(rele14,HIGH);
  digitalWrite(rele15,HIGH);
  digitalWrite(rele16,HIGH);

}



}
