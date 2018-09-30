#include "PantallaServidor.h"

using namespace std;


//------------------------------------------------------------------*
//	Constructor y destructor
//------------------------------------------------------------------*

PantallaServidor::PantallaServidor(SDL_Surface *pantallaActual) 
{
	fondo = IMG_Load((Utility::GetPathBIN() + "images/" + "fondo_presentacion.jpg").c_str());
	fix_it_felix = IMG_Load((Utility::GetPathBIN() + "images/" + "fix_it_felix_presentacion.jpg").c_str());
	resaltador = IMG_Load((Utility::GetPathBIN() + "images/" + "resaltador.jpg").c_str());


	this->pantallaActual = pantallaActual;

	if ( fix_it_felix == NULL || fondo == NULL || resaltador == NULL) {
		cout << SDL_GetError() << endl;
		exit(1);
	}

	ttfOpcion = new TTF(TAMANIO_LETRA_ESTANDAR, 0, 0, 255, 255, 255);
	ttfMensaje = new TTF(TAMANIO_LETRA_ESTANDAR, 0, 0, 255, 255, 255);
}

PantallaServidor::~PantallaServidor() {
	SDL_FreeSurface(fondo);
	SDL_FreeSurface(fix_it_felix);
	SDL_FreeSurface(resaltador);
	SDL_FreeSurface(opcion);
}







//------------------------------------------------------------------*
//	Procesos de pantallas
//------------------------------------------------------------------*

void PantallaServidor::pantallaDePresentacion() {
	this->fixItFelixSlide();
	this->pantallaMenuPrincipal();
}

void PantallaServidor::pantallaMenuPrincipal() {

	bool salir = false;
	SDL_Event evento;
	int opcionActual = OPCION_INICIAR_TORNEO;
	bool blnHasChanged=true;
	while( !salir ) {
		usleep(DELAY_POLLING_ESTANDAR);

		if( SDL_PollEvent( &evento ) ) {

			if( evento.type == SDL_KEYDOWN ) {

				int key = (int)evento.key.keysym.sym;
				switch(key) {
					case SDLK_UP:
						opcionActual = menuPrincipalTeclaArriba(opcionActual);
						break;

					case SDLK_DOWN:
						opcionActual = menuPrincipalTeclaAbajo(opcionActual);
						break;

					case SDLK_RETURN:
						salir = this->menuPrincipalTeclaEnter(opcionActual);
						break; 
				}
				blnHasChanged=true;
			}

			else if( evento.type == SDL_QUIT ) {
				exit(0);
			}
			if(blnHasChanged)
				this->mostrarMenuPrincipal(opcionActual);
			blnHasChanged=false;
		}
	}
}

void PantallaServidor::pantallaConfiguracion() {
	bool salir = false;
	SDL_Event evento;
	int opcionActual = OPCION_CONFIGURACION_PUERTO;
	bool blnHasChanged=true;
	while( !salir ) {
		usleep(DELAY_POLLING_ESTANDAR);

		if( SDL_PollEvent( &evento ) ) {
			if( evento.type == SDL_KEYDOWN ) {

				int key = (int)evento.key.keysym.sym;
				switch(key) {
					case SDLK_UP:
						opcionActual = menuConfiguracionTeclaArriba(opcionActual);
						break;

					case SDLK_DOWN:
						opcionActual = menuConfiguracionTeclaAbajo(opcionActual);
						break;

					case SDLK_RETURN:
						salir = this->menuConfiguracionTeclaEnter(opcionActual);
						break; 
				}
				blnHasChanged=true;
			}

			else if( evento.type == SDL_QUIT ) {
				salir = true;
				exit(0);
			}
			if(blnHasChanged)
				this->mostrarMenuConfiguracion(opcionActual);
			blnHasChanged=false;
		}
	}	
}

