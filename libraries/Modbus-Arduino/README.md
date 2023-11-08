# Modbus Library for Arduino
Allows your Arduino to communicate via Modbus protocol

[![GitHub release (latest by date including pre-releases)](https://img.shields.io/github/v/release/epsilonrt/modbus-arduino?include_prereleases)](https://github.com/epsilonrt/modbus-arduino/releases) 
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/epsilonrt/library/modbus-arduino.svg)](https://registry.platformio.org/libraries/epsilonrt/modbus-arduino) 
[![Arduino Registry](https://www.ardu-badge.com/badge/Modbus-Arduino.svg)](https://www.arduinolibraries.info/libraries/modbus-arduino)

[![Framework](https://img.shields.io/badge/Framework-Arduino-blue)](https://www.arduino.cc/)
[![Platform ATMELAVR](https://img.shields.io/badge/Platform-AtmelAVR-blue)](#)
[![Platform ATMELSAM](https://img.shields.io/badge/Platform-AtmelSAM-blue)](#)
[![Platform Teensy](https://img.shields.io/badge/Platform-Teensy-green)](#)
[![Platform ESP8266](https://img.shields.io/badge/Platform-Espressif8266-orange)](#)
[![Platform ESP32](https://img.shields.io/badge/Platform-Espressif32-orange)](#)

---

<a href="https://modbus.org/">
  <img src="https://github.com/epsilonrt/modbus-arduino/raw/master/extras/modbus.png" alt="Modbus Logo" align="right" valign="top">
</a>

The Modbus is a master-slave protocol used in industrial automation and can be used in other areas, such as home automation.

The Modbus generally uses serial RS-232 or RS-485 as physical layer (then called Modbus Serial) and
TCP/IP via Ethernet or WiFi (Modbus TCP). But it is also possible to associate the Modbus application protocol on any other physical layer, such as the radio for example (with [MobdusRadio](https://github.com/epsilonrt/modbus-radio) for example).

![Modbus Stack](https://github.com/epsilonrt/modbus-arduino/raw/master/extras/modbus-stack.png)

In the current version the library allows the Arduino operate **as a slave**, supporting Modbus Serial and
Modbus over IP. For more information about Modbus see:

* [Wikipedia article](https://en.wikipedia.org/wiki/Modbus)  
* [MODBUS Application Protocol Specification](http://www.modbus.org/docs/Modbus_Application_Protocol_V1_1b.pdf)  
* [MODBUS Messaging on TCP/IP Implementation Guide](http://www.modbus.org/docs/Modbus_Messaging_Implementation_Guide_V1_0b.pdf)  
* [MODBUS over serial line specification and implementation guide ](http://www.modbus.org/docs/Modbus_over_serial_line_V1_02.pdf)



## Features

* Operates as a slave (master mode in development)   
* Supports Modbus over Serial line (RS-232 or RS485) and Modbus TCP
* Reply exception messages for all supported functions  
* Modbus functions supported:  
    * 0x01 - Read Coils  
    * 0x02 - Read Input Status (Read Discrete Inputs)  
    * 0x03 - Read Holding Registers  
    * 0x04 - Read Input Registers  
    * 0x05 - Write Single Coil  
    * 0x06 - Write Single Register  
    * 0x0F - Write Multiple Coils  
    * 0x10 - Write Multiple Registers  
    * 0x11 - Report Server ID

A set of examples is available for each of the Modbus-derived classes, for example:  
* [Lamp](https://github.com/epsilonrt/modbus-serial/blob/master/examples/Lamp/Lamp.ino): Use a coil to turn on and off a LED (0x05 and 0x01 functions)  
* [LampDimmer](https://github.com/epsilonrt/modbus-serial/blob/master/examples/LampDimmer/LampDimmer.ino): Use a holding register to control the brightness of a LED (0x06 and 0x03 functions)  
* [Switch](https://github.com/epsilonrt/modbus-serial/blob/master/examples/Switch/Switch.ino): Use a discrete input to read the state of a switch (0x02 function)  
* [TempSensor](https://github.com/epsilonrt/modbus-serial/blob/master/examples/TempSensor/TempSensor.ino): Use a input register to read the temperature from a sensor (0x04 function)  
* [Servo](https://github.com/epsilonrt/modbus-radio/blob/master/examples/Servo/Servo.ino): Use a holding register to control the position of a servo motor (0x06 an 0x03 function). Show how to define boundaries for the register.
* [LampEncrypted](https://github.com/epsilonrt/modbus-radio/blob/master/examples/LampEncrypted/LampEncrypted.ino): Use a coil to turn on and off a LED (0x05 and 0x01 functions) with encrypted communication.

All examples show the use of 0x11 function to report the slave ID.


**Notes:**

1. The offsets for registers are 0-based. So be careful when setting your supervisory system or your testing software. If offsets are 1-based, so a register configured as 100 in library should be 101 in this software.

2. Early in the library Modbus.h file there is an option to limit the operation
to the functions of Holding Registers, saving space in the program memory.
Just comment out the following line:

		#define USE_HOLDING_REGISTERS_ONLY

Thus, only the following functions are supported:

* 0x03 - Read Holding Registers  
* 0x06 - Write Single Register  
* 0x10 - Write Multiple Registers  

You may test the library using the [MbPoll](https://github.com/epsilonrt/mbpoll) software. For example, to turn on the led in the Lamp example, just do:

    $ mbpoll -m rtu -b38400 -a10 -t0   /dev/tnt1 1
      mbpoll 1.5-2 - ModBus(R) Master Simulator
      Copyright (c) 2015-2023 Pascal JEAN, https://github.com/epsilonrt/mbpoll
      This program comes with ABSOLUTELY NO WARRANTY.
      This is free software, and you are welcome to redistribute it
      under certain conditions; type 'mbpoll -w' for details.

      Protocol configuration: ModBus RTU
      Slave configuration...: address = [10]
                              start reference = 1, count = 1
      Communication.........: /dev/tnt1,      38400-8E1
                              t/o 1.00 s, poll rate 1000 ms
      Data type.............: discrete output (coil)

      Written 1 references.

## How to

There are five classes corresponding to six headers that may be used:

* Modbus-Arduino - Base Library  (this repository)  
* [Modbus-Serial](https://github.com/epsilonrt/modbus-serial) - Modbus Serial RTU Library    
* [Modbus-Ethernet](https://github.com/epsilonrt/modbus-ethernet) - Modbus TCP Library (standard Ethernet Shield)   
* [Modbus-EtherCard](https://github.com/epsilonrt/modbus-ethercard) - Modbus TCP Library (for ENC28J60 chip)  
* [Modbus-Esp8266AT](https://github.com/epsilonrt/modbus-esp8266at) - Modbus IP Library (for ESP8266 chip with AT firmware)   
* [Modbus-Radio](https://github.com/epsilonrt/modbus-radio) - Modbus Radio Library (RadioHead compatible chips)  

By opting for Modbus Serial, TCP or Radio you must include in your sketch the corresponding header :

    #include <ModbusSerial.h>


## Modbus Jargon

In this library was decided to use the terms used in Modbus to the methods names, then is important clarify the names of
register types:

| Register type        | Use as             | Access            | Library methods               |
| -------------------- | ------------------ | ----------------- | ----------------------------- |
| Coil                 | Digital Output     | Read/Write        | addCoil(), coil(), setCoil()  |
| Holding Register     | Analog Output      | Read/Write        | addHreg(), hreg(), setHreg()  |
| Input Status         | Digital Input      | Read Only         | addIsts(), ists(), setIsts()  |
| Input Register       | Analog Input       | Read Only         | addIreg(), ireg(), setIreg()  |

**Notes:**

1. _Input Status_ is sometimes called _Discrete Input_.  
2. _Holding Register_ or just _Register_ is also used to store values in the slave.  
3. Examples of use: A _Coil_ can be used to drive a lamp or LED. A _Holding Register_ to
store a counter or drive a Servo Motor. A _Input Status_ can be used with a reed switch
in a door sensor and a _Input Register_ with a temperature sensor.

License
=======
The code in this repo is licensed under the BSD New License. See LICENSE for more info.

