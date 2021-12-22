int reloj = 12, dato = 13, limpiar = 11, hab1 = 10, hab2 = 9 ; // variables del registro de desplazamiento
int numero = 0;                // variable para los números
int hab[2] = {{10,9}};
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
  {0, 0, 0, 1, 1, 1, 1, 1}
}; // nueve

void setup() {
  for (int i = hab2; i <= dato; i++) // definir los pines de salida
  {
    pinMode(i, OUTPUT);
  }
  digitalWrite(hab1, HIGH);     // pin para habilitación del display (este es el pin común, Cátodo o ánodo)
  digitalWrite(hab2, HIGH);     // pin para habilitación del display (este es el pin común, Cátodo o ánodo)
  digitalWrite(limpiar, LOW); // limpia los datos del registro de desplazamiento
 
}

void loop() {

  multiples(numero);             // función para visualizar el número guardado en la variable "numero"
 
  //delay();            // esperar 1 segundo
  numero = numero + 1;      //
  if (numero == 100)
  {
    numero = 0;
  }
}

int ver(int entrada, int habilitar)       // función para cargar los datos al registro de desplazamiento
{
  digitalWrite(limpiar, LOW);
  delay(1);
  digitalWrite(limpiar, HIGH);
  digitalWrite(hab1, HIGH);
  digitalWrite(hab2, HIGH);
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
    delay(1);
    digitalWrite(reloj, LOW);
  }
  digitalWrite(habilitar, LOW);
}

int multiples(int number)             // funcion para definir las unidades y decenas (así se sabrá que display usar y que digito desplegar)
{
  if (number < 10)          // si el numero es menor que 10 
  {
    visualizacion(0, number);
  }
  else
  {
    visualizacion(floor(number/10),number-(10*floor(number/10)));
  }

}

int visualizacion(int uno, int dos)              // las dos variables de entrada son las unidades y decenas 
{
  for (int i = 0; i <= 100; i++)   // multiplexión
  {
    for(int j=0; j <= sizeof(hab)-1;j++  )
    {
      ver(uno, hab[j]);               // habilitar primer display
      delay(3);
    } 
}


