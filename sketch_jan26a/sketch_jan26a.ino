int contador=0;
int boton = 2; //(pin de entrada del botón)

void setup{
pinMode(2,INPUT);

}
void loop{
  
if(digitalRead(boton) == HIGH){
contador++; //Aumenta la variable en uno con cada paso que el botón este en HIGH
}

}
