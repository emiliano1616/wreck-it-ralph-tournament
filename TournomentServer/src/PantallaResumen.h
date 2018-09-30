#ifndef PANTALLA_RESUMEN_H
#define PANTALLA_RESUMEN_H

#include "Game.h"
#include <list>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "TTF.h"
#include <string>
#include <mutex>
#include <map>

using namespace std;

class PantallaResumen {
private:
	SDL_Surface *pantallaPrincipal;
	TTF *ttfResultados, *ttfTitulo;
	list<Game> partidas;
	int hh, mm, ss;
	int conectados;
	SDL_Rect ultimoTiempo;
	SDL_Rect ultimoConectados;
	SDL_Rect ultimoResumen;
	SDL_Rect ultimaTabla;
  	mutex mutexDibujo; 

	SDL_Surface* getTTFFromGame(Game);
	int inicioYResumen;
	void limpiarVentanaResumen();
	void limpiarVentanaCompleta();
	void refrescarPartidas();
	void refrescarTiempo();
	void refrescarConectados();
	list<string> obtenerTablaDePosiciones();

public:
	PantallaResumen();
	PantallaResumen(SDL_Surface*);
	void actualizarPartidas(list<Game>);
	void actualizarTiempo(int,int,int); // hh : mm : ss
	void actualizarConectados(int);
	void dibujarTitulo();	
};

#endif