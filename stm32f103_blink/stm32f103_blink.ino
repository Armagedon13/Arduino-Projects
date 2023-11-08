int p = PC13;

void setup() {
  // put your setup code here, to run once:
  pinMode(p, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(p, HIGH);
  delay(1000);
  digitalWrite(p, LOW);
  delay(1000);
}
