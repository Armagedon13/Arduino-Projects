/*
 * MCP x737 for XPLANE.
 * By Antonio Lapadula
 *
 * This is not a final release, is just a code that is working.
 * Set the IP and netmask according to your network and set the ip address of your XPLANE
 *
 *    Permission is hereby granted, free of charge, to any person
 *    obtaining a copy of this software and associated documentation
 *    files (the "Software"), to deal in the Software without
 *    restriction, including without limitation the rights to use,
 *    copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following
 *    conditions:
 * 
 *    This permission notice shall be included in all copies or 
 *    substantial portions of the Software.
 * 
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *    OTHER DEALINGS IN THE SOFTWARE.
 */
 
#include <SPI.h>     
#include <Ethernet.h>
#include <RotaryEncoder.h>;
#include "LedControl.h"
#include "ShiftIn.h"
#include "ShiftOut.h"

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xAF, 0xFE, 0xED };

//MY IP
IPAddress ip(192, 168, 1, 10);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

EthernetClient client;

//XPLANE ExtPlane plugin
IPAddress server(192,168,1,5);
int port=51000;

int CRSval;
long CRSlast=0;
RotaryEncoder enccrs(A14,A15,0,0,0);

int IASval;
long IASlast=0;
RotaryEncoder encias(A12,A13,0,0,0);

int HDGval;
long HDGlast=0;
RotaryEncoder enchdg(A10,A11,0,0,0);

long ALTval;
long ALTlast=0;
RotaryEncoder encalt(A8,A9,0,0,0);

int VSPval;
long VSPlast=0;
RotaryEncoder encvspd(A6,A7,0,0,0);

String str,cmd,dataref,val;

LedControl Seg7(22,24,26,5); //1in 12load 13clk 
ShiftIn in(31, 33, 35); //3data 9latch 10clk 
ShiftOut out(37, 39, 41); //11clk 12latch 14data 

void setup() {  

  Seg7.start(5); 
  
   /*Seg7.displayNumber(0,67890,5); 
   Seg7.displayNumber(1,12345,5);
   Seg7.displayNumber(2,890,3);
   Seg7.displayNumber(3,4567,4); 
   Seg7.displayNumber(4,123,3); */
  //Seg7.test(-1, 10);
  Seg7.setChar(4, 0, 'A',false);
  Seg7.setChar(4, 1, 'L',false);
  
  Seg7.setChar(3, 3, 'X',false);
  
  Seg7.setChar(2, 0, '7',false);
  Seg7.setChar(2, 1, '3',false);
  Seg7.setChar(2, 2, '7',false);
  
  Seg7.setChar(1, 4, 'X',false);
  Seg7.setChar(0, 0, 'P',false);
  Seg7.setChar(0, 1, 'l',false);
  Seg7.setChar(0, 2, 'a',false);
  Seg7.setChar(0, 3, 'n',false);
  Seg7.setChar(0, 4, 'e',false);
  
  Ethernet.begin(mac, ip, gateway, gateway, subnet);
delay(5000);
  Serial.begin(115200);
  while (!Serial) { 
  }
  Serial.println("Start...");
}

void loop()
{
  in.check(client,out.readPin(14));

  out.doBlink();
    
  if (client.available()) {
   char c = client.read();
   if (c == '\n' || str.length() > 99) {
   parseStr();
   str="";
   }
   else str += c;
 }
   
   if (!client.connected()) {
     connect();
   } else {
    crs();
    ias();
    hdg(); 
    alt();
    vspd();
  }
}

