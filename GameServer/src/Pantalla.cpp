#include "Pantalla.h"
#include "Utility.h"

Pantalla::Pantalla()
{
	if( SDL_Init(SDL_INIT_VIDEO) != 0 )
		throw;	

	pantallaActual = SDL_SetVideoMode(800, 600, 32, SDL_DOUBLEBUF);

	if(pantallaActual == NULL) 
		throw;
}

void Pantalla::SplashScreen()
{
	SDL_Rect rectangulo;
	string PATH=Utility::GetPathBIN();
	fondo = IMG_Load((PATH+"images/fondo.jpg").c_str());
	fix_it_felix = IMG_Load((PATH+"images/logo.jpg").c_str());

	if ( fix_it_felix == NULL || fondo == NULL)
		throw;

	rectangulo.x = 799;
	rectangulo.y = 200;
	rectangulo.w = fix_it_felix->w;
	rectangulo.h = fix_it_felix->h;

	SDL_BlitSurface(fix_it_felix, NULL, pantallaActual, &rectangulo);
	SDL_Flip(pantallaActual);
	usleep(FIX_IT_FELIX_DELAY);

	for(; rectangulo.x > 250; rectangulo.x -= FIX_IT_FELIX_FRECUENCIA) {
		rectangulo.x++;
		SDL_BlitSurface(fondo, NULL, pantallaActual, &rectangulo );
		rectangulo.x--;
		SDL_BlitSurface(fix_it_felix, NULL, pantallaActual, &rectangulo);
		SDL_Flip(pantallaActual);
		usleep(FIX_IT_FELIX_DELAY);
	}

	


}
Pantalla::~Pantalla() 
{
	SDL_FreeSurface(pantallaActual);
	SDL_FreeSurface(fondo);
	SDL_FreeSurface(fix_it_felix);
	SDL_Quit();
}
