#ifndef OBJETODIBUJABLE_H
#define OBJETODIBUJABLE_H

#include <unistd.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <signal.h>
#include <thread>
#include <iostream>
#include <list>
#include <vector>
#include "TTF.h"
#include <mutex>

#define SECONDPLAYERSPRITES 14
#define REVERSEPLAYERSPRITES 7

#define RALPHSTAND 10
#define RALPHREVERSESPRITES 5
#define RALPHSPINING

using namespace std;

class ObjetoDibujable{

private:
	string getPathBinario();

public:

	static bool rectangulosChocando(SDL_Rect &,SDL_Rect&);
	SDL_Surface * imagen;
	SDL_Rect ultimoRectanguloDibujadoConOffset;
	SDL_Rect ultimoRectanguloDibujadoSinOffset;
	SDL_Rect rectangulo;
	SDL_Rect rectanguloSprites;
	ObjetoDibujable(int,int,int ,int );
	ObjetoDibujable(int,int);
	~ObjetoDibujable();
	void mover();
	virtual void mover(int x, int y);
	bool ocupado;
	mutex mutexUltimoRectangulo;
	bool seMovio();
	int actualSprite;
	int anteriorSprite;
	void dibujar(SDL_Surface *,int offset);
	bool primerDibujo;
	bool primerDesocupado;
protected:
	bool igualSigno(int,int);
	int xfin;
	int yfin;
	int refresco;
	int cantidadSprite;
	int refrescoSprite;
	float width;
	int heigth;
	string pathBinario;
	virtual string getPath() {return NULL; }// = 0;
	virtual void refrescar();


	virtual void nextSprite();


};




class Gaviota : public ObjetoDibujable {
private:
	string getPath() {return pathBinario +( rectangulo.x > xfin ? "images/koopaIzquierda.png" : "images/koopaDerecha.png");}

public:
	Gaviota(int,int,int dx = 0,int dy = 0);
	void setLevelDificulty(int);
};


class Ralph : public ObjetoDibujable {
private:
	string getPath() {return pathBinario + "images/bowser.png";}
	int Parado() { return RALPHSTAND; }
	const static int corridaBegin = 0;
	int CorridaBegin() { return offsetReversa + corridaBegin; }
	const static int corridaEnd = 4;
	int CorridaEnd() { return offsetReversa + corridaEnd; }
	int offsetReversa;
	mutex mutexMovimiento;
	list<pair<int,int>> listaMovimientos;
	void moverDerecha();
	void moverIzquierda();
	void refrescar();
	void nextSprite() ;

	int fila;
	int columna;
public:
	void reiniciar(int ,int);
	void mover(int x, int y);
	Ralph(int,int);
};



class Felix : public ObjetoDibujable {
private:
	
	string getPath() {return pathBinario + "images/playerSprites.png" ;}
	const static int parado = 0;
	int Parado() { return offsetPlayer + offsetReversa + parado; }
	const static int corrida1 = 1;
	int Corrida1() { return offsetPlayer + offsetReversa + corrida1; }
	const static int corrida2 = 2;
	int Corrida2() { return offsetPlayer + offsetReversa + corrida2; }
	const static int salto1 = 3;
	int Salto1() { return offsetPlayer + offsetReversa + salto1; }
	const static int salto2 = 4;
	int Salto2() { return offsetPlayer + offsetReversa + salto2; }
	const static int caida = 5;
	int Caida() { return offsetPlayer + offsetReversa + caida; }
	const static int perder = 6;
	int Perder() { return offsetPlayer + offsetReversa + perder; }
	int offsetPlayer;
	int offsetReversa;
	mutex mutexMovimiento;
	mutex mutexPerderVida;
	int ventanasReparadas;
	list<pair<int,int>> listaMovimientos;


	void saltar();
	void bajar();
	void moverDerecha();
	void moverIzquierda();
	void spritePerderVida();
	void spriteInvulnerable();
	void refrescar();
	void perderVidaThread();
	void invulnerableThread(int);

	thread * t;
	thread * thInvulnerable;

public:
	void sumarVentanaReparada(int);
	bool soyInvulnerable;
	SDL_Surface * imagenVida;
	SDL_Rect rectanguloVida;
	string nombre;
	bool perdiendoVida;
	void perderVida();
	Felix(int x, int y, int pNumeroJugador,string Nombre) ;
	~Felix();
	void reparar();
	void mover(int x, int y);
	int numeroJugador;
	TTF * puntosDibujables;
	int ultimosPuntosDibujados;
	int puntos;
	void sumarPuntos(int puntos);
	mutex mutexPuntos;
	int vidasRestantes;
	int ultimasVidasDibujadas;
	int fila;
	int columna;
	bool nuevosPuntos();
	void reiniciar(int x, int y, int pnumero, string);

	void dibujarVidas(SDL_Surface *);

protected:
	void nextSprite() ;

};



class Ladrillo : public ObjetoDibujable {
private:
	string getPath() {return pathBinario + "images/flama.png";}
public:
	Ladrillo(int,int,int,int);
	void setLevelDificulty(int);
};


#endif