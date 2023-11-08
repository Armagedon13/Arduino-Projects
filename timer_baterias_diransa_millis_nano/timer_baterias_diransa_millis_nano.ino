const int rele1 = 12;  // The rele1 to which the LED is connected
const int rele2 = 13;  // The rele2 to which the LED is connected
const unsigned long intervalOn = 2000;  // LED on time in milliseconds (2 seconds)
const unsigned long intervalOn1 = 2000;
const unsigned long intervalOn2 = 3000;
const unsigned long intervalOn3 = 2000;
const unsigned long intervalOn4 = 2000;

//const unsigned long intervalOff = 43200000;  // LED off interval in milliseconds (12 hours)
const unsigned long intervalOff = 60000; // 1 minute

unsigned long previousMillis = 0;  // Stores the last time LED was updated
unsigned long ledStartTime = 0;    // Stores the start time of LED activation

void setup() {
  pinMode(rele1, OUTPUT);
  pinMode(rele2, OUTPUT);
  digitalWrite(rele1, HIGH);
  digitalWrite(rele2, HIGH);
  // LOW es ALTO y HIGH es BAJO
  delay(5000);
  digitalWrite(rele1, HIGH);
  digitalWrite(rele2, LOW);
  delay(3000);
  digitalWrite(rele2, HIGH);
  delay(2000);
  digitalWrite(rele2, LOW);
  delay(2000);
  digitalWrite(rele2, HIGH);
  delay(3000);
  Serial.begin(9600);
  Serial.println("check 0");
}

void loop() {
  static int sequenceStep = 0;
  unsigned long currentMillis = millis();
  static unsigned long previousMillis = 0;
  static unsigned long ledStartTime = 0;
  static bool rele1State = true;

  if (rele1State == true) {
    // LED está apagado, verificar si es hora de encenderlo
    if (currentMillis - previousMillis >= intervalOff) {
      // Guardar la última vez que se encendió el LED
      previousMillis = currentMillis;

      // Encender el LED
      digitalWrite(rele1, LOW);
      rele1State = false;
      ledStartTime = currentMillis; // Almacenar el tiempo de activación del LED
      Serial.println("check 1");
    }
  } else {
    // LED está encendido, verificar en qué etapa de la secuencia estamos
    if (sequenceStep == 0) {
      if (currentMillis - ledStartTime >= intervalOn) {
        digitalWrite(rele1, HIGH);
        sequenceStep = 1;
        Serial.println("check 2");
      }
    }
    if (sequenceStep == 1) {
      if (currentMillis - ledStartTime >= intervalOn + intervalOn1) {
        digitalWrite(rele2, LOW);
        sequenceStep = 2;
        Serial.println("check 3");
      }
    }
    else if (sequenceStep == 2) {
      if (currentMillis - ledStartTime >= intervalOn + intervalOn1 + intervalOn2) {
        digitalWrite(rele2, HIGH);
        sequenceStep = 3;
        Serial.println("check 4");
      }
    } 
    else if (sequenceStep == 3) {
      if (currentMillis - ledStartTime >= intervalOn + intervalOn1 + intervalOn2 + intervalOn3) {
        digitalWrite(rele2, LOW);
        sequenceStep = 4; 
        Serial.println("check 5");
      }
    }
    else if (sequenceStep == 4) {
      if (currentMillis - ledStartTime >= intervalOn + intervalOn1 + intervalOn2 + intervalOn3) {
        digitalWrite(rele2, HIGH);
        sequenceStep = 0; // Reiniciar la secuencia desde el principio
        rele1State = true;
        Serial.println("check 6");
      }
    }
  }
}