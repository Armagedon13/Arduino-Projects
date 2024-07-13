/*
Transmisor
*/

#ifndef ETH_PHY_TYPE
#define ETH_PHY_TYPE ETH_PHY_W5500
#define ETH_PHY_ADDR 1
#define ETH_PHY_CS 15
#define ETH_PHY_IRQ 4
#define ETH_PHY_RST 5
#endif

// SPI pins
#define ETH_SPI_SCK 14
#define ETH_SPI_MISO 12
#define ETH_SPI_MOSI 13

#include <ETH.h>
#include <SPI.h>
#include <NetworkUdp.h>

#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>  // only for esp_wifi_set_channel()

#include <ArduinoOTA.h>

#include <ezLED.h>  // ezLED library

// Definir pines para LEDs y switch
/*
led rojo: error
led verde: estado espnow
led amarillo: datos
*/

#define redLedPin 19
#define greenLedPin 23
#define activityLedPin 22

ezLED LED1(redLedPin);
ezLED LED2(greenLedPin);
ezLED LED3(activityLedPin);

#define switchPin 21  // Pin del interruptor

bool programmingMode = false;

static bool eth_connected = false;

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
      Serial.println("WiFi AP access point stopped");
      break;
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
      Serial.println("Client AP_STA connected");
      break;
    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
      Serial.println("Client AP_STA disconnected");
      break;
    case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
      Serial.println("Assigned AP_STA IP address to client");
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

// ESP NOW ------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct struct_message {
  char data[250];
} struct_message;

struct_message payload;

// Global copy of peerInfo
#define CHANNEL 6

esp_now_peer_info_t peerInfo;

//Transmisor c8:f0:9e:52:78:94
byte mac[] = { 0xc8, 0xf0, 0x9e, 0x52, 0x78, 0x94 };

//Receptor c8:f0:9e:52:e9:6c
uint8_t broadcastAddress[] = { 0xc8, 0xf0, 0x9e, 0x52, 0xe9, 0x6c };  // MAC del segundo ESP32

// ESP-NOW Hearthbeat--------------------------------------------------------------------------------------------------------------------------------------------
#define HEARTBEAT_INTERVAL 5000   // Send heartbeat every 5 seconds
#define CONNECTION_TIMEOUT 10000  // Consider disconnected if no heartbeat for 15 seconds

unsigned long lastHeartbeatSent = 0;
unsigned long lastHeartbeatReceived = 0;
bool isConnected = false;

typedef struct struct_heartbeat {
  uint32_t timestamp;
} struct_heartbeat;

// Function to send heartbeat
void sendHeartbeat() {
  if (esp_now_is_peer_exist(broadcastAddress) == false) {
    Serial.println("Peer not found, re-adding...");
    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    esp_now_add_peer(&peerInfo);
  }

  struct_heartbeat heartbeat;
  heartbeat.timestamp = millis();
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&heartbeat, sizeof(struct_heartbeat));
  if (result == ESP_OK) {
    Serial.println("Heartbeat sent to Receiver");
  } else {
    Serial.print("Error sending heartbeat to Receiver: ");
    Serial.println(esp_err_to_name(result));
    LED1.blink(500, 500);
    LED2.turnOFF();
  }
}

void checkConnectionStatus() {
  if (millis() - lastHeartbeatReceived > CONNECTION_TIMEOUT) {
    if (isConnected) {
      isConnected = false;
      Serial.println("Connection to Receiver lost");
      LED1.blink(500, 500);
      LED2.turnOFF();
    }
  } else {
    if (!isConnected) {
      isConnected = true;
      Serial.println("Connection to Receiver established");
      LED2.blink(100, 500);
      LED1.turnOFF();
    }
  }
}

// ESP-NOW DATASEND and DATARECIVE --------------------------------------------------------------------------------------------------------------------------------------------------------------------
// callback when data is sent
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nEstado del último paquete enviado:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Envío exitoso" : "Fallo en el envío");
  if (status == ESP_NOW_SEND_SUCCESS) {
    LED3.turnON();
    LED3.turnOFF();
  }
}
// callback when data is recived
void onDataReceive(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  if (len == sizeof(struct_heartbeat)) {
    lastHeartbeatReceived = millis();
    Serial.println("Heartbeat received from Receiver");
  }
}

// OTA -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const char *ssid = "Transmisor PGR-MODE-1";
const char *password = "lalaland";

// OTA AP Static IP configuration
IPAddress local_ip_AP(192, 168, 4, 22);
IPAddress gateway_AP(192, 168, 4, 9);
IPAddress subnet_AP(255, 255, 255, 0);

// Ethernet --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
NetworkUDP Udp;                    // create UDP object
unsigned int localUdpPort = 1001;  // Local port number

IPAddress local_ip(192, 168, 0, 140);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

IPAddress dns1(8, 8, 8, 8);             //optional
IPAddress dns2 = (uint32_t)0x00000000;  //optional


