#include "Ventana.h"


string Ventana::getPath()
{
	return Utility::GetPathBIN() + "images/ventanas.png";
}

Ventana::Ventana(int pEstado, int pX, int pY)
{
	imagen = IMG_Load(this->getPath().c_str());
	if(!imagen)
	{
		puts(SDL_GetError());
		exit(1);
	}

	rectangulo.x = pX;
	rectangulo.y = pY;
	rectangulo.h = height;
	rectangulo.w = width;

	rectanguloSprites.w = width;
	rectanguloSprites.h = height;
	rectanguloSprites.x = width * pEstado;
	rectanguloSprites.y = 0;
	estado = pEstado;
	ultimoEstadoDibujado = -1;

}

Ventana::Ventana()
{
	imagen = NULL;

	/*IMG_Load(this->getPath().c_str());
	if(!imagen)
	{
		puts(SDL_GetError());
		exit(1);
	}*/
	//ultimoEstadoDibujado = -1;

	rectangulo.x = -1;
	rectangulo.y = -1;
	rectangulo.h = height;
	rectangulo.w = width;

	rectanguloSprites.w = width;
	rectanguloSprites.h = height;
	rectanguloSprites.x = -1;
	rectanguloSprites.y = 0;

	ultimoEstadoDibujado = -1;
	estado = -2;
}

Ventana::~Ventana()
{
	/*
	if(imagen)
		SDL_FreeSurface(imagen);
		*/
}

/*
enum TipoObjeto {
    VENTANA_SANA = 0,
    VENTANA_ROTA1 = 1,
    VENTANA_ROTA2 = 2,
    VENTANA_ROTA3 = 3,
    VENTANA_CERRADA = 4,
    VENTANA_ABIERTA_IZQ = 5,
    VENTANA_ABIERTA_DER = 6,
    BALCON = 7,
    GAVIOTA = 8,
    LADRILLO = 9,
    JUGADOR1 = 10,
    JUGADOR2 = 11,
    RALPH = 12
};
*/

void Ventana::repararVentana()
{

	//mutexUltimaVentana.lock();
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(1,2);
	if(estado == 1 || estado == 2)
	{
		estado = 0;
	}

	if(estado == 3)
	{
		estado = distribution(generator);;
	}

	rectanguloSprites.x = width * estado;
	//mutexUltimaVentana.unlock();


}		

bool Ventana::cambioEstado()
{
	//cout<<"hola vite estado: "<< estado<< "ultimo : " << ultimoEstadoDibujado<<endl;

	return estado != ultimoEstadoDibujado;
}

void Ventana::dibujar(SDL_Surface * pantalla)
{
	//cout<<"x: " << rectangulo.x << " y: " << rectangulo.y<<endl;
	//mutexUltimaVentana.lock();
	SDL_BlitSurface(imagen,&rectanguloSprites,pantalla,&rectangulo);
	ultimoEstadoDibujado = estado;
	//mutexUltimaVentana.unlock();
}

// SDL_Surface * Screen;

// void initScreen()
// {
//   if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
//   {
//     puts(SDL_GetError());
//     exit(1);
//   }

//   Screen = SDL_SetVideoMode(800, 600, 24, SDL_DOUBLEBUF);
//   if(!Screen)
//   {
//     puts(SDL_GetError());
//     exit(1);
//   }

//   atexit(SDL_Quit);
// }

// int main()
// {
// 	initScreen();
// 	Ventana obj(0, 50,50);
// 	Ventana obj2(1,200,200);

// 	SDL_BlitSurface(obj.imagen, &obj.rectanguloSprites,Screen,&obj.rectangulo);
// 	SDL_BlitSurface(obj2.imagen, &obj2.rectanguloSprites,Screen,&obj2.rectangulo);
// 	SDL_Flip(Screen);

// 	sleep(2);
// 	SDL_FreeSurface(Screen);
// 	return 0;
// }