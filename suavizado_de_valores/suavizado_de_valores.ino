/*
  Suavizado

El siguiente código almacena secuencialmente 10 lecturas desde el sensor analógico en un arrays, uno por uno. Con cada nuevo valor, se genera y se divide la suma de todos los números, produciendo un valor medio que luego puede utilizar para suavizar. Debido a que este promedio se lleva a cabo cada vez que un nuevo valor se añade a la matriz (en lugar de la espera de 10 nuevos valores, por ejemplo) no hay tiempo de retraso en el cálculo de este medio de funcionamiento.

  Lee repetidamente desde una entrada analógica, calculando la media
  y mostrando el resultado por Serial Monitor. Mantiene 10 valores leidos en un    
  arreglo y continuamente hace la media de los valores 
*/

// se usa este valor para determinar el tamaño del arreglo
const int numReadings = 10;

int readings[numReadings];      // la lectura de la entrada analógica
int index = 0;                  // el índice de la lectura actual
int average = 0;                // la media
int total = 0;                


int inputPin = A0;

void setup()
{
  // inicializa la comunicacion serial con la computadora:
  Serial.begin(9600);                  
  // inicializa todos los elementos del arreglo con 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
    readings[thisReading] = 0;          
}

void loop() {
  // resta la última lectura
  total= total - readings[index];        
  // lectura del sensor
  readings[index] = analogRead(inputPin);
  // suma la lectura actual y el total
  total= total + readings[index];      
  // avanza al siguiente elemento del arreglo
  index = index + 1;                    

  // si se llego al final de arreglo (al último elemento)
  if (index >= numReadings)              
    // se vuelve al primer elemento (índice 0)
    index = 0;                          

  // calcula la media
  average = total / numReadings;        
  // envía el resultado a la PC
  Serial.println(average);  
  delay(1);        // retraso entre lectura y lectura para la estabilidad
}
