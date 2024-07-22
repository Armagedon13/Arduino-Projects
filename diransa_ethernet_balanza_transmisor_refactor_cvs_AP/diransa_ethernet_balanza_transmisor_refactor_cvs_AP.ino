/*
Transmisor AP UDP
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

// WIFI UDP-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const char *ssid_TR = "Receptor-Balanza";
const char *password_TR = "lalaland";
// a network broadcast address
IPAddress local_IP_TR(192, 168, 1, 111);
IPAddress gateway_TR(192, 168, 1, 1);
IPAddress subnet_TR(255, 255, 255, 0); 
IPAddress primaryDNS_TR(8, 8, 8, 8); //optional 
IPAddress secondaryDNS_TR(8, 8, 4, 4); //optional 

IPAddress remoteIp(192, 168, 1, 110); // Dirección IP del Receptor

char packetBuffer[5000];
unsigned int localWIFIPort = 8001;
unsigned int receptorWIFIPort = 8000;

//Are we currently connected?
boolean connected = false;

//The udp library class
WiFiUDP udpWIFI;


// WIFI KeepAlive --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define TRANSMITTER

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  LED1.blink(200, 500);
  LED2.turnOFF();
  Serial.println("Disconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.wifi_sta_disconnected.reason);
  WiFi.disconnect();
  Serial.println("Trying to Reconnect");

  WiFi.begin(ssid_TR, password_TR);
}

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info){
  LED2.blink(100, 700);
  LED1.turnOFF();
}

unsigned long previousKeepAliveMillis = 0;
const long keepAliveInterval = 5000;  // Send keep-alive every 5 seconds
const long keepAliveTimeout = 15000;  // Consider disconnected after 15 seconds of no keep-alive

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

// Ethernet --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
NetworkUDP udpETH;                    // create UDP object
unsigned int localETHPort = 1001;  // Local port number

IPAddress local_ip(192, 168, 0, 140);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

IPAddress dns1(8, 8, 8, 8);             //optional
IPAddress dns2 = (uint32_t)0x00000000;  //optional

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
  ETH.config(local_ip, gateway, subnet,dns1);  // Static IP, leave without this line to get IP via DHCP
  udpETH.begin(local_ip, localETHPort);  // Enable UDP listening to receive data
  WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  delay(1000);

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
    if (eth_connected) {
      int packetSize = udpETH.parsePacket();
      if (packetSize) {
        Serial.print(" Received packet from : "); Serial.println(udpETH.remoteIP());
        Serial.print(" Size : "); Serial.println(packetSize);
        int len = udpETH.read(packetBuffer, packetSize);
        if (len > 0){
          packetBuffer[len] = 0;
        }
        Serial.printf("Data : %s\n", packetBuffer);
        udpETH.printf("UDP packet was received OK\r\n");

        udpWIFI.beginPacket(remoteIp, receptorWIFIPort);
        udpWIFI.write((uint8_t*)packetBuffer, packetSize);
        udpWIFI.endPacket();
        
        if (udpETH.endPacket()){
          LED3.blinkNumberOfTimes(50, 50, 1);  // Blink activity LED
        }
        if (udpWIFI.endPacket()){
          Serial.println("data sended to Reciver");
          udpETH.endPacket();
        }
        
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
  
  initAPmode();

  Serial.println("Programming mode stopped. ESP-NOW active.");
}

void initAPmode() {
  // Connect to Wifi network.
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid_TR, password_TR);
  if (!WiFi.config(local_IP_TR, gateway_TR, subnet_TR)) {
    Serial.println("configuration failed");
  }
  udpWIFI.begin(localWIFIPort);
  delay(1000);
  while (WiFi.status() != WL_CONNECTED)
  {

  }
  Serial.println(" Connected.");
  Serial.println(WiFi.localIP());
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