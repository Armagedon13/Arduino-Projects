# esp8266-hw-spi-max7219-7seg
Arduino library for 7segment max7219 modules conncted to ESP8266 using HW SPI

## IMPORTANT - HEADER FILE NAME CHANGED
Since [commit 3124929f5ad056c64b6a8e815474faaf1db048b1](https://github.com/BugerDread/esp8266-hw-spi-max7219-7seg/commit/3124929f5ad056c64b6a8e815474faaf1db048b1) the header file name was changed from **esp8266-hw-spi-max7219-7seg.h** to **esp8266_hw_spi_max7219_7seg.h** to avoid warnings during compilation. 

You need to change the
```
#include <esp8266-hw-spi-max7219-7seg.h>
```
line to 
```
#include <esp8266_hw_spi_max7219_7seg.h>
```
in your project (.ino file) if you can't compile it after update of this library because of 
```
esp8266-hw-spi-max7219-7seg.h: No such file or directory
```
error.

## Supports 
* chained modules
* displaying of numbers and characters A-Z including decimal dots
* using ESP8266 HW SPI

## Hardware setup:
* GPIO13    ->  DIN
* GPIO14    ->  CLK
* anyother  ->  CS
