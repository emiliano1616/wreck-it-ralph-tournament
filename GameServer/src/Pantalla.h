#ifndef PANTALLA_H
#define PANTALLA_H


#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <unistd.h>


using namespace std;

class Pantalla 
{
private:
	SDL_Surface *pantallaActual, *fondo, *fix_it_felix;
	static int const FIX_IT_FELIX_DELAY = 1500;
	static int const FIX_IT_FELIX_FRECUENCIA = 3;
public:
	Pantalla();
	void SplashScreen();
	~Pantalla();
};

#endif