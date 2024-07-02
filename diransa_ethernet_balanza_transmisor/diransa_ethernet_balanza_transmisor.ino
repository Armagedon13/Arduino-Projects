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
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

//c8:f0:9e:52:e9:6c
uint8_t broadcastAddress[] = {0xc8, 0xf0, 0x9e, 0x52, 0xe9, 0x6c}; // MAC del segundo ESP32

typedef struct struct_message {
  char data[250];
  size_t len;
  char clientIP[16];  // Para almacenar la IP del cliente
} struct_message;

struct_message myData;

// Definir pines para LEDs y switch
const int greenLedPin = 2;
const int redLedPin = 12;
const int activityLedPin = 14;
const int switchPin = 13; // Pin del interruptor

// Variables de tiempo
unsigned long previousMillisGreen = 0;
unsigned long previousMillisActivity = 0;

const long intervalGreen = 500;
const long intervalActivity = 50;

bool greenLedState = LOW;
bool activityLedState = LOW;

bool switchState = LOW;
bool previousSwitchState = LOW;

static bool eth_connected = false;

// WARNING: onEvent is called from a separate FreeRTOS task (thread)!
void onEvent(arduino_event_id_t event) {
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      Serial.println("ETH Started");
      // The hostname must be set after the interface is started, but needs
      // to be set before DHCP, so set it from the event handler thread.
      ETH.setHostname("esp32-ethernet");
      break;
    case ARDUINO_EVENT_ETH_CONNECTED: Serial.println("ETH Connected"); break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      Serial.println("ETH Got IP");
      Serial.println(ETH);
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

esp_now_peer_info_t peerInfo;

WiFiServer server(80);

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// OTA
const char *ssid = "programming mode";
const char *password = "lalaland";

// Static IP configuration
IPAddress local_ip(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

int ssid_hidden_state = 1;

void setup() {
  Serial.begin(115200);
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(activityLedPin, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP); // Configurar el pin del interruptor

  WiFi.mode(WIFI_AP_STA);
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  Network.onEvent(onEvent);
  ETH.begin();
}

void loop() {
  ArduinoOTA.handle();
  unsigned long currentMillis = millis();

  switchState = digitalRead(switchPin);
  if (switchState == LOW && previousSwitchState == LOW){
    WiFi.softAPConfig(local_ip, gateway, subnet);
    WiFi.softAP(ssid, password, 0);
    Serial.print("AP IP:");
    Serial.println(WiFi.softAPIP());
    Serial.println("show SSid");
    previousSwitchState = HIGH;
  }
  else if (switchState == HIGH && previousSwitchState == HIGH){
    WiFi.softAPConfig(local_ip, gateway, subnet);
    WiFi.softAP(ssid, password, 1);
    Serial.print("AP IP:");
    Serial.println(WiFi.softAPIP());
    Serial.println("hide SSid");
    previousSwitchState = LOW;
  }

  if (eth_connected) {
    WiFiClient client = server.accept(); // Aceptar conexión de cliente
    if (client) {
      digitalWrite(greenLedPin, HIGH);  // Indicar conexión establecida
      while (client.connected()) {
        if (client.available()) {
          myData.len = client.readBytes(myData.data, sizeof(myData.data));

          // Obtener la IP del cliente
          IPAddress clientIP = client.remoteIP();
          snprintf(myData.clientIP, sizeof(myData.clientIP), "%d.%d.%d.%d", clientIP[0], clientIP[1], clientIP[2], clientIP[3]);

          esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
          Serial.println("Data sent via ESP-NOW");

          // Parpadear LED de actividad de paquetes
          activityLedState = HIGH;
          previousMillisActivity = currentMillis;
          digitalWrite(activityLedPin, activityLedState);
        }
        // Controlar el tiempo del LED de actividad
        if (currentMillis - previousMillisActivity >= intervalActivity) {
          activityLedState = LOW;
          digitalWrite(activityLedPin, activityLedState);
        }
      }
      client.stop();
    } else {
      // Parpadear LED verde mientras busca conexión
      if (currentMillis - previousMillisGreen >= intervalGreen) {
        previousMillisGreen = currentMillis;
        greenLedState = !greenLedState;
        digitalWrite(greenLedPin, greenLedState);
      }
    }
  } else {
    digitalWrite(redLedPin, HIGH);  // Indicar que no hay conexión Ethernet
  }
}

void setupOTA() {
  ArduinoOTA.setHostname("esp32");
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }

  });
  ArduinoOTA.begin();
}