#include <Joystick.h>

Joystick_ Joystick;

int zAxis_ = 0; 
int RxAxis_ = 0;                    
int RyAxis_ = 0;  
int RzAxis_ = 0;          
int Throttle_ = 0;      

const bool initAutoSendState = true; 

void setup()
{
      Joystick.begin();
  }
   
void loop(){
 
zAxis_ = analogRead(A3);  
zAxis_ = map(zAxis_,0,1023,0,255);
Joystick.setZAxis(zAxis_);  
  
RxAxis_ = analogRead(A4);
 RxAxis_ = map(RxAxis_,0,1023,0,255);
 Joystick.setRxAxis(RxAxis_);
  
 RyAxis_ = analogRead(A5);
 RyAxis_ = map(RyAxis_,0,1023,0,255);
 Joystick.setRyAxis(RyAxis_);

 RzAxis_ = analogRead(A2);
 RzAxis_ = map(RzAxis_,1023,0,255,0);            
Joystick.setRzAxis(RzAxis_);
  
Throttle_ = analogRead(A1);
Throttle_ = map(Throttle_,1023,0,255,0);         
Joystick.setThrottle(Throttle_);   

RyAxis_ = analogRead(A0);
RyAxis_ = map(RyAxis_,0,1023,0,255);
Joystick.setRyAxis(RyAxis_);
 
delay (50);
}