void PantallaServidor::pantallaConfiguracionPuerto() {

	string valorIngresado;
	ConfigFile* c = new ConfigFile("config.conf");
	c->Load();	

	this->limpiarMenu();
	valorIngresado = this->ingresoNumeroEntero( INICIO_Y_MENU + opcion->h, "PUERTO", false);
	while(! this->enteroPositivo(valorIngresado)) {
		this->limpiarMenu();
		valorIngresado = this->ingresoNumeroEntero( INICIO_Y_MENU + opcion->h, "PUERTO", true);
	}

	c->SetSetting("[Server]", "Port", valorIngresado);

}

void PantallaServidor::pantallaConfiguracionTiempoTorneo() {

	string valorIngresado;
	ConfigFile* c = new ConfigFile("config.conf");
	c->Load();	

	this->limpiarMenu();
	valorIngresado = this->ingresoNumeroEntero( INICIO_Y_MENU + opcion->h, "TIEMPO TORNEO (Minutos)", false);
	while(! this->enteroPositivo(valorIngresado)) {
		this->limpiarMenu();
		valorIngresado = this->ingresoNumeroEntero( INICIO_Y_MENU + opcion->h, "TIEMPO TORNEO (Minutos)", true);
	}

	c->SetSetting("[Server]", "TournomentTime", valorIngresado);	

}

void PantallaServidor::pantallaConfiguracionTiempoPartida() {

	string valorIngresado;
	ConfigFile* c = new ConfigFile("config.conf");
	c->Load();	

	this->limpiarMenu();
	valorIngresado = this->ingresoNumeroEntero( INICIO_Y_MENU + opcion->h, "TIEMPO PARTIDA (Minutos)", false);
	while(! this->enteroPositivo(valorIngresado)) {
		this->limpiarMenu();
		valorIngresado = this->ingresoNumeroEntero( INICIO_Y_MENU + opcion->h, "TIEMPO PARTIDA (Minutos)", true);
	}

	c->SetSetting("[Server]", "GameTime", valorIngresado);	

}

void PantallaServidor::pantallaConfiguracionTiempoInmunidad() {

	string valorIngresado;
	ConfigFile* c = new ConfigFile("config.conf");
	c->Load();	

	this->limpiarMenu();
	valorIngresado = this->ingresoNumeroEntero( INICIO_Y_MENU + opcion->h, "TIEMPO INMUNIDAD (segundos)", false);
	while(! this->enteroPositivo(valorIngresado)) {
		this->limpiarMenu();
		valorIngresado = this->ingresoNumeroEntero( INICIO_Y_MENU + opcion->h, "TIEMPO INMUNIDAD (segundos)", true);
	}

	c->SetSetting("[Server]", "ImmunityTime", valorIngresado);		

}

void PantallaServidor::pantallaConfiguracionCantidadVidas() {

	string valorIngresado;
	ConfigFile* c = new ConfigFile("config.conf");
	c->Load();	

	this->limpiarMenu();
	valorIngresado = this->ingresoNumeroEntero( INICIO_Y_MENU + opcion->h, "CANTIDAD VIDAS", false);
	while(! this->enteroPositivo(valorIngresado)) {
		this->limpiarMenu();
		valorIngresado = this->ingresoNumeroEntero( INICIO_Y_MENU + opcion->h, "CANTIDAD VIDAS", true);
	}

	c->SetSetting("[Server]", "GameLife", valorIngresado);	

}




//------------------------------------------------------------------*
//	Gráficos de menús
//------------------------------------------------------------------*

