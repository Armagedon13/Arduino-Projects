//Control beta lazo de temperatura con rtd pt100
//Autor: Sergio Pablo Peñalve
//catriel - Rio Negro

#include <LiquidCrystal.h>
#include <PID_v1.h>

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
double Setpoint, Input, Output;
PID myPID(&Input, &Output, &Setpoint,5,0.2,0.5, DIRECT);

//metodo de calibracion
// 1° usar hielo sobre la RTD PT100 con un termometro patron,pirometro o medidor con termocupla
//medir la temperatura y obtener el valor digital
//ej 7°C equivalen ---> 389 de la conversion ADC
//2° usar agua hervida y un termometro calentar la RTD PT100 y obtener el otro punto
//por ej 40°C ---> 525 ADC
//La RTD PT100 varia la resistencia de manera lineal a la temperatura por lo que tomando dos puntos en baja temp y alta temp
//podemos generar una funcion lineal con estos puntos y transformar el valor ADC en grados C°
//otra forma es calculando la resistencia el voltaje y por regla de tres con respecto al valor ADC obtener la T°

float y1= 9.35; //9.35°C puntos de calibracion
float x1=469; //adc
float y2=30; //40 °C
float x2=492; //adc
int i;
int t1;
float acum;
float m;
float c;
float temp,te;

void setup()
{
lcd.begin(16, 2);
//analogReference(INTERNAL1V1); //arduino mega 2560
analogReference(INTERNAL); //arduino uno
Serial.begin(115200);
//Input=temp;
myPID.SetMode(AUTOMATIC);
}

void loop()
{
lcd.setCursor(0, 0);
//muestreo de señal analogica
for (i=0;i<10000;i++)
{
t1 = analogRead(1);
acum=acum+(float)t1;
}
acum=acum/10000;

//calculo de temperatura por funcion lineal
// y=m * b + c
// donde m es la pendiente
// y - b son los puntos
//c es la constante

m=pendiente(y1,x1,y2,x2);
c=constante(m,y1,x1);
temp=obt_temp(m,c,acum);
lcd.print("PV:");
lcd.print(temp);
Setpoint=obt_adc(m,c,18);
Input = acum;
myPID.Compute();
lcd.setCursor(0, 1);
lcd.print("SP:");
lcd.print(obt_temp(m,c,Setpoint));

//este es para debug se puede sacar

Serial.print("rtd e :");
Serial.println(t1);
Serial.print("setpoint");
Serial.println(Setpoint);
Serial.print("ADC ");
Serial.println(t1);
if (Output>0)
{
lcd.print(" OUT:ON ");
}
else
{
lcd.print(" OUT:OFF");
}
analogWrite(11,Output);
}

//funcion pendiente con respecto a dos puntos se acuerdan de las matematicas
// y te preguntabas para que fuck servia esto jejeje

float pendiente(float y1,float x1,float y2, float x2)
{
m=0;
float m=(y2-y1)/(x2-x1);
return m;
}
//la constante la obtengo despejando
float constante(float m,float y1,float x1)
{
c=0;
float c=y1-(m*x1);
return c;
}

//esta es la funcion lineal

float obt_temp(float m,float c,float x1)
{
float y1=(m*x1)+c;
return y1;
}

//esta es una funcion inversa le damos la temp y nos devuelve el ADC para el PID

float obt_adc(float m,float c,float y1)
{
float x1=(y1 - c)/m;
return x1;

}