// Setup -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  LED1.turnOFF();
  LED2.turnOFF();
  LED3.turnOFF();
  pinMode(switchPin, INPUT_PULLUP);  // Configurar el pin del interruptor

  // Ethernet INIT
  Network.onEvent(onEvent);

  SPI.begin(ETH_SPI_SCK, ETH_SPI_MISO, ETH_SPI_MOSI);
  ETH.begin(ETH_PHY_TYPE, ETH_PHY_ADDR, ETH_PHY_CS, ETH_PHY_IRQ, ETH_PHY_RST, SPI);
  ETH.config(local_ip, gateway, subnet,dns1);  // Static IP, leave without this line to get IP via DHCP
  Udp.begin(localUdpPort);  // Enable UDP listening to receive data
  delay(1000);

  checkSwitch();
  // Initialize based on initial switch state
  if (programmingMode) {
    startProgrammingMode();
  } else {
    initEspNow();
  }
}

// Loop ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop() {
  // Loop variables
  LED1.loop();
  LED2.loop();
  LED3.loop();
  checkSwitch();
  unsigned long currentMillis = millis();
  // OTA handle
  if (programmingMode) {
    ArduinoOTA.handle();
  }
  else{
    // Heartbeat Watchdog
    if (currentMillis - lastHeartbeatSent >= HEARTBEAT_INTERVAL) {
    sendHeartbeat();
    lastHeartbeatSent = currentMillis;
  }
  checkConnectionStatus();
  }

  // Ethernet data udp handle
  if (eth_connected) {
    int packetSize = Udp.parsePacket();  // Get the current team header packet length
    if (packetSize) {                    // If data is available
      // int len = Udp.read(payload.data, 250);
      // if (len > 0) {
      //   payload.data[len] = '\0';
      // }
      // ADD IT LATER
      char incomingData[250];
      int len = Udp.read(incomingData, 250);
      if (len > 0) {
        incomingData[len] = '\0';
      }
      Serial.print("Received from weight scaler: ");
      Serial.println(incomingData);

      //Send the data over ESP-NOW
      struct_message payload;
      strncpy(payload.data, incomingData, sizeof(payload.data) - 1);
      payload.data[sizeof(payload.data) - 1] = '\0';

      Serial.println();
      Serial.print("Received: ");
      Serial.println(payload.data);
      Serial.print("From IP: ");
      Serial.println(Udp.remoteIP());
      Serial.print("From Port: ");
      Serial.println(Udp.remotePort());

      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&payload, sizeof(payload));
      if (result == ESP_OK) {
        Serial.println("Envío exitoso");
        LED3.toggle();
        LED1.turnOFF();
      } else {
        Serial.println("Error al enviar los datos");
        LED1.blink(100, 500);
        LED3.turnOFF();
      }
    }
  }
}

void checkSwitch() {
  static unsigned long lastDebounceTime = 0;
  static int lastSwitchState = LOW;
  static const unsigned long debounceDelay = 50;

  int reading = digitalRead(switchPin);

  if (reading != lastSwitchState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != programmingMode) {
      programmingMode = reading;
      if (programmingMode) {
        startProgrammingMode();
      } else {
        stopProgrammingMode();
      }
      delay(100);  // Add a small delay after mode change
    }
  }

  lastSwitchState = reading;
}

void startProgrammingMode() {
  LED1.turnOFF();
  LED2.turnOFF();
  LED3.turnOFF();
  delay(10);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(local_ip_AP, gateway_AP, subnet_AP);
  WiFi.softAP(ssid, password);
  setupOTA();

  Serial.println("Programming mode started. WiFi AP active.");
  LED1.blink(500, 500);
  LED2.blink(500, 500);  // Blink green LED to indicate programming mode
  LED3.blink(500, 500);
}

void stopProgrammingMode() {
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_OFF);
  delay(1000);
  ArduinoOTA.end();
  LED1.turnOFF();
  LED2.turnOFF();  // Turn off green LED
  LED3.turnOFF();

  initEspNow();

  Serial.println("Programming mode stopped. ESP-NOW active.");
}

void initEspNow() {
  WiFi.mode(WIFI_STA);
  delay(1000);
  esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE);
  // This is the mac address of the Master in Station Mode
  Serial.print("STA MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.print("STA CHANNEL ");
  Serial.println(WiFi.channel());

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    LED1.blink(100, 100);  // Blink red LED to indicate error
    LED2.turnOFF();
    return;
  }

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    LED1.blink(200, 200);
    LED2.turnOFF();
    return;
  } else{
    LED2.blink(250, 750);  // Blink green LED to indicate ESP-NOW is paired
    LED1.turnOFF();
  }

  esp_now_register_send_cb(onDataSent);
  esp_now_register_recv_cb(onDataReceive);
  Serial.println("ESP-NOW initialized");
}

// Setup Ota -------------------------------------------------------------------------------------------------------------------------------------------------------------------
void setupOTA() {
  ArduinoOTA.setHostname("esp32-transmisor");
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