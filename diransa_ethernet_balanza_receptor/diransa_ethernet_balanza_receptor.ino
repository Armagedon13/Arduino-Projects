#ifndef ETH_PHY_TYPE
#define ETH_PHY_TYPE  ETH_PHY_LAN8720
#define ETH_PHY_ADDR  0
#define ETH_PHY_MDC   23
#define ETH_PHY_MDIO  18
#define ETH_PHY_POWER -1
#define ETH_CLK_MODE  ETH_CLOCK_GPIO0_IN
#endif

#include <ETH.h>
#include <esp_now.h>
#include <WiFi.h>

typedef struct struct_message {
  char data[250];
  size_t len;
  char clientIP[16];  // Para almacenar la IP del cliente
} struct_message;

struct_message incomingData;

static bool eth_connected = false;

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

void onEvent(arduino_event_id_t event) {
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      Serial.println("ETH Started");
      ETH.setHostname("esp32-receiver");
      break;
    case ARDUINO_EVENT_ETH_CONNECTED: Serial.println("ETH Connected"); break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      Serial.println("ETH Got IP");
      Serial.println(ETH.localIP());
      eth_connected = true;
      break;
    case ARDUINO_EVENT_ETH_LOST_IP:
      Serial.println("ETH Lost IP");
      eth_connected = false;
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH Disconnected");
      eth_connected = false;
      break;
    case ARDUINO_EVENT_ETH_STOP:
      Serial.println("ETH Stopped");
      eth_connected = false;
      break;
    default: break;
  }
}

void onDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingData, incomingData, sizeof(incomingData));
  if (eth_connected) {
    EthernetClient client = server.available();
    if (client.connected()) {
      client.write(incomingData, len);
      Serial.println("Data received and sent via Ethernet");

      // Parpadear LED de actividad de paquetes
      activityLedState = HIGH;
      previousMillisActivity = millis();
      digitalWrite(activityLedPin, activityLedState);

      // Mostrar la IP del cliente y los datos de la báscula
      Serial.print("Client IP: ");
      Serial.println(incomingData.clientIP);
      Serial.print("Data: ");
      Serial.write(incomingData.data, incomingData.len);
      Serial.println();
    } else {
      digitalWrite(redLedPin, HIGH);  // Indicar falla
    }
  } else {
    digitalWrite(redLedPin, HIGH);  // Indicar que no hay conexión Ethernet
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(activityLedPin, OUTPUT);

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    digitalWrite(redLedPin, HIGH);  // Indicar falla
    return;
  }

  esp_now_register_recv_cb(onDataRecv);
  
  Network.onEvent(onEvent);
  ETH.begin();
}

void loop() {
  unsigned long currentMillis = millis();

  if (eth_connected) {
    EthernetClient client = server.available();
    if (!client.connected()) {
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
    digitalWrite(redLedPin, HIGH);  // Indicar que no hay conexión Ethernet
  }

  // Controlar el tiempo del LED de actividad
  if (currentMillis - previousMillisActivity >= intervalActivity) {
    activityLedState = LOW;
    digitalWrite(activityLedPin, activityLedState);
  }
}
