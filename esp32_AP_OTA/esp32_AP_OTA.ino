#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char *ssid = "AC-ESP32";
const char *passphrase = "123456789";

IPAddress local_IP(192,168,4,22);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);

unsigned long previousMillis;
void setup() {
 Serial.begin(115200);
 Serial.println("Booting");
 //AP config
 Serial.print("Setting soft-AP configuration ... ");
 Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
 Serial.print("Setting soft-AP ... ");
 Serial.println(WiFi.softAP(ssid,passphrase) ? "Ready" : "Failed!");
 		
 initOTA();
 Serial.println("Ready");
 Serial.print("IP address: ");
 Serial.println(WiFi.localIP());
}

void loop() {
 ArduinoOTA.handle();
 if (millis() - previousMillis >= 500) {
 		previousMillis = millis();
 		Serial.println(F("Code has been update via Wifi"));
 }
}

void initOTA() {
 // Port defaults to 3232
 // ArduinoOTA.setPort(3232);
 // Hostname defaults to esp3232-[MAC]
 ArduinoOTA.setHostname("ESP32");
 // No authentication by default
 // ArduinoOTA.setPassword("admin");
 // Password can be set with it's md5 value as well
 // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
 // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
 ArduinoOTA
 .onStart([]() {
 		String type;
 		if (ArduinoOTA.getCommand() == U_FLASH)
 				type = "sketch";
 		else // U_SPIFFS
 				type = "filesystem";
 		// NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
 		Serial.println("Start updating " + type);
 })
 .onEnd([]() {
 		Serial.println("\nEnd");
 })
 .onProgress([](unsigned int progress, unsigned int total) {
 		Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
 })
 .onError([](ota_error_t error) {
 		Serial.printf("Error[%u]: ", error);
 		if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
 		else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
 		else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
 		else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
 		else if (error == OTA_END_ERROR) Serial.println("End Failed");
 });
 ArduinoOTA.begin();
}