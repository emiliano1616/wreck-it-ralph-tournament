#include <iostream>
#include "Utility.h"
#include "SDL/SDL.h"
#include <random>
#include <mutex>
#include <SDL/SDL_image.h>


using namespace std;

class Ventana {
private:
	string getPath();
	const static int width = 70;
	const static int height = 78;
public:
	int fila;
	int columna;
	int estado;
	int ultimoEstadoDibujado;
	//mutex mutexUltimaVentana;
	SDL_Surface * imagen;
	SDL_Rect rectangulo;
	SDL_Rect rectanguloSprites;
	Ventana(int estado, int x, int y);
	Ventana();
	~Ventana();
	bool cambioEstado();
	void repararVentana();
	void dibujar(SDL_Surface *);
};