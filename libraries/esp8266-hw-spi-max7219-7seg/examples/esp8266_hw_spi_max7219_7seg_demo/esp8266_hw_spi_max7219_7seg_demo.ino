
/* Include Library */
#include <esp8266_hw_spi_max7219_7seg.h>

// configuration
#define SPI_SPEED             8000000 //SPI@8MHZ
#define SPI_CSPIN             5       //SPI CS=GPIO5
#define DISP_BRGTH            8       //brightness of the display (0-15)
#define DISP_AMOUNT           2       //number of max 7seg modules connected

BgrMax7seg ld = BgrMax7seg(SPI_SPEED, SPI_CSPIN, DISP_AMOUNT);

void setup() {
  /* init displays*/
  ld.init();
  ld.setBright(DISP_BRGTH, ALL_MODULES);
  ld.print("test.test", ALL_MODULES);   // prints string "test.test" on all connected modules
}

void loop() {
  delay(5000);
  ld.clear(ALL_MODULES);              //clear all displays
  delay(1000);

  /* Print some numbers */
  for (int i = -10; i <= 100; i++) {
    ld.print(String(i), 1);           //from -10 to 100 on display 1
    ld.print(String(i / 4.0, 3), 2);  //value above divided by 4, show 3 decimal places on display 2
    delay(500);
  }
}
