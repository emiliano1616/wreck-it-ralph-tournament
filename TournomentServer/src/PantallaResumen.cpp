#include "PantallaResumen.h"

PantallaResumen::PantallaResumen(SDL_Surface *pantallaPrincipal) {
	this->pantallaPrincipal = pantallaPrincipal;
	ttfResultados =  new TTF(15,0,0,255,255,255);
	ttfTitulo = new TTF(22,0,0,255,255,255);
	hh = mm = ss = 0;

	ultimoTiempo.x = 798; ultimoTiempo.y = 0; ultimoTiempo.w = 1; ultimoTiempo.h = 1;
	ultimoConectados.h = 30;
	ultimaTabla.h = ultimaTabla.w = 1;
	this->limpiarVentanaCompleta();
	this->dibujarTitulo();
}


//crea e imprime el surface del titulo del torneo
void PantallaResumen::dibujarTitulo() {
	mutexDibujo.lock();
	SDL_Rect rectangulo;
	SDL_Surface *titulo = ttfTitulo->getSurface("D-Ralph Tournament");
	inicioYResumen = titulo->h + 0;

	rectangulo.x = 0;
	rectangulo.y = 0;
	rectangulo.w = titulo->w;
	rectangulo.h = titulo->h;
	SDL_BlitSurface(titulo, NULL, pantallaPrincipal, &rectangulo);
	SDL_Flip(pantallaPrincipal);

	ultimoConectados.x = rectangulo.x;
	ultimoConectados.y = rectangulo.h + 5;
	ultimoConectados.w = 1;
	ultimoConectados.h = 1;
	mutexDibujo.unlock();

}

SDL_Surface* PantallaResumen::getTTFFromGame(Game juego) {
	string partida, puntaje1, puntaje2, nombre1, nombre2, id1, id2, ganador;
	SDL_Surface *partida_surface, *lineaJugador1, *lineaJugador2, *estado_surface , *return_surface;
	int maxW, maxH;
	SDL_Rect rectangulo;

	partida = to_string(juego.getId());
	nombre1 = juego.getJ1().getName();
	nombre2 = juego.getJ2().getName();
	puntaje1 = to_string(juego.getScorePlayer1());
	puntaje2 = to_string(juego.getScorePlayer2());
	id1 = to_string(juego.getJ1().getId());
	id2 = to_string(juego.getJ2().getId());

	if(juego.getIsFinished()) {
		ganador = puntaje1 > puntaje2 ? nombre1 : nombre2;
	}

	partida_surface = ttfResultados->getSurface("Partida ID:" + partida);
	lineaJugador1 = ttfResultados->getSurface(nombre1 + " (ID" + id1 + "): " + puntaje1 + " puntos");
	lineaJugador2 = ttfResultados->getSurface(nombre2 + " (ID" + id2 + "): " + puntaje2 + " puntos");
	if (!juego.getIsFinished()) {
		estado_surface = ttfResultados->getSurface("Jugandose");
	}
	else{
		estado_surface = ttfResultados->getSurface("Ganador: " + ganador);
	}

	if(lineaJugador1->w > lineaJugador2->w){
		maxW = lineaJugador1->w;
	}
	else{
		maxW = lineaJugador2->w;
	}

	maxH = partida_surface->h + lineaJugador1->h + lineaJugador2->h + estado_surface->h * 2;

	return_surface = SDL_CreateRGBSurface(0, maxW, maxH, 24, 0, 0, 0, 0);

	rectangulo.x = 0;
	rectangulo.y = 0;
	rectangulo.w = partida_surface->w;
	rectangulo.y = partida_surface->h;
	SDL_BlitSurface(partida_surface, NULL, return_surface, &rectangulo);

	rectangulo.y += partida_surface->h;
	rectangulo.w = lineaJugador1->w;
	rectangulo.h = lineaJugador1->h;
	SDL_BlitSurface(lineaJugador1, NULL, return_surface, &rectangulo);


	rectangulo.y += lineaJugador1->h;
	rectangulo.w = lineaJugador2->w;
	rectangulo.h = lineaJugador2->h;
	SDL_BlitSurface(lineaJugador2, NULL, return_surface, &rectangulo);


	rectangulo.y += lineaJugador2->h;
	rectangulo.w = estado_surface->w;
	rectangulo.h = estado_surface->h;
	SDL_BlitSurface(estado_surface, NULL, return_surface, &rectangulo);


	return return_surface;
}

