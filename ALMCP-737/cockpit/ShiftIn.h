#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
#include <Ethernet.h>

class ShiftIn {
 private :
    byte switchVar[3][2];
  //char* refs[]={"abc","def","ghi","lmn","opq","rst","uwz","xxx"};
  String refs[24];
  
  int _dataPin;//3
  int _latchPin; //9
  int _clockPin; //10

 public:
    ShiftIn(int dataPin, int latchPin, int clockPin);
    void check(EthernetClient client, byte p14);
   // byte shiftIn(int my_dataPin, int my_clockPin);
};

