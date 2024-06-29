/*
reciver
*/
#include <esp_now.h>
#include <WiFi.h>
#include <UIPEthernet.h>

// Datos recibidos
typedef struct struct_message {
    char data[250];
} struct_message;

struct_message myData;

// Configuración de Ethernet
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 51); // IP del ESP32 receptor
IPAddress server(192, 168, 1, 100); // Dirección IP de la PC
EthernetClient client;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    memcpy(&myData, incomingData, sizeof(myData));
    Serial.print("Bytes received: ");
    Serial.println(len);
    Serial.print("Data: ");
    Serial.println(myData.data);

    // Enviar datos a la PC
    if (client.connect(server, 80)) {
        client.write((const uint8_t*)&myData, sizeof(myData));
        client.stop();
    }
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);

    // Inicializar ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    esp_now_register_recv_cb(OnDataRecv);

    // Inicializar Ethernet
    Ethernet.begin(mac, ip);
    delay(1000);  // Esperar un poco para asegurarse de que la conexión se establezca
}

void loop() {
    // El código principal no necesita hacer nada, ya que todo se maneja en la devolución de llamada
}

