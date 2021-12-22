#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

class ShiftOut {
 private :
  int _dataPin;//3
  int _latchPin; //9
  int _clockPin; //10

  String refs[16];
  byte vals[2];

  int lastblink;
  int blinks[8];
  
  void setBlink(int pin);
  void setUnBlink(int pin);
  void doit(int pin, int val);
 public:
    ShiftOut(int clockPin, int latchPin, int dataPin);
    boolean set(String ref, int val);
    
    void doBlink();
    byte readPin(int pin);
};

