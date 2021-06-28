bool estadoActual = false, estadoUltimo, contador = false;
const byte LED = 13;
const byte boton = 6;

void setup()
{
  pinMode(LED, OUTPUT); // Pin 113 como salida, para el Led.
  pinMode(boton, INPUT_PULLUP); // Pin 2 como entrada.
  Serial.begin(115200); // Velocidad en baudios para el puerto serie.
}

void loop()
{
  estadoActual = digitalRead(boton); // Guarda el estado del pulsador.
  //delay(50); // Retardo de 50 mili segundos par evitar antirebotes.

  // ¿Pulsador y estadoActual y negación del estadoUltimo es verdadero?
  if (digitalRead(boton) && estadoActual && !estadoUltimo)
  {
    contador = !contador;   // Cambio el estado tipo boleano.

    if (contador)
    {
      
      digitalWrite(LED, HIGH);
      delay(500);
      digitalWrite(LED, LOW);
      
      Serial.write("ON"); // Envía por el puerto ON.
    }
    else
    {
      
      digitalWrite(LED, HIGH);
      delay(500);
      digitalWrite(LED, LOW);
      
      Serial.write("OFF"); // Envía por el puerto OFF.
    }
  }

  // Pasa del estadoActual a estadoUltimo.
  estadoUltimo = estadoActual;
}
