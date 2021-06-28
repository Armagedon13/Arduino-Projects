# LedController  {#mainpage}

LedController is an [Arduino](http://arduino.cc) library for MAX7219 and MAX7221 Led display drivers.
The code should work with most boards and architectures but I only test it for the ESP32 Dev Module.
Feel free to create an issue if something is missing or not working correctly.
Version 2.0.0 should have increased compatibility since the library is now header only.
The only reason why some boards are not compatible is because they do now provide the necessary dependencies or are not compatible with the Arduino framework.

The CI automatically tests the following boards(PlatformIO boards names):

* esp32dev
* d1_mini
* uno
* ATmega1280
* leonardo
* due (since 2.0.0)
* teensy41 (since 2.0.0)
* bluefruitmicro (since 2.0.0)

The fast mode of the script tests the following boards:

* esp32dev
* ATmega1280

All help regarding compatibility with more boards is welcome.

## Documentation

The following links lead to their respective documentation:

* [LedController class](d9/def/class_led_controller.html)
* [Installation](d6/da3/md_doc_install.html)
* [Usage introduction](dc/dc4/md_doc_usage.html)
* [Migration information](da/dc8/md_doc_migration_notes.html)
* [Example code](examples.html)

## Hardware SPI Pin Assignments

If you want to use hardware SPI you have to use the following port.
Please keep in mind that the MOSI(Master out Slave in) and the SCK(Serial Clock) signal can be shared by multiple SPI devices, while the SS(Slave Select) signal must be unique to every device(You can use every free pin for this).

| Arduino Board | MOSI | MISO | SCK | SS (Slave) | SS (Master) |
|---------------|------|------|-----|------------|-------------|
| Uno or Duemilanove | 11 or ICSP-4 | 12 or ICSP-1 | 13 or ICSP-3 | 10 | - |
| Mega1280 or Mega2560 | 51 or ICSP-4 | 50 or ICSP-1 | 52 or ICSP-3 | 53 | - |
| Leonardo | ICSP-4 | ICSP-1 | ICSP-3 | - | - |
| Due | ICSP-4 | ICSP-1 | ICSP-3 | - | 4, 10, 52 |
| Deek-Robot Arduino | 11 | 12 | 13 | - | 10 |
| ESP32-WROOM | 23 | 19 | 18 | - | 5 |