void PantallaResumen::limpiarVentanaCompleta() {
	SDL_FillRect( pantallaPrincipal, NULL, SDL_MapRGB(pantallaPrincipal->format, 0, 0, 0));
}


list<string> PantallaResumen::obtenerTablaDePosiciones() {
	list<Player>::iterator auxI;
	int puntaje = 0;
	list<string> listaPosiciones;
	string stringAuxiliar;
	bool flag = false;
	multimap<int,int> puntos;

	// Armo una lista de jugadores

	list<Player> listaDeJugadores;

	for(list<Game>::iterator i = this->partidas.begin() ; i != this->partidas.end() ; i++ ) {

		flag = false;

		for(list<Player>::iterator a = listaDeJugadores.begin(); a != listaDeJugadores.end(); a++) {
			if((*a).getId() == (*i).getJ1().getId())
				flag = true;		
		}

		if(flag == false)
			listaDeJugadores.push_back((*i).getJ1());

		flag = false;

		for(list<Player>::iterator a = listaDeJugadores.begin(); a != listaDeJugadores.end(); a++) {
			if((*a).getId() == (*i).getJ2().getId())
				flag = true;		
		}

		if(flag == false)
			listaDeJugadores.push_back((*i).getJ2());
		
	}

	// Por cada jugador, recorro la lista de partidas y sumo los puntos sólo si la partida terminó.

	for(list<Player>::iterator k = listaDeJugadores.begin() ; k != listaDeJugadores.end(); k++) {
		puntaje = 0;
		for(list<Game>::iterator j = partidas.begin() ; j != partidas.end(); j++) {
			if((*j).getJ1().getId() == (*k).getId())
				puntaje += (*j).getScorePlayer1();

			if((*j).getJ2().getId() == (*k).getId())
				puntaje += (*j).getScorePlayer2();			
		}

		puntos.insert(pair<int,int>(puntaje, (*k).getId()));

		//stringAuxiliar = (*k).getName() + " (" + to_string((*k).getId()) + ") : " + to_string(puntaje);

		//listaPosiciones.push_back(stringAuxiliar);
	}

	//multimap<int, int> puntosOrdenados = puntos;

	for(multimap<int,int>::reverse_iterator puntosOrdenadosIterador = puntos.rbegin(); puntosOrdenadosIterador != puntos.rend(); puntosOrdenadosIterador++) {
		for(list<Player>::iterator iteradorPlayer = listaDeJugadores.begin(); iteradorPlayer != listaDeJugadores.end(); iteradorPlayer++) {
			if((*iteradorPlayer).getId() == (*puntosOrdenadosIterador).second){
				stringAuxiliar = (*iteradorPlayer).getName();
				break;
			}

		}
		stringAuxiliar += " (" + to_string((*puntosOrdenadosIterador).second) + ") : " + to_string((*puntosOrdenadosIterador).first);
		listaPosiciones.push_back(stringAuxiliar);
	}

	return listaPosiciones;

}





// Refrescos de pantalla

void PantallaResumen::refrescarConectados() {
	mutexDibujo.lock();
	SDL_Rect rectangulo;
	SDL_Surface *leyenda = ttfTitulo->getSurface("Jugadores conectados = " + to_string(this->conectados));

	rectangulo.x = ultimoConectados.x;
	rectangulo.y = ultimoConectados.y;
	rectangulo.w = leyenda->w;
	rectangulo.h = leyenda->h;
	SDL_FillRect( pantallaPrincipal, &ultimoConectados, SDL_MapRGB(pantallaPrincipal->format, 0, 0, 0));
	SDL_BlitSurface(leyenda, NULL, pantallaPrincipal, &rectangulo);
	SDL_Flip(pantallaPrincipal);

	ultimoConectados = rectangulo;
	mutexDibujo.unlock();
}

