const int numLectura = 10;

int lectura[numLectura];      // the readings from the analog input
int leerIndex = 0;              // the index of the current reading
int resultado = 0;                  // the running total
int average = 0;                // the average

int inputPin = A0;

void setup() {
  // initialize serial communication with computer:
  Serial.begin(9600);
  // initialize all the readings to 0:
  for (int esteReading = 0; esteReading < numLectura; esteReading++) {
    lectura[esteReading] = 0;
  }
}

void loop() {
  // subtract the last reading:
  resultado = resultado - lectura[leerIndex];
  // read from the sensor:
  lectura[numLectura] = analogRead(inputPin);
  // add the reading to the total:
  resultado = resultado + lectura[leerIndex];
  // advance to the next position in the array:
  leerIndex = leerIndex+ 1;

  // if we're at the end of the array...
  if (leerIndex >= numLectura) {
    // ...wrap around to the beginning:
    leerIndex = 0;
  }

  // calculate the average:
  average = resultado / numLectura;
  // send it to the computer as ASCII digits
  Serial.println(average);
  delay(1);        // delay in between reads for stability
}


