/*
Receptor AP
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
#include <WiFiUdp.h>

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
NetworkUDP udpETH;                       // create UDP object

// Dirección IP y puerto del servidor UDP
IPAddress udpServerIP(192, 168, 0, 162);   // Cambia esta dirección IP a la del servidor UDP
unsigned int replyPort = 1001;             // port to send response to
unsigned int localUdpPort = 1000;

// WIFI UDP ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const char *ssid_TR = "Receptor-Balanza";
const char *password_TR = "lalaland";

// IP address :
IPAddress local_IP_TR(192, 168, 1, 110);
IPAddress gateway_TR(192, 168, 1, 1);
IPAddress subnet_TR(255, 255, 255, 0); 
IPAddress primaryDNS_TR(8, 8, 8, 8); //optional 
IPAddress secondaryDNS_TR(8, 8, 4, 4); //optional 

IPAddress remoteIp(192, 168, 1, 111); // Dirección IP del Receptor

unsigned int localWIFIPort = 8000;
unsigned int receptorWIFIPort = 8001;
char packetBuffer[5000];

WiFiUDP udpWIFI;

// WIFI KeepAlive ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define RECEIVER

// a station disconnected from ESP32 soft-AP
void WiFiAP_StaDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  LED1.blink(200, 500);
  LED2.turnOFF();
}

// a station connected to ESP32 soft-AP
void WiFiAP_StaConnected(WiFiEvent_t event, WiFiEventInfo_t info){
  LED2.blink(100, 700);
  LED1.turnOFF();
}

unsigned long previousKeepAliveMillis = 0;
const long keepAliveInterval = 5000;  // Send keep-alive every 5 seconds
const long keepAliveTimeout = 15000;  // Consider disconnected after 15 seconds of no keep-alive
unsigned long lastKeepAliveReceived = 0;

void handleKeepAlive() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousKeepAliveMillis >= keepAliveInterval) {
    previousKeepAliveMillis = currentMillis;
    
    // Print connection status
    Serial.println("--- Connection Status ---");
    Serial.print("WiFi: ");
    Serial.println(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
    Serial.print("Ethernet: ");
    Serial.println(eth_connected ? "Connected" : "Disconnected");
    
    #ifdef TRANSMITTER
      // Send keep-alive message to Receiver
      const char* keepAliveMsg = "KEEP_ALIVE";
      udpWIFI.beginPacket(remoteIp, receptorWIFIPort);
      udpWIFI.write((const uint8_t*)keepAliveMsg, strlen(keepAliveMsg));
      udpWIFI.endPacket();
    #endif
    
    #ifdef RECEIVER
      Serial.print("Transmitter: ");
      Serial.println((currentMillis - lastKeepAliveReceived) < keepAliveTimeout ? "Connected" : "Disconnected");
    #endif
  }
}

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
  WiFi.onEvent(WiFiAP_StaConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STACONNECTED);
  WiFi.onEvent(WiFiAP_StaDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STADISCONNECTED);
  delay(2000);

  //Udp.begin(localUdpPort);  // Enable UDP listening to receive data

  checkSwitch();
  // Initialize based on initial switch state
  if (programmingMode) {
    startProgrammingMode();
  } else {
    initAPmode();
  }
}

// Loop -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop() {
  LED1.loop();
  LED2.loop();
  LED3.loop();
  checkSwitch();
  handleKeepAlive();

  if (programmingMode) {
    ArduinoOTA.handle();
  } 
  else {
    // int packetSize = udpWIFI.parsePacket();
    // if (packetSize) {
    //   Serial.printf("Received %d bytes from %s, port %d\n", packetSize, udpWIFI.remoteIP().toString().c_str(), udpWIFI.remotePort());
    //   int len = udpWIFI.read(packetBuffer, sizeof(packetBuffer));
    //   if (len > 0){
    //     packetBuffer[len] = 0;
    //   }
    //   Serial.printf("UDP packet contents: %s\n", packetBuffer);

    //   // Forward the packet over Ethernet
    //   udpETH.beginPacket(udpServerIP, replyPort);
    //   udpETH.write((uint8_t*)packetBuffer, len);
    //   if (udpETH.endPacket()) {
    //     Serial.println("Packet forwarded successfully over Ethernet");
    //   } else {
    //     Serial.println("Failed to forward packet over Ethernet");
    //   }
    // }

  int packetSize = udpWIFI.parsePacket();
  if (packetSize) {
    // Keepalive administration
    if (strcmp(packetBuffer, "KEEP_ALIVE") == 0) {
        lastKeepAliveReceived = millis();
    } 
    else {
      Serial.printf("Received %d bytes from %s, port %d\n", packetSize, udpWIFI.remoteIP().toString().c_str(), udpWIFI.remotePort());
      int len = udpWIFI.read(packetBuffer, sizeof(packetBuffer));
      if (len > 0) {
        packetBuffer[len] = 0;  // Null-terminate the string
      }
      Serial.printf("UDP packet contents: %s\n", packetBuffer);

      // Print Ethernet status and IP
      Serial.printf("Ethernet Status: %s\n", ETH.linkUp() ? "Connected" : "Disconnected");
      Serial.printf("Ethernet IP: %s\n", ETH.localIP().toString().c_str());

      // Forward the packet over Ethernet
      udpETH.beginPacket(udpServerIP, replyPort);
      udpETH.write((uint8_t*)packetBuffer, len);
      if (udpETH.endPacket()) {
        Serial.printf("Packet forwarded to %s:%d\n", udpServerIP.toString().c_str(), replyPort);
      } else {
        Serial.println("Failed to forward packet over Ethernet");
      }
    }
  }



    /*
    int packetSize = udpWIFI.parsePacket();
    if (packetSize) {
      Serial.print(" Received packet from : "); Serial.println(udpWIFI.remoteIP());
      Serial.print(" Size : "); Serial.println(packetSize);
      int len = udpWIFI.read(packetBuffer, packetSize);
      if (len > 0) packetBuffer[len - 1] = 0;
      Serial.printf("Data : %s\n", packetBuffer);
      udpETH.beginPacket(udpServerIP, replyPort);
      udpETH.write((uint8_t*)packetBuffer, packetSize);
      udpETH.endPacket();
      LED3.blinkNumberOfTimes(50, 50, 1);  // Blink activity LED
      udpWIFI.endPacket();

      // Forward data to Ethernet
      // if (eth_connected) {
      //   if (udpETH.endPacket()) {
      //   Serial.println("UDP packet sent successfully");
      //   LED3.blinkNumberOfTimes(50, 50, 1);
        
      // } else {
      //   Serial.println("Failed to send UDP packet");
      // }
      // }
      // if (udpWIFI.endPacket()){
      //   LED3.blinkNumberOfTimes(50, 50, 1);  // Blink activity LED
      // }
    }*/
  }

  if (eth_connected && !((uint32_t)ETH.localIP())){
    while(!((uint32_t)ETH.localIP())) // Waiting for IP
    {

    }
    Serial.println("Connected");
    Serial.print("IP Address:");
    Serial.println(ETH.localIP());
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
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_OFF);
  LED1.turnOFF();
  LED2.turnOFF();
  LED3.turnOFF();
  delay(1000);
  WiFi.mode(WIFI_AP);
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
  
  initAPmode();

  Serial.println("Programming mode stopped. ESP-NOW active.");
}

void initAPmode() {
  // Creación del Punto de Acceso.
  WiFi.softAP(ssid_TR, password_TR, 1);
  WiFi.softAPConfig(local_IP_TR, gateway_TR, subnet_TR);
  udpWIFI.begin(localWIFIPort);
  udpETH.begin(localUdpPort);
  delay(1000);
  Serial.println("start normal mode");
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