void PantallaResumen::refrescarTiempo() {
	mutexDibujo.lock();
	SDL_Rect rectangulo;
	string hora = hh <= 9 ? "0" + to_string(hh) : to_string(hh);
	string minuto = mm <= 9 ? "0" + to_string(mm) : to_string(mm);
	string segundo = ss <= 9 ? "0" + to_string(ss) : to_string(ss);


	SDL_Surface *leyenda = ttfTitulo->getSurface("Tiempo:  " + hora + ":" + minuto + ":" + segundo);

	rectangulo.x = 799 - leyenda->w;
	rectangulo.y = 0;
	rectangulo.w = leyenda->w;
	rectangulo.h = leyenda->h;
	SDL_FillRect( pantallaPrincipal, &ultimoTiempo, SDL_MapRGB(pantallaPrincipal->format, 0, 0, 0));
	SDL_BlitSurface(leyenda, NULL, pantallaPrincipal, &rectangulo);
	SDL_Flip(pantallaPrincipal);

	ultimoTiempo = rectangulo;
	mutexDibujo.unlock();
}

void PantallaResumen::refrescarPartidas() {
	mutexDibujo.lock();
	SDL_Rect rectangulo;
	SDL_Surface *ttf;
	ttf = NULL;


	/*--------------------------------------
	Parte 1 - Refrescar partidas
	--------------------------------------*/


	rectangulo.y = ultimoConectados.y + ultimoConectados.h + 5;
	SDL_FillRect( pantallaPrincipal, &ultimoResumen, SDL_MapRGB(pantallaPrincipal->format, 0, 0, 0));

	ultimoResumen.x = 400;
	ultimoResumen.y = rectangulo.y;
	ultimoResumen.w = ultimoResumen.h = 1;

	
	for(list<Game>::iterator i = partidas.begin() ; i != partidas.end() ; i++ ) {
		if(!(*i).getAliveJ1() && !(*i).getAliveJ2())
			continue;
		ttf = getTTFFromGame(*i);

		if(rectangulo.y + ttf->h > pantallaPrincipal->h) {

			sleep(5);
			SDL_FillRect( pantallaPrincipal, &ultimoResumen, SDL_MapRGB(pantallaPrincipal->format, 0, 0, 0));
			rectangulo.y = ultimoResumen.y;
			ultimoResumen.h += rectangulo.h;
			if(ultimoResumen.w < rectangulo.w) {
				ultimoResumen.w = rectangulo.w;
				ultimoResumen.x = (pantallaPrincipal->w/4) * 3 - ( rectangulo.w);
			}

		}

		rectangulo.x = (pantallaPrincipal->w / 4) * 3 - (ttf->w / 2);
		rectangulo.w = ttf->w;
		rectangulo.h = ttf->h;
		SDL_BlitSurface(ttf, NULL, pantallaPrincipal, &rectangulo);
		SDL_Flip(pantallaPrincipal);
		
		ultimoResumen.h += rectangulo.h;
		if(ultimoResumen.w < rectangulo.w) {
			ultimoResumen.w = rectangulo.w;
			ultimoResumen.x = (pantallaPrincipal->w/4) * 3 - ( rectangulo.w);
		}

		rectangulo.y += ttf->h;
	}

	/*--------------------------------------
	Parte 2 - Refrescar tabla de posiciones
	--------------------------------------*/

	ultimaTabla.y = ultimoConectados.y + ultimoConectados.h + 5;
	ultimaTabla.x = 0;	

	SDL_FillRect( pantallaPrincipal, &ultimaTabla, SDL_MapRGB(pantallaPrincipal->format, 0, 0, 0));	

	list<string> listaPosiciones = obtenerTablaDePosiciones();

	rectangulo.x = 0;
	rectangulo.y = ultimoConectados.y + ultimoConectados.h + 5;
	ttf = ttfResultados->getSurface("Puntaje: ");
	rectangulo.w = ttf->w;
	rectangulo.h = ttf->h;
	SDL_BlitSurface(ttf, NULL, pantallaPrincipal, &rectangulo);
	rectangulo.y += rectangulo.h;

	for(list<string>::iterator b = listaPosiciones.begin(); b != listaPosiciones.end() ; b++) {
		ttf = ttfResultados->getSurface(*b);
		rectangulo.w = ttf->w;
		rectangulo.h = ttf->h;
		SDL_BlitSurface(ttf, NULL, pantallaPrincipal, &rectangulo);

		ultimaTabla.h += rectangulo.h;
		if(ultimaTabla.w < rectangulo.w) {
			ultimaTabla.w = rectangulo.w;
		}		

		rectangulo.y += rectangulo.h;
	}


	mutexDibujo.unlock();
}





