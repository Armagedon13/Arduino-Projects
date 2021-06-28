#include <Arduino.h>

#include<Servo.h>

Servo ser1;

void setup() {
  
ser1.attach(10);
ser1.write(0);

}

void loop() {
  
ser1.write(180);
delay(3000);

ser1.write(90);
delay(3000);


}
