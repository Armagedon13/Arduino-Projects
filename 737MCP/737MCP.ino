
#include "Adafruit_TLC5947.h"       // for the LED multiplexer
#include <CD74HC4067.h>             // Multiplexer library for the switches
#include <LedControl.h>             // for MAX72xx displays
#include <XPLDirect.h>              // My library for attaching to XPLDirect Plugin, available at patreon.com/curiosityworkshop


#include <MCP23017Encoders.h>       // My library for multiplexing rotatary encoders using a MCP23017 chip


  XPLDirect Xinterface(&Serial);                                    // Interface to Xplane


CD74HC4067 my_mux(22, 23, 24, 25);                                // Switches multiplexer
Adafruit_TLC5947 myLEDS= Adafruit_TLC5947(1, 31, 30, 32);         // 1 LED Multiplexer on pins 31 (clock), 30 (data) and 32 (latch)
LedControl myLedDisplays=LedControl(34,36,35,5);                  // data, cs, clock, number of devices
MCP23017Encoders myMCP23017Encoders(3);                           // Parameter is arduino interrupt pin that the MCP23017 is connected to

unsigned int ledBrightness = 2000;

// lights
long int n1_status;
long int speed_status1;
long int lvl_chg_status;
long int vnav_status;
long int lnav_status;
long int vorloc_status;
long int app_status;
long int hdg_sel_status;
long int alt_hld_status;
long int vs_status;
long int cmd_a_status;
long int cmd_b_status;
long int cws_a_status;
long int cws_b_status;
long int autothrottle_status;
long int master_capt_status;
long int master_fo_status;

// buttons.  Those associated with commands should be declared as int type
int button_n1;
int button_speed;
int button_lvl_chg;
int button_vnav;
int button_lnav;
int button_vorloc;
int button_app;
int button_hdg_sel;
int button_alt_hld;
int button_vs;
int button_cmd_a;
int button_cmd_b;
int button_cws_a;
int button_cws_b;
int button_spd_interv;
int button_alt_interv;
int button_co;
int button_change_over_press;

// switches
long int switch_fd_ca;
long int switch_fd_fo;
long int switch_at_arm;

long int disconnect_pos;
int disconnect_toggle;

long int bank_angle_pos;
int bank_angle_up;
int bank_angle_dn;

// displays
long int course_pilot = 888;
long int course_copilot = 888;
float airspeed_dial_kts_mach = 888;
long int mcp_hdg_dial = 888;
unsigned long int mcp_alt_dial = 88888;
long int vvi_dial_fpm = 8888;

// encoders
int cmd_course_pilot_up;
int cmd_course_pilot_dn;
int cmd_course_copilot_up;
int cmd_course_copilot_dn;
int cmd_heading_up;
int cmd_heading_dn;
int cmd_altitude_up;
int cmd_altitude_dn;
int cmd_vertical_speed_down;
int cmd_vertical_speed_up;
int cmd_airspeed_up;
int cmd_airspeed_down;

// for timing of refresh
long int startTime;
int bank_previous;

