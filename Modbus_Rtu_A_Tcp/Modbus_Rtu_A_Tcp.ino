/*
ModbusRTU ESP8266/ESP32
True RTU-TCP bridge
convierte rtu a tcp
*/
#ifdef ESP8266
 #include <ESP8266WiFi.h>
#else //ESP32
 #include <WiFi.h>
#endif
#include <ModbusIP_ESP8266.h>
#include <ModbusRTU.h>

#define TO_REG 0
#define SLAVE_ID 1
#define PULL_ID 21
#define FROM_REG 1

ModbusRTU mb1;
ModbusIP mb2;

/*
IPAddress local_IP(192,168,4,22);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);
*/
void setup() {
  Serial.begin(115200);
   //WiFi.softAPConfig(local_IP, gateway, subnet);
   //WiFi.softAP("Modbus-WIFI", "superview");
   //WiFi.softAP("Anastasia", "tele2601701b");
   WiFi.begin("Anastasia", "tele2601701b");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  Serial1.begin(9600, SERIAL_8E1); // Init Serial on default pins
  mb1.begin(&Serial1);
  //mb1.begin(&Serial2, 17);  // Specify RE_DE control pin
  mb1.master();
  mb2.server();
  mb2.addHreg(TO_REG);
}

void loop() {
  if(!mb1.slave())
    mb1.pullHreg(PULL_ID, FROM_REG, TO_REG);
  mb1.task();
  mb2.task();
  delay(50);
}
