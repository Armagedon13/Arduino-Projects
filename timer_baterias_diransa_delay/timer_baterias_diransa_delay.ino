const int rele1 = 0;  // El rele1 al que está conectado el LED
const int rele2 = 1;  // El rele2 al que está conectado el LED
//const unsigned long interval1Minute = 60000; // 1 minuto
//const unsigned long interval12Hours = 43200000UL; // 12 horas en milisegundos
unsigned long interval12Hours = 12UL * 60UL * 60UL * 1000UL; // 12 horas en milisegundos

void setup() {
  pinMode(rele1, OUTPUT);
  pinMode(rele2, OUTPUT);
  digitalWrite(rele1, HIGH);
  digitalWrite(rele2, HIGH);
  // LOW es ALTO y HIGH es BAJO
  delay(5000);

  digitalWrite(rele2, LOW);
  delay(3000);
  digitalWrite(rele2, HIGH);
  delay(2000);
  digitalWrite(rele2, LOW);
  delay(2000);
  digitalWrite(rele2, HIGH);
  delay(3000);
  
}

void loop() {
  // Ejecutar el ciclo completo
  
  // Esperar 12 horas antes de comenzar el ciclo nuevamente
  delay(interval12Hours);
  // Espera 1 minuto
  //delay(interval1Minute);
  digitalWrite(rele1, LOW);
  delay(2000);
  digitalWrite(rele1, HIGH);
  delay(1000);
  digitalWrite(rele2, LOW);
  delay(3000);
  digitalWrite(rele2, HIGH);
  delay(2000);
  digitalWrite(rele2, LOW);
  delay(2000);
  digitalWrite(rele2, HIGH);
}