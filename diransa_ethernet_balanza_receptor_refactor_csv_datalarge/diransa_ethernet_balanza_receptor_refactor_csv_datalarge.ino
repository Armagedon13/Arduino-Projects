/*
Receptor
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
#include <esp_wifi.h> // only for esp_wifi_set_channel()

#include <ArduinoOTA.h>

#include <ezLED.h> // ezLED library

// Definir pines para LEDs y switch
/*
led rojo: error
led verde: estado espnow
led amarillo: comunicacion
*/

#define redLedPin 19
#define greenLedPin 23
#define activityLedPin 22

ezLED LED1(redLedPin);
ezLED LED2(greenLedPin);
ezLED LED3(activityLedPin);

#define switchPin 21  // Pin del interruptor

bool programmingMode = false;

bool eth_connected = false;

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

// Ethernet------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Transmisor c8:f0:9e:52:78:94
// { 0xc8, 0xf0, 0x9e, 0x52, 0x78, 0x94 }
// Receptor c8:f0:9e:52:e9:6c
// { 0xc8, 0xf0, 0x9e, 0x52, 0xe9, 0x6c }

uint8_t broadcastAddress[] = { 0xc8, 0xf0, 0x9e, 0x52, 0x78, 0x94 };  // MAC del segundo ESP32
NetworkUDP Udp;                       // create UDP object

// Dirección IP y puerto del servidor UDP
IPAddress udpServerIP(192, 168, 0, 162);   // Cambia esta dirección IP a la del servidor UDP
unsigned int replyPort = 1001;      // port to send response to

// ESP NOW-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Define the structure for the received message
#define MAX_MESSAGE_SIZE 200

typedef struct struct_heartbeat {
  uint32_t timestamp;
} struct_heartbeat;

typedef struct struct_data_chunk {
  uint8_t chunk_id;
  uint8_t total_chunks;
  uint8_t data_length;
  char data[MAX_MESSAGE_SIZE];
} struct_data_chunk;

typedef struct struct_message {
  uint8_t message_type; // 0 for heartbeat, 1 for data chunk
  union {
    struct_heartbeat heartbeat;
    struct_data_chunk data_chunk;
  } content;
} struct_message;
// Create a struct_message instance to hold the received data
struct_message incomingMessage;

#define CHANNEL 6

// ESP-NOW Hearthbeat ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define HEARTBEAT_INTERVAL 5000
#define CONNECTION_TIMEOUT 10000

unsigned long lastHeartbeatSent = 0;
unsigned long lastHeartbeatReceived = 0;
bool isConnected = false;

struct_heartbeat heartbeat;

/*
void sendHeartbeat() {
  if (esp_now_is_peer_exist(broadcastAddress) == false) {
    Serial.println("Peer not found, re-adding...");
    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(esp_now_peer_info_t));
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    esp_err_t addResult = esp_now_add_peer(&peerInfo);
    if (addResult != ESP_OK) {
      Serial.print("Error re-adding peer: ");
      Serial.println(esp_err_to_name(addResult));
      return;
    }
  }

  struct_heartbeat heartbeat;
  heartbeat.timestamp = millis();
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&heartbeat, sizeof(struct_heartbeat));
  if (result == ESP_OK && isConnected) {
    Serial.println("Heartbeat sent to Transmitter");
  } else {
    Serial.print("Error sending heartbeat to Transmitter: ");
    Serial.println(esp_err_to_name(result));
  }
}
*/

void sendHeartbeat() {
  struct_message message;
  message.message_type = 0; // Heartbeat
  message.content.heartbeat.timestamp = millis();
  
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&message, sizeof(struct_message));
  if (result == ESP_OK && isConnected) {
    Serial.println("Heartbeat sent");
  } else {
    Serial.print("Error sending heartbeat: ");
    Serial.println(esp_err_to_name(result));
  }
}

void checkConnectionStatus() {
  unsigned long currentTime = millis();
  if (currentTime - lastHeartbeatReceived > CONNECTION_TIMEOUT) {
    if (isConnected) {
      isConnected = false;
      Serial.println("Connection to Transmitter lost");
      LED1.blink(500, 500);
      LED2.turnOFF();
    }
  } else {
    if (!isConnected) {
      isConnected = true;
      Serial.println("Connection to Transmitter established");
      LED2.blink(100, 500);
      LED1.turnOFF();
    }
  }
}

// ESP-NOW DATASEND DATARECIVE ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define MAX_FULL_MESSAGE_SIZE 10000 // Adjust as needed

char* fullMessage = NULL;
size_t fullMessageSize = 0;
uint16_t receivedChunks = 0;
uint16_t expectedChunks = 0;

