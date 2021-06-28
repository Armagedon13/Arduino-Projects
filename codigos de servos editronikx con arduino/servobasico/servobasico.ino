
int const servo = 10;
void setup() {
 pinMode (servo, OUTPUT);
}

void loop() {
  
 //  {==[=======> (180°) <=======]==}
 for (int Hz =0; Hz < 50 ;Hz++){      // repetimos la instruccion 50 veces
digitalWrite (servo,HIGH); 
delayMicroseconds(2200);               // llevamos a 180°
digitalWrite (servo,LOW);
delay(18);
 }
delay(2000);                          // retardo

// {==[=======> (90°) <=======]==}
 for (int Hz =0; Hz < 50 ;Hz++){     // repetimos la instruccion 50 veces
digitalWrite (servo,HIGH);
delayMicroseconds(1000);                // llevamos a 90°
digitalWrite (servo,LOW);
delay(19);
 }
delay(2000);

// {==[=======> (0°) <=======]==}
 for (int Hz =0; Hz < 50 ;Hz++){      // repetimos la instruccion 50 veces
digitalWrite (servo,HIGH);
delayMicroseconds(544);                // llevamos a 0°
digitalWrite (servo,LOW);
delay(20);  
 }
 delay(2000);
}
