unsigned long RPM;
unsigned long pulso_valor;
unsigned long pulso_valor_t;
unsigned long pulso_valor2;
unsigned long timer_1seg;

int entero;
float promedio;


void setup() {
 Serial.begin(57600);

}

void loop() {

//RPM*************************************************************************************************************

  pulso_valor = pulseIn(5, HIGH),100000; 
  pulso_valor2 = pulseIn(5, LOW,100000);
  pulso_valor_t = pulso_valor+pulso_valor2;
  timer_1seg = 1000000 / pulso_valor_t;
  promedio = 60 * timer_1seg;

RPM =  promedio;
entero = (int) RPM; // ahora i es 3
  
 Serial.println(RPM);
 

}
