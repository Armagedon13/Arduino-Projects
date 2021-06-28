
#include <Average.h>
void setup() {
  //Se inicia la comunicación serial
  Serial.begin(9600);
}
 
void loop() {
  /*
    Se crea una instancia de la librería,
    con un tamaño de buffer de 10 datos tipo float
  */
  Average<int> ave(10);
  /*
     Variables para almacenamiento del índice del
     valor minimo y maximo
  */
  int minat = 0;
  int maxat = 0;
  //Se imprime la inscripcion Data Series
  Serial.println("Data Series");
  /*
     Se generan 10 numeros aleatorios y se almacenan
     en el buffer. A la vez, se imprimen los valores
     generados
  */
  for (int i = 0; i < 10; i++) {
    int num = rand();
    ave.push(num);
    Serial.print(num);
    Serial.print(" ");
  }
  /*
     Se imprimen los resultados obtenidos a partir
     de la serie de datos generada en el ciclo for
  */
  Serial.println();
  Serial.print("Mean:   "); Serial.println(ave.mean()); //media
  Serial.print("Mode:   "); Serial.println(ave.mode()); //moda
  Serial.print("Max:    "); Serial.println(ave.maximum(&maxat)); //valor maximo
  Serial.print(" at:    "); Serial.println(maxat); //indice del valor maximo
  Serial.print("Min:    "); Serial.println(ave.minimum(&minat)); //valor minimo
  Serial.print(" at:    "); Serial.println(minat); //indice del valor minimo
  Serial.print("StdDev: "); Serial.println(ave.stddev()); //desviacion estandar
  //El proceso se repite cada 10 segundos
  delay(10000);
}
