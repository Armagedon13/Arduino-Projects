#ifndef ETH_PHY_TYPE
#define ETH_PHY_TYPE ETH_PHY_W5500
#define ETH_PHY_ADDR 1
#define ETH_PHY_CS   15
#define ETH_PHY_IRQ  4
#define ETH_PHY_RST  5
#endif

// SPI pins
#define ETH_SPI_SCK  14
#define ETH_SPI_MISO 12
#define ETH_SPI_MOSI 13

#include <ETH.h>
#include <SPI.h>

#include <esp_now.h>
#include <WiFi.h>
#include <NetworkUdp.h>

#include <ArduinoOTA.h>

// Definir pines para LEDs y switch
const int greenLedPin = 2;
const int redLedPin = 23;
const int activityLedPin = 22;
const int switchPin = 21;  // Pin del interruptor

// Variables de tiempo
unsigned long previousMillisGreen = 0;
unsigned long previousMillisActivity = 0;

const long intervalGreen = 500;
const long intervalActivity = 50;

bool greenLedState = LOW;
bool activityLedState = LOW;

bool switchState = LOW;
bool previousSwitchState = LOW;

// ESP NOW
// Define the structure for the received message
typedef struct struct_message {
  char data[250];
} struct_message;

// Create a struct_message instance to hold the received data
struct_message incomingMessage;

#define ESPNOW_WIFI_CHANNEL 6

// Ethernet
bool eth_connected = false;
uint8_t mac[] = { 0xc8, 0xf0, 0x9e, 0x52, 0xe9, 0x6c };  // MAC del segundo ESP32
NetworkUDP Udp;                       // create UDP object

// Dirección IP y puerto del servidor UDP
IPAddress udpServerIP(192, 168, 0, 140);   // Cambia esta dirección IP a la del servidor UDP
unsigned int replyPort = 1001;      // port to send response to

// ALL INTERNET EVENTS
// WARNING: onEvent is called from a separate FreeRTOS task (thread)!
void onEvent(arduino_event_id_t event) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_READY:
      Serial.println("WiFi interface ready");
      break;
    case ARDUINO_EVENT_WIFI_SCAN_DONE:
      Serial.println("Completed scan for access points");
      break;
    case ARDUINO_EVENT_WIFI_STA_START:
      Serial.println("WiFi STA client started");
      break;
    case ARDUINO_EVENT_WIFI_STA_STOP:
      Serial.println("WiFi STA clients stopped");
      break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      Serial.println("Connected to STA access point");
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("Disconnected from STA WiFi access point");
      break;
    case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
      Serial.println("Authentication mode of STA access point has changed");
      break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.print("Obtained STA IP address: ");
      Serial.println(WiFi.localIP());
      break;
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:
      Serial.println("Lost STA IP address and IP address is reset to 0");
      break;
    case ARDUINO_EVENT_WPS_ER_SUCCESS:
      Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
      break;
    case ARDUINO_EVENT_WPS_ER_FAILED:
      Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode");
      break;
    case ARDUINO_EVENT_WPS_ER_TIMEOUT:
      Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode");
      break;
    case ARDUINO_EVENT_WPS_ER_PIN:
      Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode");
      break;
    case ARDUINO_EVENT_WIFI_AP_START:
      Serial.println("WiFi AP access point started");
      break;
    case ARDUINO_EVENT_WIFI_AP_STOP:
      Serial.println("WiFi AP access point  stopped");
      break;
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
      Serial.println("Client AP connected");
      break;
    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
      Serial.println("Client AP disconnected");
      break;
    case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
      Serial.println("Assigned AP IP address to client");
      break;
    case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:
      Serial.println("Received AP probe request");
      break;
    case ARDUINO_EVENT_WIFI_AP_GOT_IP6:
      Serial.println("AP IPv6 is preferred");
      break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
      Serial.println("STA IPv6 is preferred");
      break;
    case ARDUINO_EVENT_ETH_GOT_IP6:
      Serial.println("Ethernet IPv6 is preferred");
      break;

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

// Función de callback para recibir datos ESP-NOW
void onDataReceive(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  memcpy(&incomingMessage, incomingData, sizeof(incomingMessage));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Message: ");
  Serial.println(incomingMessage.data);

  // Enviar datos por Ethernet UDP
  if (eth_connected) {
    Udp.beginPacket(udpServerIP, replyPort);
    Udp.write((const uint8_t*)incomingMessage.data, strlen(incomingMessage.data));
    if (Udp.endPacket()) {
      Serial.println("UDP packet sent successfully");
    } else {
      Serial.println("Failed to send UDP packet");
    }
    Serial.print("Sent to IP: ");
    Serial.print(udpServerIP);
    Serial.print(", port ");
    Serial.println(replyPort);
  }
}

// OTA
const char *ssid = "Receptor PGR-MODE";
const char *password = "lalaland";

// OTA AP Static IP configuration
IPAddress local_ip_AP(192, 168, 4, 23);
IPAddress gateway_AP(192, 168, 4, 10);
IPAddress subnet_AP(255, 255, 255, 0);

void setup() {
  Serial.begin(115200);
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(activityLedPin, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP); // Configurar el pin del interruptor

  // Init in WIFI AP_STA
  WiFi.mode(WIFI_STA);
  WiFi.setChannel(ESPNOW_WIFI_CHANNEL);
  Serial.println("Wi-Fi parameters:");
  Serial.println("  Mode: STA");
  Serial.println("  MAC Address: " + WiFi.macAddress());
  Serial.printf("  Channel: %d\n", ESPNOW_WIFI_CHANNEL);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  // Register the receive callback function
  esp_now_register_recv_cb(onDataReceive);
  Serial.println("ESP-NOW Receiver Initialized");

  Network.onEvent(onEvent);
  
  SPI.begin(ETH_SPI_SCK, ETH_SPI_MISO, ETH_SPI_MOSI);
  ETH.begin(ETH_PHY_TYPE, ETH_PHY_ADDR, ETH_PHY_CS, ETH_PHY_IRQ, ETH_PHY_RST, SPI);
  //ETH.config(local_ip, gateway, subnet, dns1, dns2);  // Static IP, leave without this line to get IP via DHCP

  while(!((uint32_t)ETH.localIP())) // Waiting for IP
  {

  }
  Serial.println("Connected");
  Serial.print("IP Address:");
  Serial.println(ETH.localIP());

}

void loop() {
  // ArduinoOTA.handle();
  // unsigned long currentMillis = millis();

  // switchState = digitalRead(switchPin);
  // if (switchState == HIGH && previousSwitchState == HIGH && WL_DISCONNECTED) {
  //   WiFi.softAPConfig(local_ip_AP, gateway_AP, subnet_AP);
  //   WiFi.softAP(ssid, password, ESPNOW_WIFI_CHANNEL);
  //   Serial.print("AP IP:");
  //   Serial.println(WiFi.softAPIP());
  //   Serial.println("Start AP Programming mode");
  //   setupOTA();
  //   previousSwitchState = LOW;
  // } else if (switchState == LOW && previousSwitchState == LOW && WL_CONNECTED) {
  //   WiFi.softAPdisconnect(true);
  //   Serial.println("Stop AP Programming mode");
  //   previousSwitchState = HIGH;
  // }
}

void setupOTA() {
  ArduinoOTA.setHostname("esp32-receptor");
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_SPIFFS
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