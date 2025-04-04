#include "FastInterruptEncoder.h"

#define ENCODER_READ_DELAY    300

Encoder enc(PA0, PA1, SINGLE /* or HALFQUAD or FULLQUAD */, 250 /* Noise and Debounce Filter (default 0) */); // - Example for STM32, check datasheet for possible Timers for Encoder mode. TIM_CHANNEL_1 and TIM_CHANNEL_2 only
//Encoder enc(25, 26, SINGLE, 250);  - Example for ESP32

unsigned long encodertimer = 0;

void setup() {
  Serial.begin(115200);
  
  if (enc.init()) {
    Serial.println("Encoder Initialization OK");
  } else {
    Serial.println("Encoder Initialization Failed");
    while(1);
  }

}

void loop() {
  enc.loop();                     //better to use with Timer Interrupt
                  
  if ((unsigned long)ENCODER_READ_DELAY < (unsigned long)(millis() - encodertimer)) {
    Serial.println(enc.getTicks());
    encodertimer = millis();
  }
}