void setup() 
{

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(26, INPUT_PULLUP);
  pinMode(27, INPUT_PULLUP);


//  Serial.begin(XPLDIRECT_BAUDRATE);
  
  Xinterface.begin("737 MCP");
  
  myMCP23017Encoders.begin();
  
  myLedDisplays.shutdown(0, false);     myLedDisplays.setIntensity(0, 5);  // start the LED displays
  myLedDisplays.shutdown(1, false);     myLedDisplays.setIntensity(1, 5);
  myLedDisplays.shutdown(2, false);     myLedDisplays.setIntensity(2, 5);
  myLedDisplays.shutdown(3, false);     myLedDisplays.setIntensity(3, 5);
  myLedDisplays.shutdown(4, false);     myLedDisplays.setIntensity(4, 5);
  
  myLEDS.begin();                     // start the LED multiplexer
  for (unsigned int ui = 0; ui<24; ui++)  myLEDS.setPWM(ui, 0);    // turn off LEDS for now
  myLEDS.write();
  
 //Initialize serial and wait for port to open:
  while (!Serial)
  {
 
  }

 
  // Initialize Xplane link
  digitalWrite(LED_BUILTIN, LOW);
 
  
/*

AUTOPILOT ROTARY (read/write)
-----------------------------
BANK ANGLE    0-10deg, 1-15deg, 2-20deg, 3-25deg, 4-30deg   laminar/B738/rotary/autopilot/bank_angle
MCP IAS                             sim/cockpit2/autopilot/airspeed_dial_kts_mach
MCP HDG                             laminar/B738/autopilot/mcp_hdg_dial
MCP ALT                             laminar/B738/autopilot/mcp_alt_dial
MCP VVI                             sim/cockpit2/autopilot/vvi_dial_fpm


AUTOPILOT DISPLAY (read)
------------------------

MCP SPD DIGIT 8     laminar/B738/mcp/digit_8
MCP SPD DIGIT A     laminar/B738/mcp/digit_A


*/

// Displays

  Xinterface.registerDataRef(F("sim/cockpit2/autopilot/vvi_dial_fpm"),            XPL_READ, 10, 0, &vvi_dial_fpm);
  Xinterface.registerDataRef(F("sim/cockpit2/autopilot/airspeed_dial_kts_mach"),  XPL_READ, 10, 0, &airspeed_dial_kts_mach);
  Xinterface.registerDataRef(F("laminar/B738/autopilot/course_pilot"),            XPL_READ, 10, 0, &course_pilot);
  Xinterface.registerDataRef(F("laminar/B738/autopilot/course_copilot"),          XPL_READ, 10, 0, &course_copilot);
  Xinterface.registerDataRef(F("laminar/B738/autopilot/mcp_hdg_dial"),            XPL_READ, 10, 0, &mcp_hdg_dial);
  Xinterface.registerDataRef(F("laminar/B738/autopilot/mcp_alt_dial"),            XPL_READ, 10, 0, &mcp_alt_dial);
  
// LEDs
  Xinterface.registerDataRef(F("laminar/B738/autopilot/n1_status"),           XPL_READ, 10, 0, &n1_status);            // N1 status LED
  Xinterface.registerDataRef(F("laminar/B738/autopilot/speed_status1"),       XPL_READ, 10, 0, &speed_status1);        // Speed status LED
  Xinterface.registerDataRef(F("laminar/B738/autopilot/lvl_chg_status"),      XPL_READ, 10, 0, &lvl_chg_status);       // FLC status LED
  Xinterface.registerDataRef(F("laminar/B738/autopilot/vnav_status1"),        XPL_READ, 10, 0, &vnav_status);         // VNAV status LED
  Xinterface.registerDataRef(F("laminar/B738/autopilot/lnav_status"),         XPL_READ, 10, 0, &lnav_status);          // LNAV status LED
  Xinterface.registerDataRef(F("laminar/B738/autopilot/vorloc_status"),       XPL_READ, 10, 0, &vorloc_status);        // VOR/LOC status LED
  Xinterface.registerDataRef(F("laminar/B738/autopilot/app_status"),          XPL_READ, 10, 0, &app_status);           // Approach status LED
  Xinterface.registerDataRef(F("laminar/B738/autopilot/hdg_sel_status"),      XPL_READ, 10, 0, &hdg_sel_status);       // Heading select status LED
  Xinterface.registerDataRef(F("laminar/B738/autopilot/alt_hld_status"),      XPL_READ, 10, 0, &alt_hld_status);       // Altitude Hold select status LED
  Xinterface.registerDataRef(F("laminar/B738/autopilot/vs_status"),           XPL_READ, 10, 0, &vs_status);            // Vertical Speed select status LED
  Xinterface.registerDataRef(F("laminar/B738/autopilot/cmd_a_status"),        XPL_READ, 10, 0, &cmd_a_status);         // AP A select status LED
  Xinterface.registerDataRef(F("laminar/B738/autopilot/cmd_b_status"),        XPL_READ, 10, 0, &cmd_b_status);         // AP A select status LED
  Xinterface.registerDataRef(F("laminar/B738/autopilot/cws_a_status"),        XPL_READ, 10, 0, &cws_a_status);         // CWS A select status LED
  Xinterface.registerDataRef(F("laminar/B738/autopilot/cws_b_status"),        XPL_READ, 10, 0, &cws_b_status);         // CWS B select status LED
  Xinterface.registerDataRef(F("laminar/B738/autopilot/autothrottle_status"), XPL_READ, 10, 0, &autothrottle_status);  // autothrottle status LED
  Xinterface.registerDataRef(F("laminar/B738/autopilot/master_capt_status"),  XPL_READ, 10, 0, &master_capt_status);   // MA captain status LED
  Xinterface.registerDataRef(F("laminar/B738/autopilot/master_fo_status"),    XPL_READ, 10, 0, &master_fo_status);     // MA fo status LED

// Buttons
  Xinterface.registerCommand(F("laminar/B738/autopilot/n1_press"),      &button_n1);           // Autopilot N1 press
  Xinterface.registerCommand(F("laminar/B738/autopilot/speed_press"),   &button_speed);        // Speed autopilot button 
  Xinterface.registerCommand(F("laminar/B738/autopilot/vnav_press"),    &button_vnav);         // LNAV autopilot button
  Xinterface.registerCommand(F("laminar/B738/autopilot/lvl_chg_press"), &button_lvl_chg);      // FLC autopilot button
  Xinterface.registerCommand(F("laminar/B738/autopilot/hdg_sel_press"), &button_hdg_sel);      // Heading Select autopilot button
  Xinterface.registerCommand(F("laminar/B738/autopilot/lnav_press"),    &button_lnav);         // VNAV autopilot button
  Xinterface.registerCommand(F("laminar/B738/autopilot/vorloc_press"),  &button_vorloc);       // VOR/LOC autopilot button 
  Xinterface.registerCommand(F("laminar/B738/autopilot/app_press"),     &button_app);          // APPR autopilot button
  Xinterface.registerCommand(F("laminar/B738/autopilot/alt_hld_press"), &button_alt_hld);      // Altitude Hold autopilot button
  Xinterface.registerCommand(F("laminar/B738/autopilot/vs_press"),      &button_vs);           // Altitude Hold autopilot button
  Xinterface.registerCommand(F("laminar/B738/autopilot/cmd_a_press"),   &button_cmd_a);        // command A engage autopilot button
  Xinterface.registerCommand(F("laminar/B738/autopilot/cmd_b_press"),   &button_cmd_b);        // command B engage autopilot button
  Xinterface.registerCommand(F("laminar/B738/autopilot/cws_a_press"),   &button_cws_a);        // CWS A autopilot button
  Xinterface.registerCommand(F("laminar/B738/autopilot/cws_b_press"),   &button_cws_b);        // CWS B autopilot button
  Xinterface.registerCommand(F("laminar/B738/autopilot/spd_interv"),    &button_spd_interv);   // Speed Interv autopilot button
  Xinterface.registerCommand(F("laminar/B738/autopilot/alt_interv"),    &button_alt_interv);   // Alt Interv autopilot button
  Xinterface.registerCommand(F("laminar/B738/autopilot/change_over_press"), &button_change_over_press);  //        Autopilot IAS MACH change over press

// switches
  Xinterface.registerDataRef(F("laminar/B738/switches/autopilot/fd_ca"),      XPL_WRITE, 0, 100, &switch_fd_ca);         // Switch F/D Captain
  Xinterface.registerDataRef(F("laminar/B738/switches/autopilot/fd_fo"),      XPL_WRITE, 0, 100, &switch_fd_fo);         // Switch F/D FO
  Xinterface.registerDataRef(F("laminar/B738/switches/autopilot/at_arm"),     XPL_WRITE, 0, 100, &switch_at_arm);        // Switch AutoThrottle Arm
  
  Xinterface.registerDataRef(F("laminar/B738/autopilot/disconnect_pos"),      XPL_READ, 0, 100, &disconnect_pos);       // autopilot disconnect
  disconnect_toggle = Xinterface.registerCommand(F("laminar/B738/autopilot/disconnect_toggle") );

  Xinterface.registerDataRef(F("laminar/B738/autopilot/bank_angle_pos"),      XPL_READ, 0, 100, &bank_angle_pos);       // bank angle position
  bank_angle_up = Xinterface.registerCommand(F("laminar/B738/autopilot/bank_angle_up"));
  bank_angle_dn = Xinterface.registerCommand(F("laminar/B738/autopilot/bank_angle_dn"));
  
// Encoders
  cmd_course_pilot_up     = Xinterface.registerCommand(F("laminar/B738/autopilot/course_pilot_up"));          // Autopilot course pilot up
  cmd_course_pilot_dn     = Xinterface.registerCommand(F("laminar/B738/autopilot/course_pilot_dn"));          // Autopilot course pilot down
  cmd_course_copilot_up   = Xinterface.registerCommand(F("laminar/B738/autopilot/course_copilot_up"));        // Autopilot course pilot up
  cmd_course_copilot_dn   = Xinterface.registerCommand(F("laminar/B738/autopilot/course_copilot_dn"));        // Autopilot course pilot down 
  cmd_heading_up          = Xinterface.registerCommand(F("laminar/B738/autopilot/heading_up"));               // Autopilot heading Up
  cmd_heading_dn          = Xinterface.registerCommand(F("laminar/B738/autopilot/heading_dn"));               // Autopilot heading Down
  cmd_altitude_up         = Xinterface.registerCommand(F("laminar/B738/autopilot/altitude_up"));              // Autopilot altitude Up
  cmd_altitude_dn         = Xinterface.registerCommand(F("laminar/B738/autopilot/altitude_dn"));              // Autopilot altitude Down  
  cmd_vertical_speed_down = Xinterface.registerCommand(F("sim/autopilot/vertical_speed_down"));               // Autopilot vs down
  cmd_vertical_speed_up   = Xinterface.registerCommand(F("sim/autopilot/vertical_speed_up"));                 // Autopilot vs up
  cmd_airspeed_up         = Xinterface.registerCommand(F("sim/autopilot/airspeed_up"));                        // Autopilot airspeed up
  cmd_airspeed_down       = Xinterface.registerCommand(F("sim/autopilot/airspeed_down"));                      // Autopilot airspeed dn
  
  digitalWrite(LED_BUILTIN, LOW);

}


