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

#include <WiFi.h>
#include "ESP32_NOW.h"
#include <esp_wifi.h>

#include "esp_system.h"
#include "rom/ets_sys.h"

#include <ezLED.h>  // ezLED library

// Watchdog
const int button = 0;         //gpio to use to trigger delay
const int wdtTimeout = 3000;  //time in ms to trigger the watchdog
hw_timer_t *timer = NULL;

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

static bool eth_connected = false;

void ARDUINO_ISR_ATTR resetModule() {
  ets_printf("reboot\n");
  esp_restart();
}

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
      ETH.setHostname("esp32-TRA_1-ethernet");
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
//byte mac[] = { 0xc8, 0xf0, 0x9e, 0x52, 0x78, 0x94 };

//Receptor c8:f0:9e:52:e9:6c
uint8_t broadcastAddress[] = { 0xc8, 0xf0, 0x9e, 0x52, 0xe9, 0x6c };  // MAC del segundo ESP32

// ESP-NOW Hearthbeat--------------------------------------------------------------------------------------------------------------------------------------------
#define HEARTBEAT_INTERVAL 10000  // Send heartbeat every 10 seconds
#define CONNECTION_TIMEOUT 30000 

unsigned long lastHeartbeatSent = 0;
unsigned long lastHeartbeatReceived = 0;
bool isConnected = false;

typedef struct struct_heartbeat {
  uint8_t flag;
} struct_heartbeat;

void sendHeartbeat() {
  struct_heartbeat heartbeat;
  heartbeat.flag = 1;  // Any non-zero value works as a flag

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&heartbeat, sizeof(struct_heartbeat));
  if (result == ESP_OK) {
    Serial.println("Heartbeat sent");
  } else {
    Serial.println("Error sending heartbeat");
    LED1.blink(500, 500);
    LED2.turnOFF();
  }
}

void checkConnectionStatus() {
  unsigned long currentTime = millis();
  if (currentTime - lastHeartbeatReceived > CONNECTION_TIMEOUT) {
    if (isConnected) {
      isConnected = false;
      Serial.println("Connection lost");
      LED1.blink(500, 500);
      LED2.turnOFF();
    }
  } else if (!isConnected) {
    isConnected = true;
    Serial.println("Connection established");
    LED2.blink(100, 500);
    LED1.turnOFF();
  }
}

// ESP-NOW DATASEND and DATARECIVE --------------------------------------------------------------------------------------------------------------------------------------------------------------------
// callback when data is sent
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nEstado del último paquete enviado:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Envío exitoso" : "Fallo en el envío");
}
// callback when data is recived
// Modify the onDataReceive function
void onDataReceive(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  if (len == sizeof(struct_heartbeat)) {
    lastHeartbeatReceived = millis();
    Serial.println("Heartbeat received");
  }
}

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

  // ESP NOW INIT
  WiFi.mode(WIFI_STA);
  esp_wifi_set_protocol(WIFI_IF_STA  , WIFI_PROTOCOL_LR);
  esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE);
  while (!WiFi.STA.started()) {
    delay(100);
  }
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

  // Ethernet INIT
  Network.onEvent(onEvent);

  SPI.begin(ETH_SPI_SCK, ETH_SPI_MISO, ETH_SPI_MOSI);
  ETH.begin(ETH_PHY_TYPE, ETH_PHY_ADDR, ETH_PHY_CS, ETH_PHY_IRQ, ETH_PHY_RST, SPI);
  ETH.config(local_ip, gateway, subnet, dns1);  // Static IP, leave without this line to get IP via DHCP
  Udp.begin(local_ip, localUdpPort);  // Enable UDP listening to receive data
  delay(2000);

  timer = timerBegin(1000000);                     //timer 1Mhz resolution
  timerAttachInterrupt(timer, &resetModule);       //attach callback
  timerAlarm(timer, wdtTimeout * 1000, false, 0);  //set time in us

}

// Loop ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop() {
  // Loop variables
  timerWrite(timer, 0);  //reset timer (feed watchdog)
  LED1.loop();
  LED2.loop();
  LED3.loop();
  unsigned long currentMillis = millis();
  if (currentMillis - lastHeartbeatSent >= HEARTBEAT_INTERVAL) {
    sendHeartbeat();
    lastHeartbeatSent = currentMillis;
  }
  checkConnectionStatus();
  

  // Ethernet data udp handle
  if (eth_connected) {
    int packetSize = Udp.parsePacket();  // Get the current team header packet length
    if (packetSize) {                    // If data is available
      char incomingData[250];
      int len = Udp.read(incomingData, 250);
      if (len > 0) {
        incomingData[len] = '\0';
      }
      //Send the data over ESP-NOW
      struct_message payload;
      strncpy(payload.data, incomingData, sizeof(payload.data));
      payload.data[sizeof(payload.data) -1] = '\0';

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
        LED3.blinkNumberOfTimes(50, 50, 1);  // Blink activity LED
        LED1.turnOFF();
      } else {
        Serial.println("Error al enviar los datos");
        LED1.blink(100, 500);
        LED3.turnOFF();
      }
    }
  }

}