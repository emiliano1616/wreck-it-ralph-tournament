#include "TTF.h"

TTF::TTF(int tamanio, int x, int y, int r, int g, int b) {
	string pathTipo;

	if ( TTF_WasInit() == 0 )
		if( TTF_Init() != 0 ) {
			cout << TTF_GetError() << endl;
		}

	if(tamanio <= 0){
		cout << TTF_GetError() << endl;
	}
	this->tamanio = tamanio;

	this->tipo = TTF_OpenFont( (Utility::GetPathBIN() + "fonts/" + "arial.ttf").c_str() , this->tamanio );
	if(this->tipo == NULL){
		cout << TTF_GetError() << endl;
	}

	surface = NULL;

	rectangulo.x = x;
	rectangulo.y = y;

	color.r = r;
	color.g = g;
	color.b = b;
}

SDL_Surface * TTF::getSurface(string texto) {
	if (texto.empty())
		return NULL;

	this->surface = TTF_RenderText_Solid( this->tipo, texto.c_str(), this->color );
	if(this->surface == NULL) 
		return NULL;

	this->rectangulo.w = this->surface->w;
	this->rectangulo.h = this->surface->h;
	return this->surface;
}

TTF::~TTF() {
	SDL_FreeSurface(surface);
	TTF_CloseFont(tipo);
}