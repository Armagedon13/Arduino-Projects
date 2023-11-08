#include <CmdMessenger.h>  // CmdMessenger

// Blinking led variables 
const int kBlinkLed             = PB13;  // Pin of internal Led

const int analogInputPin1       = PA0;
const int analogInputPin2       = PA1;
const int analogInputPin3       = PA2;
const int analogInputPin4       = PA3;
const int analogInputPin5       = PA4;
const int analogInputPin6       = PA5;

const int switchPin1            = PB12;
const int switchPin2            = PB13;
const int switchPin3            = PB14;
const int switchPin4            = PB15;
const int switchPin5            = PA8;
const int switchPin6            = PA9;
const int switchPin7            = PA10;


bool switch1;
bool switch2;
bool switch3;
bool switch4;
bool switch5;
bool switch6;
bool switch7;

int analog1;
int analog2;
int analog3;
int analog4;
int analog5;
int analog6;

bool acquireData                   = false;
const unsigned long sampleInterval = 100; // 0.1 second interval, 10 Hz frequency
unsigned long previousSampleMillis = 0;
long startAcqMillis                = 0;

// Attach a new CmdMessenger object to the default Serial port
CmdMessenger cmdMessenger = CmdMessenger(Serial);

enum
{
  // Commands
  kAcknowledge         , // Command to acknowledge that cmd was received
  kError               , // Command to report errors
  kStartLogging        , // Command to request logging start      (typically PC -> Arduino)
  kPlotDataPoint       , // Command to request datapoint plotting (typically Arduino -> PC)
};

// Commands we send from the PC and want to receive on the Arduino.
// We must define a callback function in our Arduino program for each entry in the list below.

void attachCommandCallbacks()
{
  // Attach callback methods
  cmdMessenger.attach(OnUnknownCommand);
}

// ------------------  C A L L B A C K S -----------------------

// Called when a received command has no attached function
void OnUnknownCommand()
{
  cmdMessenger.sendCmd(kError,"Command without attached callback");
}


// ------------------ M A I N  ----------------------

// Setup function
void setup() 
{
  //stm32 analog resolution
  analogReadResolution(12);

  // Listen on serial connection for messages from the pc
  Serial.begin(115200); 

  // Adds newline to every command
  cmdMessenger.printLfCr();   

  // Attach my application's user-defined callback methods
  attachCommandCallbacks();

  // set pin for blink LED
  pinMode(kBlinkLed, OUTPUT);

  pinMode(analogInputPin1,INPUT);
  pinMode(analogInputPin2,INPUT);
  pinMode(analogInputPin3,INPUT);
  pinMode(analogInputPin4,INPUT);
  pinMode(analogInputPin5,INPUT);
  pinMode(analogInputPin6,INPUT);

  pinMode(switchPin1,INPUT_PULLDOWN);
  pinMode(switchPin2,INPUT_PULLDOWN);
  pinMode(switchPin3,INPUT_PULLDOWN);
  pinMode(switchPin4,INPUT_PULLDOWN);
  pinMode(switchPin5,INPUT_PULLDOWN);
  pinMode(switchPin6,INPUT_PULLDOWN);
  pinMode(switchPin7,INPUT_PULLDOWN);
}
// Loop function
void loop() 
{
  // Process incoming serial data, and perform callbacks
  cmdMessenger.feedinSerialData();

  measure();

}

// simple readout of two Analog pins. 
void measure() {
   analog1 = analogRead(analogInputPin1);
   analog2 = analogRead(analogInputPin2);
   analog3 = analogRead(analogInputPin3);
   analog4 = analogRead(analogInputPin4);
   analog5 = analogRead(analogInputPin5);
   analog6 = analogRead(analogInputPin6);

   switch1 = digitalRead(switchPin1);
   switch2 = digitalRead(switchPin2);
   switch3 = digitalRead(switchPin3);
   switch4 = digitalRead(switchPin4);
   switch5 = digitalRead(switchPin5);
   switch6 = digitalRead(switchPin6);
   switch7 = digitalRead(switchPin7);
   
   cmdMessenger.sendCmdStart(kPlotDataPoint);  
   cmdMessenger.sendCmdArg(analog1);
   cmdMessenger.sendCmdArg(analog2);
   cmdMessenger.sendCmdArg(analog3);
   cmdMessenger.sendCmdArg(analog4);
   cmdMessenger.sendCmdArg(analog5);
   cmdMessenger.sendCmdArg(analog6);
  
   cmdMessenger.sendCmdArg(switch1);
   cmdMessenger.sendCmdArg(switch2);
   cmdMessenger.sendCmdArg(switch3);    
   cmdMessenger.sendCmdArg(switch4);
   cmdMessenger.sendCmdArg(switch5);
   cmdMessenger.sendCmdArg(switch6);
   cmdMessenger.sendCmdArg(switch7);

   cmdMessenger.sendCmdEnd();
} 