// Actualizaciones

void PantallaResumen::actualizarPartidas(list<Game> partidas) {
	this->partidas = partidas;
	this->refrescarPartidas();

}

void PantallaResumen::actualizarTiempo(int hh, int mm, int ss) {

	this->hh = hh;
	this->mm = mm;
	this->ss = ss;

	this->refrescarTiempo();

}

void PantallaResumen::actualizarConectados(int conectados) {
	this->conectados = conectados;
	this->refrescarConectados();
}








/*
int main() {
	list<Game> listaDeJugadas;
	SDL_Surface *pantallaPrincipal;

	// Inicio de SDL
	if( SDL_Init(SDL_INIT_VIDEO) != 0 ) {
		cout << SDL_GetError() << endl;
		exit(1);
	}

	pantallaPrincipal = SDL_SetVideoMode(800, 600, 24, SDL_DOUBLEBUF);
	if(pantallaPrincipal == NULL) {
		cout << SDL_GetError() << endl;
		exit(1);
	}

	PantallaResumen pantalla(pantallaPrincipal);

	// Jugadores
	Player jugador1(1,"jugador 1", 3);
	Player jugador2(2,"jugador 2", 3);
	Player jugador3(3,"jugador 3", 3);
	Player jugador4(4,"jugador 4", 3);
	Player jugador5(5,"jugador 5", 3);
	Player jugador6(6,"jugador 6", 3);
	Player jugador7(7,"jugador 7", 3);
	Player jugador8(8,"jugador 8", 3);
	Player jugador9(9,"jugador 9", 3);
	Player jugador10(10,"jugador 10", 3);
	Player jugador11(11,"jugador 11", 3);
	Player jugador12(12,"jugador 12", 3);

	// Juegos
	Game juego1(1,jugador1, jugador2);
	Game juego2(2,jugador3, jugador4);
	Game juego3(3,jugador5, jugador6);
	Game juego4(4,jugador7, jugador8);
	Game juego5(5,jugador9, jugador10);
	Game juego6(6,jugador11, jugador12);

	// Puntaje
	juego1.setScorePlayer1(10);
	juego1.setScorePlayer2(20);

	juego2.setScorePlayer1(30);
	juego2.setScorePlayer2(40);

	juego3.setScorePlayer1(50);
	juego3.setScorePlayer2(60);

	juego4.setScorePlayer1(100);
	juego4.setScorePlayer2(80);

	juego5.setScorePlayer1(90);
	juego5.setScorePlayer2(190);

	juego6.setScorePlayer1(1);
	juego6.setScorePlayer2(120);


	listaDeJugadas.push_back(juego1);
	listaDeJugadas.push_back(juego2);
	listaDeJugadas.push_back(juego3);
	listaDeJugadas.push_back(juego4);
	listaDeJugadas.push_back(juego5);
	listaDeJugadas.push_back(juego6);

	//pantalla.refrescarViejo(listaDeJugadas);
	pantalla.actualizarConectados(12);
	pantalla.actualizarTiempo(1,2,3);
	pantalla.actualizarPartidas(listaDeJugadas);

	for(int i = 3; i >= 0 ; i--) {



		pantalla.actualizarTiempo(i,i,i);	
		pantalla.actualizarConectados(i);
		pantalla.actualizarPartidas(listaDeJugadas);
		pantalla.actualizarConectados(i);
		sleep(1);
	}

	sleep(3);

	SDL_FreeSurface(pantallaPrincipal);
	SDL_Quit();	

	return 0;
}*/
