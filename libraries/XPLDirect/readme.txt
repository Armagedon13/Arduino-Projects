Curiosity Workshop XPLDirect Arduino Library XPLDirect.h - Library for serial interface to Xplane SDK.
  
  Created by Michael Gerlicher, Curiosity Workshop,  September 2020.

  To report problems, download updates and examples, suggest enhancements or get technical support, please visit out patreon page or our YouTube Page:
     
     www.patreon.com/curiosityworkshop
     https://www.youtube.com/channel/UCISdHdJIundC-OSVAEPzQIQ

  Branded versions of the plugin are available for commercial redistribution, contact me for details.

  This version should be run with plugin build 20201216


** 12/16/2020   - Minor bug fixes

** 11/23/2020   - Added support for manual triggering of registered commands.  

                    an function overload was added for command registration if you only plan to trigger it manually:
                    
                            int registerCommand(char *commandName);     // returns handle to command or negative number on error

                    Two new functions were added:

                            int commandTrigger();                   // triggers the command one time
                            int commandTrigger(int triggerCount);   // triggers the command triggerCount times

                    Example:

                        setup
                            int myCommand = Xinterface.registerCommand("sim/radios/nav2_standy_flip", 6);

                        loop

                            Xinterface.commandTrigger(myCommand);

                This could be useful if you need to trigger commands with a rotary encoder or similar device.

                - small change made to reduce data flow when sending integers to the plugin.

** 11/21/2020   - So I put a serial sniffer on the line to monitor communications between the plugin and the library and found a couple places
                  where packets were being sent twice from the plugin, causing potential buffer overflows and other quirks.  
                  I took the opportunity to revamp the rx engine on the library to increase its robustness.  Please update the library and the 
                  plugin to this version, it has tested quite stable by multiple users.  I have a couple other things on my to-do list but I don't 
                  foresee any short-term major changes at this point.  Thank you for your patience!

** 11/20/2020   - Fixed a minor bug with command registration

                - Added support for PROGMEM.  On some boards, this will prevent the names of the datarefs from being transfered into usable RAM.
                  When *many* datarefs are being registered this makes a substantial difference with memory usage.  Some boards (SAM stuff) don't work
                  this way so the #define XPL_USE_PROGMEM will need to be disabled if you have compiler errors, particularly with things like strlen_PF
                  commands in the library.  Disable the #define and all will be better.  As I prefer more elegant solutions, I will work on making
                  this automatic in the future.
                  
                  To use the change, you will need to wrap your dataref registration command names with an F() macro, for example:

                  Xinterface.registerDataRef(F("sim/cockpit/electrical/beacon_lights_on"), XPL_WRITE, 100, 0, &beacon);

                  For now you can use either, but if you are registering many datarefs you will need to use this on older boards.

                - Minor changes made to the plugin, please update it as well.  


** 11/17/2020    - Added registerCommand overload: registerCommand(char *commandname, int datapin);  
                    This links an arduino datapin to a command, just specify which pin and we do the rest of the work!
                 - fixed a minor bug with command registrations

** 11/16/2020    - changed when dataref and command registration takes place.  Xplane loads plugins, then loads aircraft.  Some 
                    aircraft create new datarefs which would not be found by this plugin.  Registration of datarefs now occurs on the 
                    first flight loop.  If I can find a better way to implement this it will be revised.

                 - variable length loop times implemented for busy loops or slower processors.

                        setMinimumLoopTimes(long int loopTimeBeforeRegistration, long int loopTimeAfterRegistration)
                
                    Add this after the "begin()" command to change the time that xloop watches for serial input before returning.

                    "loopTimeBeforeRegistration" is in milliseconds and xloop will stay looping for this time until all datarefs are
                    registered.  This may need to be higher if the plugin is having difficulty with dataref registrations because of 
                    high packet lengths during the registration time, especially if your code is busier.  The default is 1000ms.

                    "loopTimeAfterRegistration" is also in milliseconds and xloop will stay looping for this time after the dataref 
                    registration process is complete.  This can be a smaller value unless your application is receiving large volumes
                    of data from the plugin.  The default is 20ms.

                - removal of all use of String types
                - streamlining of send and receive buffers
               
