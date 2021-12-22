#include "ShiftIn.h"
#include "ShiftOut.h"

ShiftIn::ShiftIn(int dataPin, int latchPin, int clockPin) {
  _dataPin = dataPin;
  _latchPin = latchPin;
  _clockPin = clockPin;

refs[0] = "set x737/systems/afds/fdA_status @";
refs[1] = "set x737/systems/athr/athr_armed @";
refs[2] = "set x737/cockpit/MCP/N1_button 1";
refs[3] = "set x737/cockpit/MCP/SPD_button 1";
refs[4] = "bt1";
refs[5] = "bt2";
refs[6] = "set x737/cockpit/MCP/LVLCHG_button 1";
refs[7] = "set x737/cockpit/MCP/VNAV_button 1";
refs[8] = "set x737/cockpit/MCP/HDG_button 1";
refs[9] = "set x737/cockpit/MCP/APP_button 1";
refs[10] = "set x737/cockpit/MCP/VORLOC_button 1";
refs[11] = "set x737/cockpit/MCP/LNAV_button 1";
refs[12] = "set x737/cockpit/MCP/ALTHLD_button 1";
refs[13] = "set x737/cockpit/MCP/VVI_button 1";
refs[14] = "bt3";
refs[18] = "set sim/cockpit/switches/gear_handle_status @";
refs[19] = "set x737/cockpit/MCP/AP_DISENGAGE_button 1";
refs[20] = "cmdb";
refs[21] = "set x737/systems/brakes/parkingBrakeHandle #14";
refs[22] = "set x737/cockpit/MCP/CMDA_button 1";
refs[23] = "cws1";

  pinMode(_latchPin, OUTPUT);
  pinMode(_clockPin, OUTPUT); 
  pinMode(_dataPin, INPUT);
}

void ShiftIn::check(EthernetClient client, byte p14) {
  int pin;
  
  digitalWrite(_latchPin,1);
  delayMicroseconds(20); 
  digitalWrite(_latchPin,0);

  for (int i=0;i<3;i++) {
    
    switchVar[i][0] = shiftIn(_dataPin, _clockPin, LSBFIRST );//ShiftIn::
    //Serial.println( switchVar[i][0], BIN);
    
    //clicla tutti i bit
    for (int k=0; k<8; k++) {
      
      int val = (switchVar[i][0] & (1 << k))?1:0;
      int valOld = (switchVar[i][1] & (1 << k))?1:0;
      
      //controlla che il valore attuale sia diverso dal precedente
      if (val != valOld){
       //setto precedente
        switchVar[i][1] = switchVar[i][0];
    
        pin = (i*8)+k;
        Serial.print("button "); Serial.print(k); Serial.print("/"); Serial.print(i); Serial.print(" - "); Serial.println(pin);    
      
        String tosend = refs[pin];
        
        //invia un valore 0 o 1
        if (tosend.indexOf("@")>0) {
            tosend.replace("@", String(val));
            val = 1;
        } 
        if (tosend.indexOf("#14")>0) {
            tosend.replace("#14", String(p14==1?0:1));
        } 
        
        if (val==1) {
            client.println(tosend);
            Serial.println(tosend);
        }        
      }
    }
  }
  
  //delay(500);
}

