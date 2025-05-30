//
//    FILE: MCP4725_wave_generator_II.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: demo function generators
//     URL: https://github.com/RobTillaart/MCP4725
//     URL: https://github.com/RobTillaart/FunctionGenerator
//
//  depending on the platform, the range of "smooth" sinus is limited.
//  other signals are less difficult so have a slightly larger range.
//
//  PLATFORM     SINUS    SQUARE  SAWTOOTH  TRIANGLE
//  UNO          -100 Hz
//  ESP32        -200 Hz  -1000   -250      -100
//


#include "MCP4725.h"
#include "Wire.h"

//  amplitude
//  between 0 and 100 percent, use v and V to control it
uint16_t amplitude = 100;

//  frequency
//  use + - * /  to control it
uint16_t freq = 100;

// helpers
uint32_t period = 0;
uint32_t halvePeriod = 0;


//  wave form
//  q = square       z = zero
//  s = sinus        m = mid
//  w = sawtooth     h = high
//  t = stair
//  r = random
char waveForm = 'q';


MCP4725 MCP(0x63);
uint16_t count;
uint32_t lastTime = 0;


//  LOOKUP TABLE SINE
uint16_t sine[361];


void setup()
{
  while(!Serial);
  Serial.begin(115200);
  Serial.println(__FILE__);
  Serial.print("MCP4725_VERSION: ");
  Serial.println(MCP4725_VERSION);
  Serial.println();

  Wire.begin();
  //  Wire.setClock(3400000);

  //  fill table
  for (int i = 0; i < 361; i++)
  {
    sine[i] = 2047 + round(2047 * sin(i * PI / 180));
  }

  MCP.begin();
  Wire.setClock(800000);

  MCP.setValue(0);
  if (!MCP.isConnected())
  {
    Serial.println("err");
    while (1);
  }
  period = 1e6 / freq;
  halvePeriod = period / 2;

  while (1)
  {
    yield();
    uint32_t now = micros();

    count++;

    if (now - lastTime > 100000)
    {
      lastTime = now;
      //  show # updates per 0.1 second
      //  Serial.println(count);
      count = 0;
      if (Serial.available())
      {
        int c = Serial.read();
        switch (c)
        {
          case 'V':
            if (amplitude < 100) amplitude++;
            break;
          case 'v':
            if (amplitude > 0) amplitude--;
            break;
          case '+':
            freq++;
            break;
          case '-':
            freq--;
            break;
          case '*':
            freq *= 10;
            break;
          case '/':
            freq /= 10;
            break;
          case '0' ... '9':
            freq *= 10;
            freq += (c - '0');
            break;
          case 'c':
            freq = 0;
            break;
          case 'A':
            break;
          case 'a':
            break;
          case 'q':
          case 's':
          case 'w':
          case 't':
          case 'r':
          case 'z':
          case 'm':
          case 'h':
            waveForm = c;
            break;
          default:
            break;
        }
        period = 1e6 / freq;
        halvePeriod = period / 2;
        Serial.print(freq);
        //        Serial.print('\t');
        //        Serial.print(period);
        //        Serial.print('\t');
        //        Serial.print(halvePeriod);
        Serial.println();
      }
    }

    uint32_t t = now % period;

    //  a = amplitude as float.
    float a = amplitude * 0.01;

    switch (waveForm)
    {
      case 'q':
        if (t < halvePeriod ) MCP.setValue(a * 4095);
        else MCP.setValue(0);
        break;
      case 'w':
        MCP.setValue(a * t * 4095 / period );
        break;
      case 't':
        if (t < halvePeriod) MCP.setValue(a * t * 4095 / halvePeriod);
        else MCP.setValue( a * (period - t) * 4095 / halvePeriod );
        break;
      case 'r':
        MCP.setValue(random(a * 4096));
        break;
      case 'z':  //  zero
        MCP.setValue(0);
        break;
      case 'h':  //  high
        MCP.setValue(4095);
        break;
      case 'm':  //  mid
        MCP.setValue(2047);
        break;
      default:
      case 's':
        //  reference
        //  float f = ((PI * 2) * t)/period;
        //  MCP.setValue(2047 + 2047 * sin(f));
        //
        int idx = (360 * t) / period;
        MCP.setValue(a * sine[idx]);   //  fetch from lookup table
        break;
    }
  }
}


void loop()
{
}


//  -- END OF FILE --

