const int EntradaADC = A0;
const int SalidaPWM = 11;
int AnalogIN;

void setup() {

  Serial.begin(9600);
  pinMode(EntradaADC, INPUT);
  pinMode(SalidaPWM, OUTPUT);

}

void loop() {

float m= ;
float volt=m*5/1024;

 AnalogIN =  analogRead(EntradaADC);
 analogWrite(SalidaPWM, m);
 Serial.println(volt);

}
