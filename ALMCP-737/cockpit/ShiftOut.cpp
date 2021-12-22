#include "ShiftOut.h"

long previousMillis = 0;

ShiftOut::ShiftOut(int clockPin, int latchPin, int dataPin) {
  _dataPin = dataPin;
  _latchPin = latchPin;
  _clockPin = clockPin;

refs[0] = "x737/systems/afds/fdA_status";
refs[1] = "x737/systems/athr/athr_armed";
refs[2] = "x737/systems/MCP/LED_N1_on";
refs[3] = "x737/systems/MCP/LED_MCPSPD_on";
refs[4] = "x737/systems/MCP/LED_LVLCHG_on";
refs[5] = "x737/systems/MCP/LED_VNAV_on";
refs[6] = "x737/systems/MCP/LED_HDG_on";
refs[7] = "x737/systems/MCP/LED_APP_on";

refs[8] = "x737/systems/MCP/LED_VORLOC_on";
refs[9] = "x737/systems/MCP/LED_LNAV_on";
refs[10] = "x737/systems/MCP/LED_ALTHLD_on";
refs[11] = "x737/systems/MCP/LED_VVI_on";
refs[13] = "x737/systems/MCP/LED_CMDA_on";
refs[14] = "x737/systems/brakes/parkingBrakeHandle";
//extra
refs[17] = "x737/systems/MCP/LED_VVIARMED_on";

  vals[0]=0;
  vals[1]=0;

  pinMode(_latchPin, OUTPUT);
  pinMode(_clockPin, OUTPUT); 
  pinMode(_dataPin, OUTPUT);
}

boolean ShiftOut::set(String ref, int val) {
  int k;
  for (k=0; k<18; k++) {
    if (refs[k]==ref) break;
  }
  
  /********************************************/
  if (k==17 && val==1) { //se LED_VVIARMED_on ed e' attivo
    ShiftOut::setBlink(11);
    return true;
  }
  if (k==11) {
    ShiftOut::setUnBlink(11);
  }
  /********************************************/
  
  //non trova niente
  if (k==18) return false;
  
  doit(k,val);

  return true;
}

byte ShiftOut::readPin(int pin) {
  int nbyte;
  
  //in quale byte
  if (pin<8) nbyte=0;
  else {
    nbyte=1;
    pin=pin-8;
  }
  
  return bitRead(vals[nbyte], pin);
}

void ShiftOut::doit(int pin, int val) {
  int nbyte;
  
  //in quale byte
  if (pin<8) nbyte=0;
  else {
    nbyte=1;
    pin=pin-8;
  }
  
  bitWrite(vals[nbyte], pin, val);
/*
  Serial.print("OUT: ");
  Serial.println(vals[0],BIN);
  Serial.println(vals[1],BIN);
*/  
  digitalWrite(_latchPin, LOW);
  shiftOut(_dataPin, _clockPin, MSBFIRST, vals[1]);
  shiftOut(_dataPin, _clockPin, MSBFIRST, vals[0]);
  digitalWrite(_latchPin, HIGH);
}

void ShiftOut::doBlink() {
  unsigned long currentMillis = millis();
  
  if(currentMillis - previousMillis > 100) {
    previousMillis = currentMillis;

    if (blinks[0]>-1) {
      lastblink++;
      if (lastblink==2) lastblink=0;
      
      doit(blinks[0],lastblink);
    }
  }
}

void ShiftOut::setBlink(int pin) {
//  Serial.print("blinks: "); Serial.println(pin);
  blinks[0]=pin;
}
void ShiftOut::setUnBlink(int pin) {
//  Serial.print("blinks: "); Serial.println(pin);
  blinks[0]=-1;
}
