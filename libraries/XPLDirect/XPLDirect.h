/*
  XPLDirect.h - Library for serial interface to Xplane SDK.
  Created by Michael Gerlicher,  September 2020.

  To report problems, download updates and examples, suggest enhancements or get technical support, please visit my patreon page:
     
     www.patreon.com/curiosityworkshop
 
*/

#ifndef XPLDirect_h
#define XPLDirect_h

#define XPLDIRECT_MAXDATAREFS_ARDUINO 100               // This can be changed to suit your needs and capabilities of your board.  
#define XPLDIRECT_MAXCOMMANDS_ARDUINO 50                // Same here.

#define XPLDIRECT_RX_TIMEOUT          500               // after detecting a frame header, how long will we wait to receive the rest of the frame.  (default 500)

#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined (__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)        // add to this for boards that need it
    #define XPL_USE_PROGMEM                                 // define this for boards with limited memory that can use PROGMEM to store strings.  
                                                            // You will need to wrap your dataref names with F() macro ie:
                                                            // Xinterface.registerDataref(F("laminar/B738/annunciator/drive2"), XPL_READ, 100, 0, &drive2);
                                                            // Disable for boards that have issues compiling: errors with strncmp_PF for instance.

#endif

#define XPL_COMMAND_MAX_UPDATE_RATE_MILLIS  300         // good for debounce, these are usually buttons that are pressed.  (default 300)

#define XPLMAX_PACKETSIZE 150                           // Probably leave this alone.  If you need a few extra bytes of RAM it could be reduced, but it needs to
                                                        // be as long as the longest dataref name + 15   (default 150)


//////////////////////////////////////////////////////////////
// STOP! Dont change any other defines in this header!
//////////////////////////////////////////////////////////////

#define XPLDIRECT_BAUDRATE 115200           // don't mess with this, it needs to match the plugin which won't change
#define XPLDIRECT_PACKETHEADER  '<'         // Same here...
#define XPLDIRECT_PACKETTRAILER '>'         // ...and here



/*
   You are probably looking at all the following command tokens and wondering how the protocol was developed.
   Did the developer agonize over consistency and structure?  Why is there a combination of numbers, upper case
   and lower case letters?  Why does the naming seem a little inconsistent?  Here are some attributes:

        Highly planned in advance (unlikely)
        Arbitrary (likely)
        Was good microbrew involved (likely)

*/

#define XPLERROR                   'E'   // %s         general error
#define XPLRESPONSE_NAME           '0'       
#define XPLRESPONSE_DATAREF        '3'   // %3.3i%s    dataref handle, dataref name 
#define XPLRESPONSE_COMMAND        '4'   // %3.3i%s    command handle, command name
#define XPLCMD_PRINTDEBUG          '1'
#define XPLCMD_RESET               '2'
#define XPLCMD_SENDNAME            'a'
#define XPLREQUEST_REGISTERDATAREF 'b'   //  %1.1i%2.2i%5.5i%s    RWMode, array index (0 for non array datarefs), divider to decrease resolution, dataref name
#define XPLREQUEST_REGISTERCOMMAND 'm'  // just the name of the command to register
#define XPLREQUEST_NOREQUESTS      'c'   // nothing to request
#define XPLREQUEST_REFRESH         'd'	//  the plugin will call this once xplane is loaded in order to get fresh updates from arduino handles that write

#define XPLCMD_DUMPREGISTRATIONS   'Z'  // for debug purposes only
#define XPLCMD_DATAREFUPDATE       'e'
#define XPLCMD_SENDREQUEST         'f'
#define XPLCMD_DEVICEREADY         'g'
#define XPLCMD_DEVICENOTREADY      'h'
#define XPLCMD_COMMANDSTART         'i'
#define XPLCMD_COMMANDEND           'j'
#define XPLCMD_COMMANDTRIGGER       'k'   //  %3.3i%3.3i   command handle, number of triggers

#define XPL_READ		1
#define XPL_WRITE       2
#define XPL_READWRITE	3

#define XPL_DATATYPE_INT    1
#define XPL_DATATYPE_FLOAT  2
#define XPL_DATATYPE_STRING 3



class XPLDirect
{
  public:

    XPLDirect(Stream*); 
    void begin(char *devicename);     // parameter is name of your device for reference 
  