void PantallaServidor::mostrarMenuPrincipal(int opcionActual) {

	this->limpiarMenu();

	rectangulo.x = (RESOLUCION_X / 2) - ( resaltador->w / 2);
	rectangulo.y = INICIO_Y_MENU + resaltador->h * opcionActual;
	rectangulo.w = resaltador->w;
	rectangulo.h = resaltador->h;
	SDL_BlitSurface(resaltador, NULL, pantallaActual, &rectangulo);

	opcion = this->ttfOpcion->getSurface("INICIAR TORNEO");
	rectangulo.x = (RESOLUCION_X / 2) - opcion->w / 2;
	rectangulo.y = INICIO_Y_MENU + ( (resaltador->h / 2) - opcion->h / 2);
	rectangulo.w = resaltador->w;
	rectangulo.h = resaltador->h;
	SDL_BlitSurface(opcion, NULL, pantallaActual, &rectangulo);

	opcion = this->ttfOpcion->getSurface("CONFIGURAR");
	rectangulo.x = (RESOLUCION_X / 2) - opcion->w / 2;
	rectangulo.y = INICIO_Y_MENU + resaltador->h + ( (resaltador->h / 2) - opcion->h / 2);
	rectangulo.w = resaltador->w;
	rectangulo.h = resaltador->h;
	SDL_BlitSurface(opcion, NULL, pantallaActual, &rectangulo);

	opcion = this->ttfOpcion->getSurface("SALIR");
	rectangulo.x = (RESOLUCION_X / 2) - opcion->w / 2;
	rectangulo.y = INICIO_Y_MENU + resaltador->h * 2 + ( (resaltador->h / 2) - opcion->h / 2);
	rectangulo.w = resaltador->w;
	rectangulo.h = resaltador->h;
	SDL_BlitSurface(opcion, NULL, pantallaActual, &rectangulo);	

	SDL_Flip(pantallaActual);
}

void PantallaServidor::mostrarMenuConfiguracion(int opcionActual) {

	this->limpiarMenu();

	rectangulo.x = (RESOLUCION_X / 2) - ( resaltador->w / 2);
	rectangulo.y = INICIO_Y_MENU + resaltador->h * opcionActual;
	rectangulo.w = resaltador->w;
	rectangulo.h = resaltador->h;
	SDL_BlitSurface(resaltador, NULL, pantallaActual, &rectangulo);

	opcion = this->ttfOpcion->getSurface("PUERTO");
	rectangulo.x = (RESOLUCION_X / 2) - opcion->w / 2;
	rectangulo.y = INICIO_Y_MENU + ( (resaltador->h / 2) - opcion->h / 2);
	rectangulo.w = resaltador->w;
	rectangulo.h = resaltador->h;
	SDL_BlitSurface(opcion, NULL, pantallaActual, &rectangulo);

	opcion = this->ttfOpcion->getSurface("TIEMPO TORNEO");
	rectangulo.x = (RESOLUCION_X / 2) - opcion->w / 2;
	rectangulo.y = INICIO_Y_MENU + resaltador->h + ( (resaltador->h / 2) - opcion->h / 2);
	rectangulo.w = resaltador->w;
	rectangulo.h = resaltador->h;
	SDL_BlitSurface(opcion, NULL, pantallaActual, &rectangulo);

	opcion = this->ttfOpcion->getSurface("TIEMPO PARTIDA");
	rectangulo.x = (RESOLUCION_X / 2) - opcion->w / 2;
	rectangulo.y = INICIO_Y_MENU + resaltador->h * 2 + ( (resaltador->h / 2) - opcion->h / 2);
	rectangulo.w = resaltador->w;
	rectangulo.h = resaltador->h;
	SDL_BlitSurface(opcion, NULL, pantallaActual, &rectangulo);

	opcion = this->ttfOpcion->getSurface("TIEMPO INMUNIDAD");
	rectangulo.x = (RESOLUCION_X / 2) - opcion->w / 2;
	rectangulo.y = INICIO_Y_MENU + resaltador->h * 3 + ( (resaltador->h / 2) - opcion->h / 2);
	rectangulo.w = resaltador->w;
	rectangulo.h = resaltador->h;
	SDL_BlitSurface(opcion, NULL, pantallaActual, &rectangulo);

	opcion = this->ttfOpcion->getSurface("CANTIDAD VIDAS");
	rectangulo.x = (RESOLUCION_X / 2) - opcion->w / 2;
	rectangulo.y = INICIO_Y_MENU + resaltador->h * 4 + ( (resaltador->h / 2) - opcion->h / 2);
	rectangulo.w = resaltador->w;
	rectangulo.h = resaltador->h;
	SDL_BlitSurface(opcion, NULL, pantallaActual, &rectangulo);	

	opcion = this->ttfOpcion->getSurface("ATRAS");
	rectangulo.x = (RESOLUCION_X / 2) - opcion->w / 2;
	rectangulo.y = INICIO_Y_MENU + resaltador->h * 5 + ( (resaltador->h / 2) - opcion->h / 2);
	rectangulo.w = resaltador->w;
	rectangulo.h = resaltador->h;
	SDL_BlitSurface(opcion, NULL, pantallaActual, &rectangulo);	

	SDL_Flip(pantallaActual);
}

