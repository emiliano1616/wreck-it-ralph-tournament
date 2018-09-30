#ifndef PANTALLASERVIDOR_H
#define PANTALLASERVIDOR_H

#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <unistd.h>
#include "TTF.h"
#include "Utility.h"
#include "ConfigFile.h"
#include <cstring>
#include <string>
#include <cstdlib>
#include <sstream>

using namespace std;

class PantallaServidor {
private:
	SDL_Surface *pantallaActual, *fondo, *fix_it_felix, *resaltador, *opcion;
	SDL_Rect rectangulo;
	TTF *ttfOpcion, *ttfMensaje;



	// Gráficos de menús
	void mostrarMenuPrincipal(int);
	void mostrarMenuConfiguracion(int);



	// Gestión de telcas de menú...
	// ... principal
	int menuPrincipalTeclaArriba(int);
	int menuPrincipalTeclaAbajo(int);
	bool menuPrincipalTeclaEnter(int);

	// ... configuración
	int menuConfiguracionTeclaArriba(int);
	int menuConfiguracionTeclaAbajo(int);
	bool menuConfiguracionTeclaEnter(int);

	// Otros
	void limpiarMenu();
	void fixItFelixSlide();	




	// Procesos de pantallas
	void pantallaMenuPrincipal();
	void pantallaConfiguracion();
	void pantallaConfiguracionPuerto();
	void pantallaConfiguracionTiempoTorneo();
	void pantallaConfiguracionTiempoPartida();
	void pantallaConfiguracionTiempoInmunidad();
	void pantallaConfiguracionCantidadVidas();



	// Validaciones de datos
	bool enteroPositivo(string);


	// Ingreso de teclas
	string ingresoNumeroEntero(int, string, bool);
	

public:
	PantallaServidor(SDL_Surface *);
	~PantallaServidor();
	void pantallaDePresentacion();


	//Constantes de configuración

	// ... de códigos de opciones
	static int const OPCION_INICIAR_TORNEO = 0;
	static int const OPCION_CONFIGURAR = 1;
	static int const OPCION_SALIR = 2;
	static int const OPCION_CONFIGURACION_PUERTO = 0;
	static int const OPCION_CONFIGURACION_TIEMPO_TORNEO = 1;
	static int const OPCION_CONFIGURACION_TIEMPO_PARTIDA = 2;
	static int const OPCION_CONFIGURACION_TIEMPO_INMUNIDAD = 3;
	static int const OPCION_CONFIGURACION_CANTIDAD_VIDAS = 4;
	static int const OPCION_CONFIGURACION_ATRAS = 5;	

	// ... de posiciones en la pantalla
	static int const RESOLUCION_X = 800;
	static int const RESOLUCION_Y = 600;
	static int const INICIO_Y_FIX_IT_FELIX = 100;
	static int const INICIO_Y_MENU = 300;
	static int const INTERLINEADO = 5;

	// ... de tiempos
	static int const FIX_IT_FELIX_DELAY = 2000;
	static int const FIX_IT_FELIX_FRECUENCIA = 5;
	static int const DELAY_POLLING_ESTANDAR = 40 * 1000;

	// Otros
	static int const TAMANIO_LETRA_ESTANDAR = 18;
};

#endif