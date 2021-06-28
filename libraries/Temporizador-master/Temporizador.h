
#ifndef Temporizador_h
#define Temporizador_h
#include "Arduino.h"


class Temporizador
{
	public:
		//Inicializamos el contador del tiempo
		void temporizar(int dia, int hora, int minuto, int segundo, int milisegundos)
        {	
			_tiempo=((((dia*24+hora)*60+minuto)*60+segundo)*1000)+milisegundos;
            if(_tiempo==0) _tiempo=0;   
			_tiempo=_tiempo+millis();
        }

		
		//Comprobamos si ha transcurrido el tiempo indicado
        boolean completado()
        {		
			if(millis()>=_tiempo) return(true);
			else return(false);
        }
	
	private:
		volatile unsigned long _tiempo;
};

#endif