void loop() 
{
int newValue;
  
  Xinterface.xloop();
  
// *****************************************************************************************
// everything after the next line will only occur every 100ms
// *****************************************************************************************
  if (millis() - startTime > 100) startTime = millis();   else return;                                          

  // Buttons
  
  button_spd_interv         = readMuxPin(0);
  switch_at_arm             = !readMuxPin(1);
  button_change_over_press  = readMuxPin(2);
  button_vnav               = readMuxPin(3);
  button_lnav               = readMuxPin(4);
  button_alt_interv         = readMuxPin(5);

  button_cmd_a              = readMuxPin(22);        // remapped for a bad multiplexer pin?
  button_cmd_b              = readMuxPin(7);

  switch_fd_ca              = !readMuxPin(8);
  button_n1                 = readMuxPin(9);
  button_speed              = readMuxPin(10);
  button_lvl_chg            = readMuxPin(11);
  button_hdg_sel            = readMuxPin(12);
  button_vorloc             = readMuxPin(13);
  button_app                = readMuxPin(14);
  button_alt_hld            = readMuxPin(15);
  button_vs                 = readMuxPin(16);

  if ( (!readMuxPin(17) && !disconnect_pos) || ( readMuxPin(17) && disconnect_pos ) ) Xinterface.commandTrigger(disconnect_toggle);

  button_cws_a              = readMuxPin(19);
  button_cws_b              = readMuxPin(20);
  switch_fd_fo              = !readMuxPin(21);

// deal with bank angle rotary switch
  if (!readMuxPin(26)) newValue = 0;
  if (!readMuxPin(27)) newValue = 1;
  if (!readMuxPin(28)) newValue = 2;
  if (!readMuxPin(29)) newValue = 3;
  if (!readMuxPin(30)) newValue = 4;
  if (newValue != bank_previous)
  {
    if (newValue < bank_angle_pos) Xinterface.commandTrigger(bank_angle_dn);
    if (newValue > bank_angle_pos) Xinterface.commandTrigger(bank_angle_up);
    bank_previous = newValue;
  }

  // LEDs
  myLEDS.setPWM( 7, n1_status           * ledBrightness);
  myLEDS.setPWM( 8, speed_status1       * ledBrightness);
  myLEDS.setPWM( 9, lvl_chg_status      * ledBrightness);
  myLEDS.setPWM(10, hdg_sel_status      * ledBrightness);
  myLEDS.setPWM(11, vorloc_status       * ledBrightness);
  myLEDS.setPWM(12, app_status          * ledBrightness);
  myLEDS.setPWM(13, alt_hld_status      * ledBrightness);
  myLEDS.setPWM( 2, vnav_status         * ledBrightness);
  myLEDS.setPWM( 3, lnav_status         * ledBrightness);
  myLEDS.setPWM( 4, cmd_a_status        * ledBrightness);
  myLEDS.setPWM( 5, cmd_b_status        * ledBrightness);
  myLEDS.setPWM(14, vs_status           * ledBrightness);
  myLEDS.setPWM(15, cws_a_status        * ledBrightness);
  myLEDS.setPWM(16, cws_b_status        * ledBrightness);
  myLEDS.setPWM(1, autothrottle_status  * ledBrightness);
  myLEDS.setPWM(0, master_capt_status   * ledBrightness);
  myLEDS.setPWM(6, master_fo_status     * ledBrightness);
  
  myLEDS.write();
  
//LED Displays
  printNumber(0, "%3.3i", course_pilot,           5, &myLedDisplays);

  if (airspeed_dial_kts_mach > 0  && airspeed_dial_kts_mach < 1)     // for mach indications
      printNumber(0, "%3.3i", airspeed_dial_kts_mach * 100, 0, &myLedDisplays);
  else 
      printNumber(0, "%3.3i", airspeed_dial_kts_mach, 0, &myLedDisplays);

  printNumber(1, "%3.3i", mcp_hdg_dial,           1, &myLedDisplays);
  printNumber(2, "% 5.3u", mcp_alt_dial,           1, &myLedDisplays);

  if (vs_status) printNumber(3, "% 2.2i", vvi_dial_fpm,           1, &myLedDisplays);
  else           printNumber(3, "      ", vvi_dial_fpm,           1, &myLedDisplays);

  printNumber(4, "%3.3i", course_copilot,         0, &myLedDisplays);

  // Encoders

  newValue = myMCP23017Encoders.read(0) /3;
  if (newValue > 0) { Xinterface.commandTrigger(cmd_course_copilot_dn, newValue);  myMCP23017Encoders.write(0, 0); }
  if (newValue < 0) { Xinterface.commandTrigger(cmd_course_copilot_up, 0-newValue);  myMCP23017Encoders.write(0, 0); }
 
  newValue = myMCP23017Encoders.read(1) /3;
  if (newValue > 0) { Xinterface.commandTrigger(cmd_vertical_speed_down, newValue);  myMCP23017Encoders.write(1, 0); }
  if (newValue < 0) { Xinterface.commandTrigger(cmd_vertical_speed_up, 0-newValue);  myMCP23017Encoders.write(1, 0); }

  newValue = myMCP23017Encoders.read(2) /3;
  if (newValue > 0) { Xinterface.commandTrigger(cmd_altitude_up, newValue);  myMCP23017Encoders.write(2, 0); }
  if (newValue < 0) { Xinterface.commandTrigger(cmd_altitude_dn, 0-newValue);  myMCP23017Encoders.write(2, 0); }

  newValue = myMCP23017Encoders.read(3) /3;
  if (newValue > 0) { Xinterface.commandTrigger(cmd_heading_dn, newValue);  myMCP23017Encoders.write(3, 0); }
  if (newValue < 0) { Xinterface.commandTrigger(cmd_heading_up, 0-newValue);  myMCP23017Encoders.write(3, 0); }

  newValue = myMCP23017Encoders.read(4) /3;
  if (newValue > 0) { Xinterface.commandTrigger(cmd_airspeed_down, newValue);  myMCP23017Encoders.write(4, 0); }
  if (newValue < 0) { Xinterface.commandTrigger(cmd_airspeed_up, 0-newValue);  myMCP23017Encoders.write(4, 0); }

  newValue = myMCP23017Encoders.read(5) /3;
  if (newValue > 0) { Xinterface.commandTrigger(cmd_course_pilot_dn, newValue);  myMCP23017Encoders.write(5, 0); }
  if (newValue < 0) { Xinterface.commandTrigger(cmd_course_pilot_up, 0-newValue);  myMCP23017Encoders.write(5, 0); }

  
}

/*
 * read multiplexer pin
 */
int readMuxPin( int pin)
{

  my_mux.channel(pin % 16);                   // set the channel to the appropriate pin
  return digitalRead( (int)((pin / 16) + 26) );   // read the pin associated with the board
   
}



/* 
 *  prints a number on the MAX 72xx display
 */
void printNumber(int ledDisplay, char *printMask, long int v, int pos, LedControl *lc) 
{  
char tString[10];
int tpos = strlen(tString) + pos;

  sprintf(tString, printMask, v);
  for (int i = strlen(tString)-1; i>=0; i--)
  {
    lc->setChar(ledDisplay, tpos++, tString[i], false);
  }
  
    
} 
