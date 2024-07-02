#include <ETH.h>
#include <WiFi.h>
#include <WiFiClient.h>

const char *ssid = "ESP32-AP";
const char *password = "yourpassword";
WiFiClient client;

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
  
  connectToWiFi();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }

  if (client.connected()) {
    // Recepción de datos por WiFi y envío por Ethernet
    if (client.available()) {
      char c = client.read();
      Serial.write(c);
      // Enviar los datos recibidos por WiFi por Ethernet usando LAN8720
      sendEthernetData(c);
    }
  } else {
    reconnectToServer();
  }
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  reconnectToServer();
}

void reconnectToServer() {
  while (!client.connect("192.168.4.1", 80)) { // IP del primer ESP32 (AP)
    Serial.println("Connection to server failed, retrying...");
    delay(1000);
  }
  Serial.println("Connected to server");
}

void sendEthernetData(char data) {
  // Implementar la función para enviar datos por Ethernet utilizando la LAN8720
  // Este es un lugar holder; necesitarás implementar la lógica específica para enviar datos por Ethernet
}