    int connectionStatus(void);
    int commandTrigger(int commandHandle);                                                      // triggers specified command 1 time;
    int commandTrigger(int commandHandle, int triggerCount);                                    // triggers specified command triggerCount times.  
    int registerDataRef(char *, int, unsigned int, float, long int *);                    // single int dataref.  name, Mode, rate, divider, Value
    int registerDataRef(char*, int, unsigned int, float, long int*, int);                 // array int dataref
    int registerDataRef(char*, int, unsigned int, float, float*);                         // single float dataref
    int registerDataRef(char*, int, unsigned int, float, float*, int);                    // array float dataref
    int registerCommand(char* commandName, int* value);                               // use this overload to attach the command to a variable
    int registerCommand(char* commandName, int pin);                                  // use this overload to attach the command to a pin
    int registerCommand(char* commandName);                                           // use this overload to trigger the command manually (commandTrigger);
    
   
    int registerDataRef(const __FlashStringHelper*, int, unsigned int, float, long int*);
    int registerDataRef(const __FlashStringHelper*, int, unsigned int, float, long int*, int);
    int registerDataRef(const __FlashStringHelper*, int, unsigned int, float, float*);
    int registerDataRef(const __FlashStringHelper*, int, unsigned int, float, float*, int);

    int registerCommand(const __FlashStringHelper* commandName, int pin);                       // use this overload to attach the command to a pin
    int registerCommand(const __FlashStringHelper* commandName, int* value);                    // use this overload to attach the command to a variable
    int registerCommand(const __FlashStringHelper* commandName);                                // use this overload to trigger the command manually (commandTrigger)

    //int sendReadyCommand(void);
    //int getCurrentData(int, int*);
    int sendDebugMessage(char*);
    int allDataRefsRegistered(void);
    void dumpRegistrations(void);						                                        // a debug function, usually disabled.
   
    int xloop(void);                                                                            // where the magic happens!
  
  private:
      void _processSerial();
      void _processPacket();

      void _sendPacket(int command, int handle, int value);         // for ints
      void _sendPacket(int command, int handle, float value);       // for floats
      void _sendPacket(int command, int handle);                    // just a command with a handle
      void _sendPacket(int command, char* str);                     // send a string
   
      void _transmitPacket();

      void _sendname();
      int _getResponse(int *);

      int _getHandleFromFrame();
      int _getPayloadFromFrame(long int *);
      int _getPayloadFromFrame(float*);

      Stream* streamPtr;

    
      char _receiveBuffer[XPLMAX_PACKETSIZE]; int _receiveBufferBytesReceived;
      char _sendBuffer[XPLMAX_PACKETSIZE];
  
     

      int _pin;
      int _connectionStatus;

    
      long int _loopTimeBeforeRegistration;
      long int _loopTimeAfterRegistration;

    char * _deviceName;

    struct _dataRefStructure
    {
        int dataRefHandle;
        int dataRefRWType;               //XPL_READ, XPL_WRITE, XPL_READWRITE
        int dataRefVARType;              // XPL_DATATYPE_INT 1, XPL_DATATYPE_FLOAT  2   XPL_DATATYPE_STRING 3
        float divider;                     // tell the host to reduce resolution by dividing then remultiplying by this number to reduce traffic.   (ie 1, 10, 100, 1000)
        int forceUpdate;                    // in case xplane plugin asks for a refresh
        unsigned long updateRate;        // maximum update rate in milliseconds, 0 = every change
        unsigned long lastUpdateTime;
        char* dataRefName;
        const __FlashStringHelper* FdataRefName;
        void* latestValue;
        long int lastSentIntValue;
        float lastSentFloatValue;
        int arrayIndex;                 // for datarefs that speak in arrays

    };

    struct _commandStructure
    {
        int commandHandle;
        const char* commandName;
        const __FlashStringHelper* FcommandName;
        long int lastUpdateTime;
        int *latestValue;
        int lastSentValue;
        int pin;

    };

    _dataRefStructure *_dataRefs[XPLDIRECT_MAXDATAREFS_ARDUINO];
    _commandStructure *_commands[XPLDIRECT_MAXCOMMANDS_ARDUINO];

    int _dataRefsCount;
    int _commandsCount;
    int _allDataRefsRegistered;
    
   
};

#endif
