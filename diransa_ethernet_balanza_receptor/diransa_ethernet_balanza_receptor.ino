/* 
   * ETH_CLOCK_GPIO0_IN   - default: external clock from crystal oscillator
   * ETH_CLOCK_GPIO0_OUT  - 50MHz clock from internal APLL output on GPIO0 - possibly an inverter is needed for LAN8720
   * ETH_CLOCK_GPIO16_OUT - 50MHz clock from internal APLL output on GPIO16 - possibly an inverter is needed for LAN8720
   * ETH_CLOCK_GPIO17_OUT - 50MHz clock from internal APLL inverted output on GPIO17 - tested with LAN8720
*/
#ifndef ETH_PHY_TYPE
  #define ETH_PHY_TYPE        ETH_PHY_LAN8720
  #define ETH_PHY_ADDR         1
  #define ETH_PHY_MDC         23
  #define ETH_PHY_MDIO        18
  #define ETH_PHY_POWER       -1
  #define ETH_CLK_MODE        ETH_CLOCK_GPIO17_OUT
  #define ETH_PHY_RST_GPIO    -1 // not connected
#endif

#include <ETH.h>
#include <esp_now.h>
#include <WiFi.h>
#include <ArduinoOTA.h>

typedef struct struct_message {
  char data[250];
  size_t len;
  char clientIP[16];  // Para almacenar la IP del cliente
} struct_message;

struct_message incomingData;

static bool eth_connected = false;

// Definir pines para LEDs y switch
const int greenLedPin = 2;
const int redLedPin = 4;
const int activityLedPin = 12;
const int switchPin = 13; // Pin del interruptor

// Variables de tiempo
unsigned long previousMillisGreen = 0;
unsigned long previousMillisActivity = 0;
const long intervalGreen = 500;
const long intervalActivity = 50;
bool greenLedState = LOW;
bool activityLedState = LOW;

WiFiServer server(80); // Servidor en el puerto 80

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
      server.begin(); // Iniciar el servidor
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

void onDataRecv(const esp_now_recv_info *info, const uint8_t *data, int len) {
  memcpy(&incomingData, data, len);

  if (eth_connected) {
    WiFiClient client = server.available(); // Aceptar conexión de cliente
    if (client) {
      client.write((uint8_t*)&incomingData, sizeof(incomingData));
      Serial.println("Data received and sent via Ethernet");

      // Parpadear LED de actividad de paquetes
      activityLedState = HIGH;
      previousMillisActivity = millis();
      digitalWrite(activityLedPin, activityLedState);

      // Mostrar la IP del cliente y los datos de la báscula
      Serial.print("Client IP: ");
      Serial.println(incomingData.clientIP);
      Serial.print("Data: ");
      Serial.write((uint8_t*)incomingData.data, incomingData.len);
      Serial.println();
    } else {
      digitalWrite(redLedPin, HIGH);  // Indicar falla
    }
  } else {
    digitalWrite(redLedPin, HIGH);  // Indicar que no hay conexión Ethernet
  }
}

// void setupOTA() {
//   // Configurar y empezar OTA
//   ArduinoOTA.setHostname("esp32-receiver-OTA");
//   ArduinoOTA.onStart([]() {
//     String type;
//     if (ArduinoOTA.getCommand() == U_FLASH) {
//       type = "sketch";
//     } else { // U_SPIFFS
//       type = "filesystem";
//     }
//     // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
//     Serial.println("Start updating " + type);
//   });
//   ArduinoOTA.onEnd([]() {
//     Serial.println("\nEnd");
//   });
//   ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
//     Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
//   });
//   ArduinoOTA.onError([](ota_error_t error) {
//     Serial.printf("Error[%u]: ", error);
//     if (error == OTA_AUTH_ERROR) {
//       Serial.println("Auth Failed");
//     } else if (error == OTA_BEGIN_ERROR) {
//       Serial.println("Begin Failed");
//     } else if (error == OTA_CONNECT_ERROR) {
//       Serial.println("Connect Failed");
//     } else if (error == OTA_RECEIVE_ERROR) {
//       Serial.println("Receive Failed");
//     } else if (error == OTA_END_ERROR) {
//       Serial.println("End Failed");
//     }
//   });
//   ArduinoOTA.begin();
// }

void setup() {
  Serial.begin(115200);
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(activityLedPin, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP); // Configurar el pin del interruptor

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

  // if (digitalRead(switchPin) == LOW) { // Si el interruptor está activado
  //   if (WiFi.status() != WL_CONNECTED) {
  //     WiFi.begin("OTA_AP_SSID", "OTA_AP_PASSWORD"); // Configura tu SSID y contraseña
  //     while (WiFi.status() != WL_CONNECTED) {
  //       delay(500);
  //       Serial.print(".");
  //     }
  //     Serial.println("Connected to WiFi");
  //     setupOTA();
  //   }
  //   ArduinoOTA.handle();
  // } else {
  //   if (WiFi.status() == WL_CONNECTED) {
  //     WiFi.disconnect();
  //   }
  // }

  if (eth_connected) {
    WiFiClient client = server.available(); // Aceptar conexión de cliente
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