void onDataReceive(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  if (len == sizeof(struct_message)) {
    struct_message* message = (struct_message*)incomingData;
    
    if (message->message_type == 0) {
      // Handle heartbeat
      lastHeartbeatReceived = millis();
      Serial.println("Heartbeat received from Transmitter");
    } else if (message->message_type == 1) {
      // Handle data chunk
      struct_data_chunk* chunk = &message->content.data_chunk;
      
      if (chunk->chunk_id == 0) {
        // First chunk, initialize or reallocate fullMessage
        size_t requiredSize = chunk->total_chunks * MAX_MESSAGE_SIZE;
        if (requiredSize > fullMessageSize) {
          if (fullMessage) {
            free(fullMessage);
          }
          fullMessage = (char*)esp_heap_caps_malloc(requiredSize, MALLOC_CAP_8BIT);
          if (!fullMessage) {
            Serial.println("Failed to allocate memory for full message");
            return;
          }
          fullMessageSize = requiredSize;
        }
        receivedChunks = 0;
        expectedChunks = chunk->total_chunks;
      }
      
      if (fullMessage) {
        memcpy(fullMessage + (chunk->chunk_id * MAX_MESSAGE_SIZE), chunk->data, chunk->data_length);
        receivedChunks++;
        
        if (receivedChunks == expectedChunks) {
          // All chunks received, process the full message
          size_t fullLength = (expectedChunks - 1) * MAX_MESSAGE_SIZE + chunk->data_length;
          fullMessage[fullLength] = '\0'; // Null-terminate the string
          
          Serial.println("Full message received:");
          Serial.println(fullMessage);
          
          // Send via Ethernet UDP
          if (eth_connected && (uint32_t)ETH.localIP()) {
            Udp.beginPacket(udpServerIP, replyPort);
            Udp.write((const uint8_t*)fullMessage, fullLength);
            if (Udp.endPacket()) {
              Serial.println("UDP packet sent successfully");
              LED3.toggle();
            } else {
              Serial.println("Failed to send UDP packet");
            }
          }
          
          receivedChunks = 0; // Reset for next message
        }
      }
    }
  }
}

/*
  recepcion de datos no funciona la ip fija del transmisor funciona prende led amarillo pero no lo recibe el receptor
  mirar la funcion de envio de datos del transmisor para dejar funcionando
  el dhcp del transmisor no funciona con el modulo de balanza
  intentar dejar una ip fija a la balanza , no termina de funcionar
  para enviar datos de la balanza
  primero apretar la tecla "1", despues agregar un numero de lote y apretar enter, agregar peso, poner el tara y seleccionar enviar que es 
  un boton que + y una camara.

  otro problema es el dns, agregar uno fijo con valor 8.8.8.8
  pc con ip fija se conecta con el modulo transmisor.
*/

// OTA----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const char *ssid = "Receptor PGR-MODE";
const char *password = "lalaland";

// OTA AP Static IP configuration
IPAddress local_ip_AP(192, 168, 4, 23);
IPAddress gateway_AP(192, 168, 4, 10);
IPAddress subnet_AP(255, 255, 255, 0);

// Setup --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
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
  //ETH.config(local_ip);  // Static IP, leave without this line to get IP via DHCP
  delay(1000);

  //Udp.begin(localUdpPort);  // Enable UDP listening to receive data

  checkSwitch();
  // Initialize based on initial switch state
  if (programmingMode) {
    startProgrammingMode();
  } else {
    initEspNow();
  }
}

// Loop -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop() {
  LED1.loop();
  LED2.loop();
  LED3.loop();
  checkSwitch();
  unsigned long currentMillis = millis();

  if (programmingMode) {
    ArduinoOTA.handle();
  }
  else{
    //Heartbeat Watchdog
    if (currentMillis - lastHeartbeatSent >= HEARTBEAT_INTERVAL) {
      sendHeartbeat();
      lastHeartbeatSent = currentMillis;
    }
    checkConnectionStatus();
  }
  if (eth_connected && !((uint32_t)ETH.localIP())){
    while(!((uint32_t)ETH.localIP())) // Waiting for IP
    {

    }
    Serial.println("Connected");
    Serial.print("IP Address:");
    Serial.println(ETH.localIP());
  }
  // Free any previously allocated memory
  if (largeBuffer) {
    free(largeBuffer);
    largeBuffer = NULL;
    largeBufferSize = 0;
  }

  if (fullMessage) {
    free(fullMessage);
    fullMessage = NULL;
    fullMessageSize = 0;
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
  esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE);
  delay(1000);
  
  Serial.print("Initializing ESP-NOW... ");
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  Serial.println("Done");

  Serial.print("Registering receive callback... ");
  esp_now_register_recv_cb(onDataReceive);
  Serial.println("Done");

  Serial.print("Adding peer... ");
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(esp_now_peer_info_t));
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    Serial.print("Error: ");
    Serial.println(esp_err_to_name(esp_now_add_peer(&peerInfo)));
    LED1.blink(200, 200);
    LED2.turnOFF();
    return;
  }
  else{
    LED2.blink(250, 750);  // Blink green LED to indicate ESP-NOW is paired
    LED1.turnOFF();
  }
  Serial.println("Done");

  Serial.println("ESP-NOW Receiver Initialized");
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