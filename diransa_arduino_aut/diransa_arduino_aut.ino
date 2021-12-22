int tuki = 7;
int valvula = 2;
int valvulaEstado;

void setup() {
  pinMode(tuki, OUTPUT);
  pinMode(valvula, INPUT);
}

void loop() {
  valvulaEstado = digitalRead(valvula);

 if(valvulaEstado == 1){
    digitalWrite(tuki, HIGH);
    delay(150);
    digitalWrite(tuki, LOW);
    delay(150);
 }
}
