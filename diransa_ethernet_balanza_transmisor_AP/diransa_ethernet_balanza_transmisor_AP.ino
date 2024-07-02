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
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>

const char *ssid = "puto-el-que-lee";
const char *password = "lalaland";
WiFiServer server(80);

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
void setup() {
  Serial.begin(115200);
  Network.onEvent(onEvent);
  ETH.begin();

  WiFi.softAP(ssid, password);
  Serial.println("Access Point started");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
  server.begin();
}

void loop() {
  // Recepción de datos por Ethernet y envío al cliente WiFi
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client.");
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // Enviar los datos recibidos por Ethernet al cliente WiFi
        client.write(c);
      }
    }
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

