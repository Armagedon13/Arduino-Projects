#include <Arduino.h>

#include <font4x6.h>
#include <font6x8.h>
#include <font8x8.h>
#include <font8x8ext.h>
#include <fontALL.h>

#include <TVout.h>
#include <video_gen.h>

// this constant won't change:
const int  buttonPin = 2;    // the pin that the pushbutton is attached to
const int ledPin = 13;       // the pin that the LED is attached to

// Variables will change:
int buttonPushCounter = 0;   // counter for the number of button presses
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button
int ledState = 0;

void setup() {
  // initialize the button pin as a input:
  pinMode(buttonPin, INPUT_PULLUP); //Resistencia de pullup interna
  // initialize the LED as an output:
  pinMode(ledPin, OUTPUT);
  // initialize serial communication:
  Serial.begin(9600);
}


void loop() {
  // read the pushbutton input pin:
  buttonState = digitalRead(buttonPin);

  // compare the buttonState to its previous state
  if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {
      // if the current state is HIGH then the button
      // wend from off to on:
      buttonPushCounter++;
      Serial.println("on");
      Serial.print("number of button pushes:  ");
      Serial.println(buttonPushCounter);
      
      digitalWrite(ledPin, !digitalRead(ledPin));

      
    }
    else {
      // if the current state is LOW then the button
      // wend from on to off:
      Serial.println("off");
    }

    // save the current state as the last state,
    //for next time through the loop
    lastButtonState = buttonState;

  }
}