void PantallaServidor::fixItFelixSlide() {
	int limiteDesplazamiento;

	rectangulo.x = RESOLUCION_X -1;
	rectangulo.y = INICIO_Y_FIX_IT_FELIX;
	rectangulo.w = fix_it_felix->w;
	rectangulo.h = fix_it_felix->h;

	SDL_BlitSurface(fix_it_felix, NULL, pantallaActual, &rectangulo);
	SDL_Flip(pantallaActual);
	usleep(FIX_IT_FELIX_DELAY);

	limiteDesplazamiento = (RESOLUCION_X / 2) - (fix_it_felix->w / 2) + 15; // 15 corresponde al desfasaje por el subindice "JR" en el dibujo de presentación
	for(; rectangulo.x > limiteDesplazamiento; rectangulo.x -= FIX_IT_FELIX_FRECUENCIA) {
		rectangulo.x++;
		SDL_BlitSurface(fondo, NULL, pantallaActual, &rectangulo );
		rectangulo.x--;
		SDL_BlitSurface(fix_it_felix, NULL, pantallaActual, &rectangulo);
		SDL_Flip(pantallaActual);
		usleep(FIX_IT_FELIX_DELAY);

	}
}

void PantallaServidor::limpiarMenu() {
	SDL_Rect rect;
	rect.x = 0;
	rect.y = INICIO_Y_MENU;
	rect.w = RESOLUCION_X;
	rect.h = resaltador->h * 6;
	SDL_FillRect(this->pantallaActual, &rect, 0x000000 );
}






//------------------------------------------------------------------*
//	Ingreso de teclas
//------------------------------------------------------------------*

string PantallaServidor::ingresoNumeroEntero(int yInicio, string atributo, bool mensajeError) {
	SDL_Event evento;
	bool salir = false;
	TTF ttfValor(TAMANIO_LETRA_ESTANDAR,0,0,255,255,255);
	string cadenaRetorno(""), stringAuxiliar;
	SDL_Surface *ipIngresado, *mensaje;
	SDL_Rect rectanguloMensaje;
	unsigned int cantidadMaxima = 6;
	TTF ttfMensajeError(TAMANIO_LETRA_ESTANDAR,0,0,255,0,0);
	stringstream ss;

	mensajeError == false ? (mensaje = this->ttfOpcion->getSurface("INGRESE " + atributo + ":")) : (mensaje = ttfMensajeError.getSurface(atributo + " ERRONEO, INGRESE " + atributo + ":"));
	rectanguloMensaje.x = (RESOLUCION_X / 2) - mensaje->w / 2;
	rectanguloMensaje.y = INICIO_Y_MENU + ( ( resaltador->h / 2 ) - mensaje->h / 2);
	rectanguloMensaje.w = resaltador->w;
	rectanguloMensaje.h = resaltador->h;	

	SDL_BlitSurface(mensaje, NULL, pantallaActual, &rectanguloMensaje);
	SDL_Flip(this->pantallaActual);	

	while(!salir) {

		usleep(DELAY_POLLING_ESTANDAR);

		if(SDL_PollEvent(&evento)) {

			if(evento.type == SDL_KEYDOWN) {
			
				int key = (int)evento.key.keysym.sym;
				ss.clear();

				if( key == SDLK_BACKSPACE && cadenaRetorno.length() > 0 )
					cadenaRetorno = cadenaRetorno.substr(0,cadenaRetorno.length() - 1 );

				else if( key == SDLK_RETURN || key == SDLK_KP_ENTER)
					salir = true;				

				else if( cadenaRetorno.length() < cantidadMaxima ) {
					if( key >= SDLK_0 && key <= SDLK_9) {
						ss << (char) key;
						ss >> stringAuxiliar;
						cadenaRetorno += stringAuxiliar;
					}
					else if( key >= SDLK_KP0 && key <= SDLK_KP9 ) {
						ss << (char) (key - (SDLK_KP0 - SDLK_0));
						ss >> stringAuxiliar;
						cadenaRetorno += stringAuxiliar;				
					}
				}	

				// Limpio la pantalla de la IP anterior:
				rectangulo.x = (RESOLUCION_X / 2) - (ipIngresado->w / 2);
				rectangulo.y = yInicio + INTERLINEADO;
				rectangulo.w = ipIngresado->w;
				rectangulo.h = ipIngresado->h;				
				SDL_BlitSurface( fondo, &rectangulo, this->pantallaActual, &rectangulo);

				if(cadenaRetorno.length() > 0) {
					ipIngresado = ttfValor.getSurface(cadenaRetorno);
					rectangulo.x = (RESOLUCION_X / 2) - (ipIngresado->w / 2 );
					rectangulo.y = yInicio + INTERLINEADO;
					rectangulo.w = ipIngresado->w;
					rectangulo.h = ipIngresado->h;
					SDL_BlitSurface( ipIngresado, NULL, this->pantallaActual, &rectangulo);
				}

				SDL_BlitSurface(mensaje, NULL, pantallaActual, &rectanguloMensaje);
				SDL_Flip(this->pantallaActual);
			}

			else if(evento.type == SDL_QUIT)
				exit(0);
		}
	}
	return cadenaRetorno;
}






