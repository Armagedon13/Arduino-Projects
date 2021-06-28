/*Thanks. Remember to visit and subscribe to my Youtube channel http://www.youtube.com/c/ELECTRONOOBS
  If you don't whant to Serial print the valeus just delete the serial.print lines
  and leave just the LCD print ones.
  I've used a i2c LCD screen module. 
*/
//LCD config
#include <Wire.h> 
/*#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3f,20,4);  //sometimes the adress is not 0x3f. Change to 0x27 if it dosn't work.
*/
#define analogPin      0          
#define chargePin      13         
#define dischargePin   8
#define gnd   A2        
#define resistorValue  10000.0F  //Remember, we've used a 10K resistor to charge the capacitor

unsigned long startTime;
unsigned long elapsedTime;
float microFarads;                
float nanoFarads;

void setup(){
  Serial.begin(9600);
  pinMode(chargePin, OUTPUT);     
  digitalWrite(chargePin, LOW);
  pinMode(gnd, OUTPUT);     
  digitalWrite(gnd, LOW); 
  /*lcd.init();
  lcd.backlight();*/
}

void loop(){
  digitalWrite(chargePin, HIGH);  //apply 5 Volts
  startTime = micros();           //Start the counter
  while(analogRead(analogPin) < 648){       //While the value is lower than 648, just wait
  }

  elapsedTime= micros() - startTime;
  microFarads = ((float)elapsedTime / resistorValue) ; //calculate the capacitance value

     
  
  if (microFarads > 1){

    /*lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("SCALE:  0.1uF-4F");
    lcd.setCursor(0,1);  
    lcd.print(microFarads);
    lcd.setCursor(14,1);        
    lcd.print("uF");*/
      Serial.println("SCALE:  0.1uF-4F");
      Serial.println(microFarads);
      Serial.println("uF");
    delay(500);    
  }

  else{
    nanoFarads = microFarads * 1000.0; 
    /*lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("SCALE:  0.1uF-4F");
    lcd.setCursor(0,1);       
    lcd.print(nanoFarads);  
    lcd.setCursor(14,1);       
    lcd.print("nF");    */
      Serial.println("SCALE:  0.1uF-4F");
      Serial.println(nanoFarads);
      Serial.println("nF");
    delay(500); 
  }

  
  digitalWrite(chargePin, LOW);            
  pinMode(dischargePin, OUTPUT);            
  digitalWrite(dischargePin, LOW);     //discharging the capacitor     
  while(analogRead(analogPin) > 0){         
  }//This while waits till the capaccitor is discharged

  pinMode(dischargePin, INPUT);      //this sets the pin to high impedance
  
  /*lcd.setCursor(0,0);
  lcd.print("DISCHARGING.....");
  lcd.setCursor(0,1);  */
      Serial.println("DISCHARGING.....");
  
}
