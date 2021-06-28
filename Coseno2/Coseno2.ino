#include <LiquidCrystal.h>
#include <math.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 6);
int pin = 2;
//const int capacitor = 6;
float rads      = 57.29577951; // 1 radian = approx 57 deg.
float degree    = 360;
float frequency = 50;
float nano = 1 * pow (10,-6); // Multiplication factor to convert nano seconds into seconds

// Define floats to contain calculations

float pf;
float angle, suma_angle = 0.0;
float pf_max = 0;
float angle_max = 0;
int ctr;

bool status, statusAnt = false;
unsigned long start;
volatile unsigned long result;
bool flag = false;


void setup()
{  
   attachInterrupt(INT0, isr, CHANGE );  // INT0 es pin2 INT1 es pin 3
   lcd.begin(16, 2);
   Serial.begin(9600);
}

void isr()
{
  status = digitalRead(pin);
  if (status && !statusAnt) { // flanco 0 a 1 // flanco de subida  RISING
     start = micros();        // inicio la cuenta en microsegundos
  }
  if (!status && statusAnt) { // flanco 1 a 0
                              // flanco de bajada FALLLING
      result = micros()-start;
      flag = true;
  }
  statusAnt = status;
}

void loop()
{
  if (flag) {
    suma_angle += result * nano* degree* frequency;
      detachInterrupt(INT0);
      ctr++;
      if (ctr>=9) {
        angle  =  suma_angle;
          ctr = 0;
          suma_angle = 0.0;
          attachInterrupt(INT0, isr, CHANGE );
      }
      flag = false;
    
  }
  // 1st line calculates the phase angle in degrees from differentiated time pulse
  // Function COS uses radians not Degree's hence conversion made by dividing angle / 57.2958
  
     
  if (angle > angle_max) // Test if the angle is maximum angle
  {
      angle_max = angle; // If maximum record in variable "angle_max"
      pf_max = cos(angle_max / rads); // Calc PF from "angle_max"
  }
   
   //if (angle_max > 360) // If the calculation is higher than 360 do following...
   if (angle_max > 360)
   {
    angle_max = 0; // assign the 0 to "angle_max"
    pf_max = 1; // Assign the Unity PF to "pf_max"
   }
   if (angle_max == 0) // If the calculation is higher than 360 do following...
   {
    angle_max = 0; // assign the 0 to "angle_max"
    pf_max = 1; // Assign the Unity PF to "pf_max"
   }
   
   Serial.print(angle_max, 2); // Print the result
   Serial.print(",");
   Serial.println(pf_max, 2);
   
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("PF=");
   lcd.setCursor(4,0);
   lcd.print(pf_max);
   lcd.print(" ");
   lcd.setCursor(0,1);
   lcd.print("Ph-Shift=");
   lcd.setCursor(10,1);
   lcd.print(angle_max);
   lcd.print(" ");
   
   delay(1000);
   angle = 0; // Reset variables for next test
   angle_max = 0;
}