void connect() {
  delay(1000);
  Serial.println("connecting...");
  client.stop();

  CRSlast=-1;
  IASlast=-1;
  HDGlast=-1;
  ALTlast=-1;
  VSPlast=-1;

  if (client.connect(server, port)) {
    Serial.println("connected");
    //7segments
    client.println("sub sim/cockpit2/radios/actuators/nav1_obs_deg_mag_pilot 5"); //course1
    client.println("sub x737/systems/athr/MCPSPD_spd 10");  //ias
    client.println("sub x737/systems/afds/HDG_magnhdg 5");
    client.println("sub x737/systems/afds/ALTHLD_baroalt 100");
    client.println("sub x737/systems/afds/VS_vvi 1040"); //vspeed
    //x737/systems/athr/MCPSPD_overspeed overspeed 8
    
    //led shiftout
    client.println("sub x737/systems/afds/fdA_status");
    client.println("sub x737/systems/athr/athr_armed");
    client.println("sub x737/systems/MCP/LED_N1_on");
    client.println("sub x737/systems/MCP/LED_MCPSPD_on");
    client.println("sub x737/systems/MCP/LED_LVLCHG_on");
    client.println("sub x737/systems/MCP/LED_VNAV_on");
    client.println("sub x737/systems/MCP/LED_HDG_on");
    client.println("sub x737/systems/MCP/LED_APP_on");
    //
    client.println("sub x737/systems/MCP/LED_VORLOC_on");
    client.println("sub x737/systems/MCP/LED_LNAV_on");
    client.println("sub x737/systems/MCP/LED_ALTHLD_on");
    client.println("sub x737/systems/MCP/LED_VVI_on"); 
    client.println("sub x737/systems/MCP/LED_VVIARMED_on");
    client.println("sub x737/systems/MCP/LED_CMDA_on");

    //x737/systems/MCP/LED_FDA_MA_on
    
    //button shiftn
    //client.println("sub x737/systems/afds/fdA_status"); already
    //client.println("sub x737/systems/athr/athr_armed"); already
    client.println("sub x737/cockpit/MCP/N1_button");
    client.println("sub x737/cockpit/MCP/SPD_button");
    client.println("sub x737/cockpit/MCP/LVLCHG_button");
    client.println("sub x737/cockpit/MCP/VNAV_button");
    client.println("sub x737/cockpit/MCP/HDG_button");
    client.println("sub x737/cockpit/MCP/APP_button");
    client.println("sub x737/cockpit/MCP/VORLOC_button");
    client.println("sub x737/cockpit/MCP/LNAV_button");
    client.println("sub x737/cockpit/MCP/ALTHLD_button");
    client.println("sub x737/cockpit/MCP/VVI_button");
    client.println("sub x737/cockpit/MCP/CMDA_button");
    client.println("sub x737/cockpit/MCP/AP_DISENGAGE_button");
    client.println("sub sim/cockpit/switches/gear_handle_status");
    client.println("sub x737/systems/brakes/parkingBrakeHandle");
  } 
  else {
    // kf you didn't get a connection to the server:
    Serial.println("connection failed");
  } 
}
void crs() {
  int enc = enccrs.readEncoder();

  if(enc != 0) {
    
    CRSval = CRSval + (enc);
    if (CRSval>359) CRSval=0;
    else if (CRSval<0) CRSval=359;
    Serial.println(CRSval);

    if (millis()>3000) { //se arduino avviato da + di 3sec
      Seg7.displayCourse1(CRSval);
      CRSlast = millis();
    
      client.print("set sim/cockpit2/radios/actuators/nav1_obs_deg_mag_pilot ");
      client.println(CRSval);
    }
  }
}
void ias() {
  int enc = encias.readEncoder();

  if(enc != 0) {
    IASlast = millis();
    IASval = IASval + (enc);
    if (IASval>600) IASval=600;
    else if (IASval<0) IASval=0;
    Serial.println(IASval);

    Seg7.displayIas(IASval);
    client.print("set x737/systems/athr/MCPSPD_spd ");
    client.println(IASval);
  }
}
void hdg() {
  int enc = enchdg.readEncoder();
  
  if(enc != 0) {
    HDGlast = millis();
    HDGval = HDGval + (enc);
    if (HDGval>359) HDGval=0;
    else if (HDGval<0) HDGval=359;
    Serial.println(HDGval);

    Seg7.displayHdg(HDGval);
    client.print("set x737/systems/afds/HDG_magnhdg ");
    client.println(HDGval+1);
  }
}
void alt() {
  int enc = encalt.readEncoder();
  int stepv=0;
  
  if(enc != 0) {
    ALTlast = millis();
  
    if (ALTval>9999) stepv=500;
    else stepv=100;
    
    ALTval = ALTval + (enc*stepv);
    if (ALTval%stepv) ALTval=int(ALTval/stepv)*stepv;
    
    if (ALTval>50000) ALTval=50000;
    else if (ALTval<0) ALTval=0;
    Serial.println(ALTval);

    Seg7.displayAlt(ALTval);
    client.print("set x737/systems/afds/ALTHLD_baroalt ");
    client.println(ALTval);
  }
}
void vspd() {
  int enc = encvspd.readEncoder();

  if(enc != 0) {
    VSPlast = millis();
    if (VSPval%100) VSPval=int(VSPval/100)*100;
    VSPval = VSPval + (enc*100);
    if (VSPval>6100) VSPval=6100;
    else if (VSPval<-7900) VSPval=-7900;
    Serial.println(VSPval);

    Seg7.displayVspeed(VSPval);
    client.print("set x737/systems/afds/VS_vvi ");
    client.println(VSPval);
  }
}
void parseStr() {
  int i=0,f=0;

  f = str.indexOf(" ");
  cmd = str.substring(i,f);

  f = str.indexOf(" ",i = f+1);
  dataref = str.substring(i,f);

  f = str.indexOf(" ",i = f+1);
  val = str.substring(i,f);

  Serial.println(cmd);
  Serial.println(dataref);
  Serial.println(val);

  //e' un led ?
  if (out.set(dataref,val.toInt())) return;

  else if (dataref.indexOf("nav1_obs_deg_mag_pilot")>-1) {
    if(millis() - CRSlast > 9991 || CRSlast<0) { //9991 millisecondi
      CRSval = val.toInt();
      Seg7.displayCourse1(CRSval); 
    }
  }
  else if (dataref.indexOf("athr/MCPSPD_spd")>-1) {
    if(millis() - IASlast > 9991 || IASlast<0) {
      IASval = val.toInt();
      Seg7.displayIas(IASval); 
    }
  }  
  else if (dataref.indexOf("afds/HDG_magnhdg")>-1) {
    if(millis() - HDGlast > 9991 || HDGlast<0) {
      HDGval = val.toInt();
      Seg7.displayHdg(HDGval);
    }
  }
  else if (dataref.indexOf("afds/ALTHLD_baroalt")>-1) {
    if(millis() - ALTlast > 9991 || ALTlast<0) {
      ALTval = val.toInt();
      Seg7.displayAlt(ALTval); 
    }
  }
  else if (dataref.indexOf("afds/VS_vvi")>-1) {
    if(millis() - VSPlast > 9991 || VSPlast<0) {
      VSPval = val.toInt();
      Seg7.displayVspeed(VSPval); 
    }
  }
}



