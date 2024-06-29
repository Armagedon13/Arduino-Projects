#include <SPI.h>
#include <Ethernet.h>
#include <esp_now.h>
#include <WiFi.h>

typedef struct struct_message {
  char data[250];
  size_t len;
} struct_message;

struct_message incomingData;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; 
IPAddress ip(192, 168, 1, 178);
EthernetClient client;
EthernetServer server(80);

// Definir pines para LEDs
const int greenLedPin = 2;
const int redLedPin = 4;
const int activityLedPin = 12;

// Variables de tiempo
unsigned long previousMillisGreen = 0;
unsigned long previousMillisActivity = 0;
const long intervalGreen = 500;
const long intervalActivity = 50;
bool greenLedState = LOW;
bool activityLedState = LOW;

void onDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingData, incomingData, sizeof(incomingData));
  if (client.connected()) {
    client.write(incomingData, len);
    Serial.println("Data received and sent via Ethernet");

    // Parpadear LED de actividad de paquetes
    activityLedState = HIGH;
    previousMillisActivity = millis();
    digitalWrite(activityLedPin, activityLedState);
  } else {
    digitalWrite(redLedPin, HIGH);  // Indicar falla
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(activityLedPin, OUTPUT);

  Ethernet.begin(mac, ip);
  server.begin();
  client = EthernetClient();

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    digitalWrite(redLedPin, HIGH);  // Indicar falla
    return;
  }
  
  esp_now_register_recv_cb(onDataRecv);
  digitalWrite(redLedPin, LOW);  // Apagar LED rojo
}

void loop() {
  unsigned long currentMillis = millis();

  if (!client.connected()) {
    client = server.available();
    if (!client) {
      // Parpadear LED verde mientras busca conexión
      if (currentMillis - previousMillisGreen >= intervalGreen) {
        previousMillisGreen = currentMillis;
        greenLedState = !greenLedState;
        digitalWrite(greenLedPin, greenLedState);
      }
    } else {
      digitalWrite(greenLedPin, HIGH);  // Conexión establecida
    }
  } else {
    digitalWrite(greenLedPin, HIGH);  // Conexión establecida
  }

  // Controlar el tiempo del LED de actividad
  if (currentMillis - previousMillisActivity >= intervalActivity) {
    activityLedState = LOW;
    digitalWrite(activityLedPin, activityLedState);
  }
}
