void setup()
{
  DDRB = 32;
}

void loop()
{
  PORTB = 32; //Turn LED on.
  delay(100);
  PORTB = 0; //Turn LED off.
  delay(100);
}
