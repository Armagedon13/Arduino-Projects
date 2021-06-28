# DWEasyFlipFlop
Arduino Library for calling 2 different function with desired intervals.
What for? You need call 2 functions periodically with different intervals without delaying your loop flow; 
Let's say function1 (flip) will be triggered at the beginning and 300 millisecondes function2 will fired for 10 millisecondes ... end so on.

How? Library is very simple to use; 
+Define delays for each task/functions 
+Define callback functions (functions will be fired after millisecondes set) 
+Put the flipflop.loop() instance of engin in Arduino loop() function

That's all, continue programming in ease.
