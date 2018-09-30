
#include <unistd.h> 
#include <signal.h> 
#include <iostream>  
#include <cstdlib>
#include "PantallaServidor.h"
#include "TournomentServer.h"

using namespace std;


int main(int argc, char *argv[]) 
{
	SDL_Surface *screen;
 	if( SDL_Init(SDL_INIT_VIDEO) != 0 ) 
 	{
 		cout << SDL_GetError() << endl;
 		exit(1);
 	}

	screen = SDL_SetVideoMode(PantallaServidor::RESOLUCION_X, PantallaServidor::RESOLUCION_Y, 24, SDL_DOUBLEBUF);
	
	if(screen == NULL)
	{
		cout << SDL_GetError() << endl;
		exit(1);
	}		

	PantallaServidor pantalla(screen);
	pantalla.pantallaDePresentacion();
	

    TournomentServer *ts = new TournomentServer(screen);    
    ts->MainFunction();
   
   
   
	//SDL_FreeSurface(screen);
	cout<<"voy a borrar TS"<<endl;
   
	delete ts;
    return 0;
}