** 11/08/2020:  - Memory space allocation for datarefs and xplane commands is now dynamic.  This can be good and bad, as it does not
                  watch for low memory situations.  If things begin to act erratically, memory may be the issue.  
                - Support for Xplane commands has now been implemented.
			            Register a command:  registerCommand(command name, *value);   See example XPLDirectCommandDemo for an example.
                - You will need to update the plugin also with this revision.

** 11/02/2020:  - Changes made to frame formatting to save memory, processing time and data stream traffic.
				- Enabled float type for divider parameter when registering datarefs.  
				  For example, now we can assign a divider of .1 if we only want one decimal precision.
                - Problem fixed with data collisions if arduino board begins sending data before all datarefs are registered
                  with the plugin.
                - Fixed a problem sending negative floats from the arduino board since some don't support floats to string buffers.  
                  What a pain!
				- You will need to update the plugin also with this revision.

** 10/31/2020:  Update to correct an issue with registration of array type datarefs.
** 10/27/2020:  Initial Release 10/27/2020


Installation
-------------
Extract the XPLDirect folder and place it into the libraries folder of your arduino sketches


Usage
------


Include the XPLDirect library:

     #include <XPLDirect.h> 

Create an instance, supply the pointer to your serial port:
     
	 XPLDirect Xinterface(&Serial);

Assign variables for the datarefs you plan to register.  X-plane considers int type to include long int.

     long int beacon;

In your 'begin' code section, initialize your serial port and also the interface:

 Serial.begin(XPLDIRECT_BAUDRATE); 
 Xinterface.begin("Xplane Demo");       
 //Xinterface.setMinimumLoopTimes(beforeRegistration, afterRegistration);  // only needed for specific applications.  Defaults are 1000 and 20.

Also in your 'begin' code, register the datarefs you want to use.
   parameters:  

      char * dataref name, the x-plane dataref you wish to register
      int    write type:				XPL_READ to obtain the value of the dataref from xplane
										XPL_WRITE to send the value to xplane
                                      XPL_READWRITE to do both (not really recommended)
      int    update frequency (ms)	how often we should update this dataref, to reduce traffic
      float  update divider			this reduces the precision of data transferred back and forth.  For example,
                                      if you have a tachometer and only need 10rpm resolution, put 10 here.  This
										will reduce the traffic sent between the arduino and the xplane plugin.  
      long int * variable				the variable containing data that will be written to / read from xplane.


 Xinterface.registerDataRef("sim/cockpit/electrical/beacon_lights_on", XPL_READ, 100, 0, &beacon); 

In your 'loop' code, run the xloop routine:
 
 Xinterface.xloop(); 

That's it!  in this example, the variable 'beacon' will automatically update to reflect the status of the beacon lights in x-plane.

registerDataRef is overloaded also with a float version:

     int registerDataRef(const char*, int, unsigned int, int, float*);     

registerDataRef also includes overloads to handle array type datarefs.  The last parameter is the array element to use:

    int registerDataRef(const char*, int, unsigned int, int, long int*, int);       
    int registerDataRef(const char*, int, unsigned int, int, float*, int);           


-- Avoid using delays in your loop as it can create buffer overflows that will cause frame loss.

--Boards with limited memory such as the original uno and nano fill up quickly and can only handle a few datarefs, depending on 
  the additional code in your sketch.  If you are experiencing erratic operation, try reducing the number of datarefs and commands
  you are using.  If you need more datarefs and your board can handle it, modify the header as needed: 
  
  #define XPLDIRECT_MAXDATAREFS_ARDUINO 10
  #define XPLDIRECT_MAXCOMMANDS_ARDUINO 10

  and specify the number of datarefs/commands you are using.  

