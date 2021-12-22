UnoArduSimV2.8 README

HOST SITE: https://www.sites.google.com/site/unoardusim/home

USAGE NOTE: It is recommended that you use a "high-contrast" or "dark" OS theme when running UnoArduSim, as the default Windows theme seems to washes out the selection-color highlihting in the Code Pane and Variables Pane.

I. REQUIRED DLL's

A number of required DLL's are included in the zip of UnoArduSIm. 
In addition to the ones whose names start with "Qt" (the Qt Creator used DLL's), the following three DLL's are used by the mingw32_49 compiler used for UnOArduSImV2.2 (and higher):
libgcc_s_dw2-1.dll
libstdc++-6.dll
libwinpthread-1.dll

In addition, the following two DLL's are required because the Qt5Core.dll itself depends on them (since the QtCreator release was built with Microsoft Visual C++ 10):
MSVCR100.dll
MSVCP100.dll
Although already present on most systems, as of V2.2 these two DLL's are now included in the zip file. 

ALL DLL's included in the UnoArduSIm zip file must be left inside the directory that is home to UnoArduSim.exe to ensure perfect version compatibility. DO NOT copy any of these DLL's to your Windows System directories! (as other programs may depend on differing versions of those DLL's found in your System directories, so those must not be overwritten).

Version 2.5 has been compiled under the new Qt version 5.6.2 which supports display DPI awareness. UnoArduSim.exe should  now autmatically scale itself up so it does not appear too small on high ressolution display screens. I have added the Qt application attribute "High DPI Awareness" but have not been able to directly test as I do not have access to a high DPI display. 

II. NEW include FOLDERS

Version V2.6 introduced the ability to use 'Print' and 'Stream' as base classes for user-defined (or 3rd party) classes.
"Tweaked" source code (i.e. tweaked so it can be Parsed without issues inside UnoArduSim)  for Print.h, Print.cpp, Stream.h, Stream.cpp are povided under IDENTICAL file names in the 'include_Sys' folder inside the UnoArduSim unzip folder. This source code is treated like the source code inside any other normal user '#include' file, and so will be executed line-by-line (and so  its functions can be stepped into) during execution. The equivalent built-in base class functions (such as those called by 'HardwareSerial', for example) are instead executed using (faster) integrated custom functions inside UnoArduSim, and those naturally cannot be stepped into.

A second new folder, 'include_3rdParty', has also been added to the installation (unzip) folder, and contains source code (again tweaked for UnoArduSim so it Parses without error) for 3rd party libraries that support some of the newly added 'I/O' devices (such as 'Liquid_Crystal_I2C.h' and 'Liquid_Crystal_I2C.cpp'). You can '#include' these in your user source program using either 'quoted' or 'bracket' syntax -- in either case, if UnoArduSim  cannot find your  '#include' file inside the directory of the currently loaded program, but its name matches to one of the files inside 'include_3rdPaty', it will instead load that file from the 'include_3rdParty' folder. Note that although these files have some tweaks, and so are NOT identical line-for-line with the original (untweaked) 3rd party library files, all class names, function names, and overall functionality, is IDENTICAL.

To function properly, BOTH OF THESE FOLDERS should be left inside the same folder that contains the UnoArduSim.exe executable, and the requiored DLL's and other sub-folders mentioned above. 

III. INTERNATIONALIZATION:

As of version 2.0, UnoArduSim  has support for multiple languages through its Configure | Preferences menu. The desired language is represented by a two-Latin-character code 'xx' (the ISO-639-1 standard code, where for example 'xx' is 'en' for English). English can always be selected under Preferences, as can the 'xx' language for the user's locale (as long as a corresponding UnoArduSim_xx.qm translation file is present inside the 'translations' folder of the UnoArduSim.exe directory). In addition, regardless of your locale, you can also specify your preferred language using its two-letter code 'xx' as the very first line of the myArduPrefs.txt file. If the needed translation file is absent, UnoArduSim reverts to English. 

Translated Help files are held in the '\translations' sub-foldwer of the directory where you unzipped UnoArduSim. 

Note that UnoArduSim will read and write user program files using a text encdoing suitable to the user's system language locale (or chosen Preferences alternate language),  but willl also aytomatically detect Unicode text encoding in program files (if you choose to instead use files with such encoding), and will maintain that Unicode encoding when writing those program files back. As of V2.4, all I/O Devices files are now always written in language-translated form (with UTF-9 encoding to avoid msimatch errors in reading keywords and device names). I/O device files in ENglish can also always be read, regradless of user locale language.

When you Load and Save your Preferences, they are loaded and saved using translations for the current language. If you have these files in English, you can switch temporarily from your preferred language back to 'en' (under Configure | Preferences), Load them in English, then switch back to preferred language 'xx', and Save to get them in your preferred language. Expected/used file text encoding is UTF-8 at UnoArduSim startt, but you can now defeat this under the File menu in order to try using the default text encding for yur chosen language's geigraohic locale.

I must create all the desired *.qm translation files from the source code using the supplied QtLinguist support tools. I do this with the aid of "Google Translator Toolkit" and a small set of custom utility programs I have created for this purpose. If you would like to see a new translation added for your locale, and you are willing to provide a translated list of keywords and phrases used ion the app and Help files, please send me an email at the address listed on the host web site Translations page, and I will send you the necessary keyword/pgrses file for editing. I will add new translation files to the Translations page as users help me to create them.

IV. INTRODUCTION TO UnoArduSim:

When you first run UnoArduSim.exe, you will see a default set of I/O devices (one of each of eight of nine available "big" I/O devices, and several of each of the "small" I/O devices), and a dummy "simple.ino" default program which you can immediately run.  

You can use the Configure menu to adjust the set of I/O devices, and from the provided dialog "Save" that configuration for next time (as a named text file). That file name will be automatically added to the myArduPrefs.txt file that gets loaded at the next program startup -- to go back to the default startup configuration, delete the “IODevs File Name” line in the myArduPrefs.txt file (or just delete that entire file).

When ready, you can load and try out the provided SamplePrograms\DemoProg1.ino demonstration program -- that program needs to have a particular set of I/O devices attached, so use "Configure->IO Devices" and "Load" the myUnoDevs_DemoProg1.txt file (although if you have not moved the DemoProg1.ino file from the SamplePrograms folder, the myArduOrefs.txt file reswident there will instruct UnoArduSIm to auto-load that particular devices file) -- this loads the devices (with pre-set pin connections and settings and values) that are needed by the DemoProg1.ino program. Run it to see the IO devices and program in action. Try double-clicking (or right-clicking) on the SERIAL I/O device to see an associated larger window pop up for easier monitoring/setting of RX/TX bytes/chars (SOFTSER, SPISLV, I2CSLV, and SD_DRV devices also have that popup window functionality). During execution, drag the new I/O Values Scaler slider found on the main toolbar to control the driven DC motor's speed.

You can then try out a more complex (and fun) program DemoProg2.ino that has musical playback and I/O devices moving to the music (my acknowledgement to the animated TV program, the Simpsons, for that one). This second demo program also shows that you can split your code (and included data) between multiple files for better viewing and navigation.

Finally, DemoProg3.ino demonstrates the newer 1SHOT I/O device, and generates waveforms which can be viewed in the Digital Waveforms window. Load DemoProg3.ino and then the myUnoDevs_DemoProg3.txt file. Now left-click on pins 2,3,4, and 5 in turn on the border of the Uno board -- four waveforms will be showing. Run the program to see the waveforms created by a loop() instruction repeatedly triggering the first 1SHOT device. You can manually trigger the second 1SHOT at any time by pressing and then releasing the PUSH I/O Device attached to pin 3 to cause a rising edge. You can halt the program at any point and use the blue and red cursors inside the Digital Waveforms window to measure the produced pulse widths and delays.

You can "listen" to signals on any Uno pin by connectiong a PIEZO 'I/O' device to that pin. If you run your program but hear the soud breaking up (especially as you move the mouse cursor around), it is only because there is some other process (usually a Windows operating system process) occasionally forcing its way in and stealing too much CPU time (even when overall CPU load is still very small). Try running at another time and you will hear the sound is no longer breaking up.
