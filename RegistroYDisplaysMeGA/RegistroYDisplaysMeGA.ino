/* ARDUINO UNO y LEONARDO, PUEDE SER USADO  EN OTROS ARDUINOS PERO SE DEBEN MODIFICAR PINES
 *  Código para manejar N displays siete segmentos, muy simple de usar, 
 *  las conexiones de los segmentos se encuentran en los gráficos adjuntados 
 *  debe ser conectado perfectamente para evitar errores de visualización
 * 
 * Tutorial youtube:  https://youtu.be/7NzCfLf0DmY
 * 
 * PAGINA FACEBOOK: https://www.facebook.com/TutosIngenieria
 * SUSCRIBETE: https://www.youtube.com/user/tutosdeingenieria?sub_confirmation=1
 * BLOG: http://tutosingenieria.blogspot.com/
 * TWITTER: https://twitter.com/TutosIngenieria
 * 
 */


int reloj = 52, dato = 53, limpiar = 51; // variables del registro de desplazamiento
int number = 0;                // variable para los números
int Dnum=5;                    // Número de displays 
int Tmul=4;                    // tiempo de multiplexion en milisegundos
int hab[] = {22,23,24,25,26,27};    // pines que manejan los comunes de los display // el primer digito es el más significativo (debe tenerse en cuenta en la conexión)  
int num[10][8] = {
                    {1, 1, 1, 1, 0, 1, 1, 1},  // cero
                    {0, 0, 0, 1, 0, 0, 0, 1},  // uno
                    {0, 1, 1, 0, 1, 0, 1, 1},  // dos
                    {0, 0, 1, 1, 1, 0, 1, 1},  // tres
                    {0, 0, 0, 1, 1, 1, 0, 1},  // cuatro
                    {0, 0, 1, 1, 1, 1, 1, 0},  // cinco
                    {1, 1, 1, 1, 1, 1, 0, 0},  // seis
                    {0, 0, 0, 1, 0, 0, 1, 1},  // siete
                    {1, 1, 1, 1, 1, 1, 1, 1},  // ocho
                    {0, 0, 0, 1, 1, 1, 1, 1}}; // nueve

void setup() {
  for (int i = 22; i <= dato; i++) // definir los pines de salida
  {
    pinMode(i, OUTPUT);
  }
  for (int j=0;j<=sizeof(hab);j++)
  {
    digitalWrite(hab[j], HIGH);     // Se inhabilitan todos los display
  }
  digitalWrite(limpiar, LOW); // limpia los datos del registro de desplazamiento
  //Serial.begin(9600);
}

void loop() {

  multiples(number);             // función para visualizar el número guardado en la variable "numero"
  number = number + 1;          //
  if (number == 10000)
  {
    number = 0;
  }
}

int ver(int entrada, int habilitar)       // función para cargar los datos al registro de desplazamiento
{
  digitalWrite(limpiar, LOW);
  delay(0.1);
  digitalWrite(limpiar, HIGH);
  for(int j=0; j <= Dnum-1;j++  )
    {
       digitalWrite(hab[j], HIGH);
    } 

  for (int i = 0; i <= 7; i++)
  {
    if (num[entrada][i] == 1)
    {
      digitalWrite(dato, HIGH);
    }
    else
    {
      digitalWrite(dato, LOW);
    }

    digitalWrite(reloj, HIGH);
    delay(0.1);
    digitalWrite(reloj, LOW);
  }
  digitalWrite(habilitar, LOW);
}

int multiples(int number)             // funcion para definir las unidades y decenas (así se sabrá que display usar y que digito desplegar)
{
  int j=0;                            // indice para guardar los datos en el vector
  int Ndisp[Dnum];                    // Array para guardar los digitos de los display
  for(int i=1;i<=Dnum-1;i++)          // ciclo para encontrar el tamaño del número a visualizar 10, 100, 1000, 10000...etc
  {
    if(number<=pow(10,i))             // si el número es menor que 10, 100, 1000...etc
    {
      for(int j=1;j<=Dnum-i ;j++)     // agregar el número de ceros a visualizar ejemplo (3 display, visualizar el número 9)...los display deben mostrar 009
      {
         Ndisp[j]=0;                  // agregar el número de ceros en el vector ejemplo anterior se agrega {0,0}
      }  
      break;                          // romper el ciclo for 
    }
  }
  for(int k=Dnum;k>=j+1;k--)         //  rellenar el vector Ndisp, con los números que tiene el valor number, con el ejemplo anterior falta por llenar el valor {0,0,9}
  {
     Ndisp[k]=number%10;            //  se guarda en el espacio k del vector, el último dígito de number ....el dígito menos significativo
     number=floor(number/10);       //  se elimina el dígito menos significativo para continuar con el proceso 
  }
  visualizacion(Ndisp);             // visualizar el número guardado en el vector Ndisp

}

int visualizacion(int *dispD)              // Ingresa un vector con los valores de cada display
{
  for (int i = 0; i <= 2*Tmul; i++)          // multiplexión
  {
    for(int j=0; j <= Dnum-1;j++  )
    {
      ver(dispD[j+1], hab[j]);            // Habilitar el dato dispD[j+1] en el display hab[j] // el dispD esta hecho con uno index y hab[j] con cero index
      delay(Tmul);
      //Serial.println(dispD[j+1]);         // linea de verificación
    } 
  } 
}


