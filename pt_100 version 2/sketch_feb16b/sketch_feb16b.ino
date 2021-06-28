#include <ResponsiveAnalogRead.h>

const int sensorPin1 = A0;

//smooth
 ResponsiveAnalogRead analogOne(A0, true);
 
void setup() {
  Serial.begin(9600);
}

void loop() {
  analogOne.update();
  
  int value = analogOne.getValue();
  float millivolts = (value / 1023.0) * 5000;
  float celsius = millivolts / 10; 

Serial.println(millivolts);

delay(1000);

}
