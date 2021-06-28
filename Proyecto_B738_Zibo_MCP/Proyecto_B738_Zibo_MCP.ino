//libreries
#include <CmdMessenger.h>
#include <LedControl.h>
#include <RotaryEncoder.h>



/*
 LedControl.h
 pin 11 is connected to the DataIn 
 pin 13 is connected to the CLK 
 pin 9 is connected to LOAD 
 We have 3 MAX72XX.
*/

LedControl lc=LedControl(11,13,9,3);

// Setup a RoraryEncoder for pins A2 and A3:
RotaryEncoder encoder(A2, A3);

CmdMessenger cmdMessenger = CmdMessenger(Serial);

//pins
const int LedPin = 4;

//values to get in x plane
int Course;                   //Course reader
int CourseEncoder;
long int VertSpeed = 22222;
long int Altitude = 33333;
int Heading = 444;
long int IAS = 5555;

// This is the list of recognized commands. These can be commands that can either be sent or received. 
// In order to receive, attach a callback function to these events
enum
{
  kAcknowledge,
  kError,
  kSentCourse,
  kReciveCourse,
  kSentVertSpeed,
  kSentAltitude,
  kSentHeading,
  kSentIAS,
  kReciveVertSpeed,
  kReciveAltitude,
  kReciveHeading,
  kReciveIAS
};

// Callbacks define on which received commands we take action
void attachCommandCallbacks()
{
  // Attach callback methods
  cmdMessenger.attach(OnUnknownCommand);
  cmdMessenger.attach(kSentCourse, ReciveCourseK);
  //cmdMessenger.attach(kReciveCourse, SentCourseK);
}

// Called when a received command has no attached function
void OnUnknownCommand()
{
  cmdMessenger.sendCmd(kError,"Command without attached callback");
}

void ReciveCourseK()
{
  Course = cmdMessenger.readFloatArg();
  cmdMessenger.sendCmd(kAcknowledge,Course);
}
/*void SentCourseK()
{
  cmdMessenger.sendCmd(kReciveCourse,CourseEncoder);
}*/

void setup() {

  pinMode(LedPin,OUTPUT);  
  
  // Listen on serial connection for messages from the PC
  Serial.begin(115200); 

  // Adds newline to every command
  //cmdMessenger.printLfCr();   

  // Attach my application's user-defined callback methods
  attachCommandCallbacks();

  // Send the status to the PC that says the Arduino has booted
  // Note that this is a good debug function: it will let you also know 
  // if your program had a bug and the arduino restarted  
  cmdMessenger.sendCmd(kAcknowledge,"Arduino has started!");
  
  //Encoder Set up
  // You may have to modify the next 2 lines if using other pins than A2 and A3
  PCICR |= (1 << PCIE1);    // This enables Pin Change Interrupt 1 that covers the Analog input pins or Port C.
  PCMSK1 |= (1 << PCINT10) | (1 << PCINT11);  // This enables the interrupt for pin 2 and 3 of Port C.
  
  LedControl lc=LedControl(11,13,9,3);
  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
  */
  lc.shutdown(0,false);
  lc.shutdown(1,false);
  lc.shutdown(2,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,8);
  lc.setIntensity(1,8);
  lc.setIntensity(2,8);
  /* and clear the display */
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  lc.clearDisplay(2);
}

// The Interrupt Service Routine for Pin Change Interrupt 1
// This routine will only be called on any signal change on A2 and A3: exactly where we need to check.
ISR(PCINT1_vect) {
  encoder.tick(); // just call tick() to check the state.
}

void loop() { 

  // Process incoming serial data, and perform callbacks
  cmdMessenger.feedinSerialData();

  
  //display 1
  //Course control
  lc.setDigit(0, 0, Course % 10, false); // en digito 3 divide por 1000 y aplica modulo 10
  lc.setDigit(0, 1, (Course/10) % 10, false); // en digito 3 divide por 1000 y aplica modulo 10
  lc.setDigit(0, 2, (Course/100) % 10, false); // en digito 3 divide por 1000 y aplica modulo 10

  //VertSpeed
  lc.setDigit(0, 3, VertSpeed % 10, false); // en digito 3 divide por 1000 y aplica modulo 10
  lc.setDigit(0, 4, (VertSpeed/10) % 10, false); // en digito 3 divide por 1000 y aplica modulo 10
  lc.setDigit(0, 5, (VertSpeed/100) % 10, false); // en digito 3 divide por 1000 y aplica modulo 10
  lc.setDigit(0, 6, (VertSpeed/1000)% 10, false); // en digito 3 divide por 1000 y aplica modulo 10
  lc.setDigit(0, 7, (VertSpeed/10000) % 10, false); // en digito 3 divide por 1000 y aplica modulo 10
  //display 2
  //Altitude
  lc.setDigit(1, 0, Altitude % 10, false); // en digito 3 divide por 1000 y aplica modulo 10
  lc.setDigit(1, 1, (Altitude/10) % 10, false); // en digito 3 divide por 1000 y aplica modulo 10
  lc.setDigit(1, 2, (Altitude/100) % 10, false); // en digito 3 divide por 1000 y aplica modulo 10
  lc.setDigit(1, 3, (Altitude/1000)% 10, false); // en digito 3 divide por 1000 y aplica modulo 10
  lc.setDigit(1, 4, (Altitude/10000) % 10, false); // en digito 3 divide por 1000 y aplica modulo 10

  //Heading
  lc.setDigit(1, 5, Heading % 10, false); // en digito 3 divide por 1000 y aplica modulo 10
  lc.setDigit(1, 6, (Heading/10) % 10, false); // en digito 3 divide por 1000 y aplica modulo 10
  lc.setDigit(1, 7, (Heading/100) % 10, false); // en digito 3 divide por 1000 y aplica modulo 10 
  //display 3
  //IAS
  lc.setDigit(2, 0, IAS % 10, false); // en digito 3 divide por 1000 y aplica modulo 10
  lc.setDigit(2, 1, (IAS/10) % 10, false); // en digito 3 divide por 1000 y aplica modulo 10
  lc.setDigit(2, 2, (IAS/100) % 10, false); // en digito 3 divide por 1000 y aplica modulo 10 
  lc.setDigit(2, 3, (IAS/1000)% 10, false); // en digito 3 divide por 1000 y aplica modulo 10
  //Course control
  lc.setDigit(2, 4, Course % 10, false); // en digito 3 divide por 1000 y aplica modulo 10
  lc.setDigit(2, 5, (Course/10) % 10, false); // en digito 3 divide por 1000 y aplica modulo 10
  lc.setDigit(2, 6, (Course/100) % 10, false); // en digito 3 divide por 1000 y aplica modulo 10

  

  static int pos = 0;

  int newPos = encoder.getPosition();
  if (pos != newPos) {
    pos = newPos;
    CourseEncoder = newPos;
  }

  // Send data to PC    
  cmdMessenger.sendCmdStart(kReciveCourse);  
  cmdMessenger.sendCmdArg((float)CourseEncoder);
  cmdMessenger.sendCmdEnd();  
  //delay(1);            // demora de 1 segundo
}
