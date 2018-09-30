#ifndef TTF_H
#define TTF_H
#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <unistd.h>
#include <SDL/SDL_ttf.h>
#include "Utility.h"

using namespace std;

class TTF {
private:
	TTF_Font *tipo;
	int tamanio;
	SDL_Surface *surface;
	SDL_Color color;

public:
	SDL_Rect rectangulo;
	TTF( int , int , int , int , int , int );
	SDL_Surface * getSurface(string);
	SDL_Rect * getRect() { return &(this->rectangulo); }
	void changeRectOrigin(int x, int y) { this->rectangulo.x = x; this->rectangulo.y = y; }
	~TTF();
};

#endif