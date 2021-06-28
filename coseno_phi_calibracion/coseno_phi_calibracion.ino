 
const int contactor = 12;
int contador;
int contador2;
float PF;
bool sen=false;


void setup() 
{
  Serial.begin(9600);
  pinMode(contactor, OUTPUT);
  pinMode(A0, INPUT);
  digitalWrite(contactor, HIGH);
}

void loop() 
{
 int sensorValue = analogRead(A0);
 float PF = sensorValue * (1.0 / 1023.0);
 
  if(PF<=0.85){                  //si el factor de potencia es menor a 0.85 se activa el contactor 
    contador++;
    contador2=0;
    Serial.println(contador);
    if(contador==10000){
    digitalWrite(contactor,LOW);
    contador=0;
    }}
  if(PF>=0.85){                   //si el factor de potencia es mayor a 0.85 se apaga el contactor
    contador2++;
    contador=0;
    Serial.println(contador2);
    if(contador2==10000){
    contador=0;
    digitalWrite(contactor,HIGH);
    }}

   
    


//Serial.println(PF);


}
