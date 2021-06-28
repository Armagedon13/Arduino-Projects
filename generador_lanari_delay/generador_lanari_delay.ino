int pote= A1; 
int caca = 2;

void setup() {
  pinMode(A1,INPUT);
  pinMode(caca,OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int poteValue= analogRead(pote);
  int timer = map(poteValue,0,1023,1000,6000);
  delay(timer);
  digitalWrite(caca,HIGH);
  Serial.print("Tiempo: ");
  Serial.print(timer);
  Serial.print(" Lectura: ");
  Serial.print(digitalRead(caca));
  Serial.println();
}
