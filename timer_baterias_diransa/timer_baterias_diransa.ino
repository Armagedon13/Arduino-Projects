const int rele1 = 12;  // The rele1 to which the LED is connected
const int rele2 = 13;  // The rele2 to which the LED is connected
const unsigned long intervalOn = 2000;  // LED on time in milliseconds (2 seconds)
const unsigned long intervalOn1 = 3000;
const unsigned long intervalOn2 = 2000;
const unsigned long intervalOn3 = 3000;

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
}

void loop() {
  unsigned long currentMillis = millis();
  bool rele1State =  true;//digitalRead(rele1);  // Current state of the LED

  if (rele1State == true) {
    // LED is off, check if it's time to turn it on
    if (currentMillis - previousMillis >= intervalOff) {
      // Save the last time LED was turned on
      previousMillis = currentMillis;
      
      // Turn on the LED
      digitalWrite(rele1, LOW);
      digitalWrite(rele2, HIGH);
      rele1State = false;
      ledStartTime = currentMillis;  // Store the LED activation time
      Serial.println("check 1");
    }
  } 
  else {
    // LED is on, check if it's time to turn it off
    if (currentMillis - ledStartTime >= intervalOn) {
      // Save the last time LED was turned off
      previousMillis = currentMillis;
      
      // Turn off the LED
      digitalWrite(rele1, HIGH);
      digitalWrite(rele2, LOW);
      
      // Check if it's time to start the rele2 sequence
      if (currentMillis - ledStartTime >= intervalOn1) {
        // Turn on rele2 for 3 seconds
        digitalWrite(rele2, HIGH);
      }
        
      // Check if it's time to turn off rele2
      if (currentMillis - ledStartTime >= intervalOn2) {
        // Turn off rele2
        digitalWrite(rele2, LOW);
      }
          
      if (currentMillis - ledStartTime >= intervalOn3) {
        digitalWrite(rele2, HIGH);
        rele1State = true;
      } 
    }
  }
}