//------------------------------------------------------------------*
//	Manejadores de eventos de menús
//------------------------------------------------------------------*


// De menú principal

int PantallaServidor::menuPrincipalTeclaAbajo(int opcionActual) {
	if(opcionActual < OPCION_SALIR)
		return opcionActual + 1;
	return opcionActual;
}

int PantallaServidor::menuPrincipalTeclaArriba(int opcionActual) {
	if(opcionActual > OPCION_INICIAR_TORNEO)
		return opcionActual - 1;
	return opcionActual;
}

bool PantallaServidor::menuPrincipalTeclaEnter(int opcionActual) {
	if (opcionActual == OPCION_SALIR)
		exit(0);

	if( opcionActual == OPCION_INICIAR_TORNEO)
		return true;

	if (opcionActual == OPCION_CONFIGURAR) {
		this->pantallaConfiguracion();
		return false;
	}

	return false;
}


// De menú configuración

int PantallaServidor::menuConfiguracionTeclaArriba(int opcionActual) {
	if (opcionActual > OPCION_CONFIGURACION_PUERTO)
		return opcionActual - 1;
	return opcionActual;
}

int PantallaServidor::menuConfiguracionTeclaAbajo(int opcionActual) {
	if (opcionActual < OPCION_CONFIGURACION_ATRAS)
		return opcionActual + 1;
	return opcionActual;
}

bool PantallaServidor::menuConfiguracionTeclaEnter(int opcionActual) {
	if (opcionActual == OPCION_CONFIGURACION_ATRAS)
		return true;

	if( opcionActual == OPCION_CONFIGURACION_PUERTO) {
		this->pantallaConfiguracionPuerto();
		return false;
	}

	if( opcionActual == OPCION_CONFIGURACION_TIEMPO_TORNEO) {
		this->pantallaConfiguracionTiempoTorneo();
		return false;
	}

	if( opcionActual == OPCION_CONFIGURACION_TIEMPO_PARTIDA) {
		this->pantallaConfiguracionTiempoPartida();
		return false;
	}

	if( opcionActual == OPCION_CONFIGURACION_TIEMPO_INMUNIDAD) {
		this->pantallaConfiguracionTiempoInmunidad();
		return false;
	}

	if( opcionActual == OPCION_CONFIGURACION_CANTIDAD_VIDAS) {
		this->pantallaConfiguracionCantidadVidas();
		return false;
	}		

	return false;
}

//------------------------------------------------------------------*
//	Validaciones de datos
//------------------------------------------------------------------*

bool PantallaServidor::enteroPositivo(string numero) {
	if (numero.empty() || stoi(numero) <= 0 )
		return false;
	return true;
}
