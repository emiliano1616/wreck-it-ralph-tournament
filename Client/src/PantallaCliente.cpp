#include "PantallaCliente.h"

using namespace std;


//------------------------------------------------------------------*
//	Constructor y destructor
//------------------------------------------------------------------*

PantallaCliente::PantallaCliente(SDL_Surface *pantallaActual) {
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

PantallaCliente::~PantallaCliente() {
	SDL_FreeSurface(fondo);
	SDL_FreeSurface(fix_it_felix);
	SDL_FreeSurface(resaltador);
	SDL_FreeSurface(opcion);
}







//------------------------------------------------------------------*
//	Procesos de pantallas
//------------------------------------------------------------------*

void PantallaCliente::pantallaDePresentacion() {
	this->fixItFelixSlide();
	this->pantallaMenuPrincipal();
}

void PantallaCliente::pantallaMenuPrincipal() {

	bool salir = false;
	SDL_Event evento;
	int opcionActual = OPCION_JUGAR;
	DEFAULT_NAME="default";

	this->mostrarMenuPrincipal(opcionActual);
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
						salir = menuPrincipalTeclaEnter(opcionActual);
						break; 
				}
			}

			else if( evento.type == SDL_QUIT ) {
				exit(0);
			}

			this->mostrarMenuPrincipal(opcionActual);			
		}
	}
}

void PantallaCliente::pantallaConfiguracion() {
	bool salir = false;
	SDL_Event evento;
	int opcionActual = OPCION_CONFIGURACION_TECLAS;

	this->mostrarMenuConfiguracion(opcionActual);
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
			}

			else if( evento.type == SDL_QUIT ) {
				salir = true;
				exit(0);
			}

			this->mostrarMenuConfiguracion(opcionActual);
		}
	}	
}

void PantallaCliente::pantallaConfiguracionServidor() {
	bool salir = false;
	SDL_Event evento;
	int opcionActual = OPCION_CONFIGURACION_SERVIDOR_IP;

	this->mostrarMenuConfiguracionServidor(opcionActual);
	while( !salir ) {
		usleep(DELAY_POLLING_ESTANDAR);

		if( SDL_PollEvent( &evento ) ) {
			if( evento.type == SDL_KEYDOWN ) {

				int key = (int)evento.key.keysym.sym;
				switch(key) {
					case SDLK_UP:
						opcionActual = menuConfiguracionServidorTeclaArriba(opcionActual);
						break;

					case SDLK_DOWN:
						opcionActual = menuConfiguracionServidorTeclaAbajo(opcionActual);
						break;

					case SDLK_RETURN:
						salir = this->menuConfiguracionServidorTeclaEnter(opcionActual);
						break; 
				}
			}

			else if( evento.type == SDL_QUIT ) {
				exit(0);
			}

			this->mostrarMenuConfiguracionServidor(opcionActual);
		}
	}	
}

void PantallaCliente::pantallaConfiguracionPuerto() {

	string valorIngresado;
    ConfigFile* c = new ConfigFile("config.conf");
    c->Load();	

	this->limpiarMenu();
	valorIngresado = this->ingresoNumeroEntero( INICIO_Y_MENU + opcion->h, "PORT", false);
	while(! this->enteroPositivo(valorIngresado)) {
		this->limpiarMenu();
		valorIngresado = this->ingresoNumeroEntero( INICIO_Y_MENU + opcion->h, "PORT", true);
	}

	c->SetSetting("[Server]", "PORT", valorIngresado);
	delete c;

}

void PantallaCliente::pantallaConfiguracionHost() {
	string valorIngresado;
	ConfigFile* c = new ConfigFile("config.conf");
	c->Load();	

	this->limpiarMenu();
	valorIngresado = this->ingresoValorAlfanumerico( INICIO_Y_MENU + opcion->h, "IP", false);
	while(valorIngresado.empty()) {
		this->limpiarMenu();
		valorIngresado = this->ingresoValorAlfanumerico( INICIO_Y_MENU + opcion->h, "IP", true);
	}

	c->SetSetting("[Server]", "IP", valorIngresado);
	delete c;
}

void PantallaCliente::pantallaConfiguracionNombre() {
	string valorIngresado;
	ConfigFile* c = new ConfigFile("config.conf");
	c->Load();

	this->limpiarMenu();
	valorIngresado = this->ingresoValorAlfanumerico( INICIO_Y_MENU + opcion->h, "NAME", false);
	while(valorIngresado.empty()) {
		this->limpiarMenu();
		valorIngresado = this->ingresoValorAlfanumerico( INICIO_Y_MENU + opcion->h, "NAME", true);
	}

	c->SetSetting("[Client]", "NAME", valorIngresado);
	delete c;
}






//------------------------------------------------------------------*
//	Gráficos de menús
//------------------------------------------------------------------*

void PantallaCliente::mostrarMenuPrincipal(int opcionActual) {

	this->limpiarMenu();

	rectangulo.x = (RESOLUCION_X / 2) - ( resaltador->w / 2);
	rectangulo.y = INICIO_Y_MENU + resaltador->h * opcionActual;
	rectangulo.w = resaltador->w;
	rectangulo.h = resaltador->h;
	SDL_BlitSurface(resaltador, NULL, pantallaActual, &rectangulo);

	opcion = this->ttfOpcion->getSurface("JUGAR");
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

void PantallaCliente::mostrarMenuConfiguracion(int opcionActual) {

	this->limpiarMenu();

	rectangulo.x = (RESOLUCION_X / 2) - ( resaltador->w / 2);
	rectangulo.y = INICIO_Y_MENU + resaltador->h * opcionActual;
	rectangulo.w = resaltador->w;
	rectangulo.h = resaltador->h;
	SDL_BlitSurface(resaltador, NULL, pantallaActual, &rectangulo);

	opcion = this->ttfOpcion->getSurface("TECLAS");
	rectangulo.x = (RESOLUCION_X / 2) - opcion->w / 2;
	rectangulo.y = INICIO_Y_MENU + ( (resaltador->h / 2) - opcion->h / 2);
	rectangulo.w = resaltador->w;
	rectangulo.h = resaltador->h;
	SDL_BlitSurface(opcion, NULL, pantallaActual, &rectangulo);

	opcion = this->ttfOpcion->getSurface("SERVIDOR");
	rectangulo.x = (RESOLUCION_X / 2) - opcion->w / 2;
	rectangulo.y = INICIO_Y_MENU + resaltador->h + ( (resaltador->h / 2) - opcion->h / 2);
	rectangulo.w = resaltador->w;
	rectangulo.h = resaltador->h;
	SDL_BlitSurface(opcion, NULL, pantallaActual, &rectangulo);

	opcion = this->ttfOpcion->getSurface("NOMBRE");
	rectangulo.x = (RESOLUCION_X / 2) - opcion->w / 2;
	rectangulo.y = INICIO_Y_MENU + resaltador->h * 2 + ( (resaltador->h / 2) - opcion->h / 2);
	rectangulo.w = resaltador->w;
	rectangulo.h = resaltador->h;
	SDL_BlitSurface(opcion, NULL, pantallaActual, &rectangulo);	

	opcion = this->ttfOpcion->getSurface("ATRAS");
	rectangulo.x = (RESOLUCION_X / 2) - opcion->w / 2;
	rectangulo.y = INICIO_Y_MENU + resaltador->h * 3 + ( (resaltador->h / 2) - opcion->h / 2);
	rectangulo.w = resaltador->w;
	rectangulo.h = resaltador->h;
	SDL_BlitSurface(opcion, NULL, pantallaActual, &rectangulo);	

	SDL_Flip(pantallaActual);
}

void PantallaCliente::mostrarMenuConfiguracionServidor(int opcionActual) {

	this->limpiarMenu();

	rectangulo.x = (RESOLUCION_X / 2) - ( resaltador->w / 2);
	rectangulo.y = INICIO_Y_MENU + resaltador->h * opcionActual;
	rectangulo.w = resaltador->w;
	rectangulo.h = resaltador->h;
	SDL_BlitSurface(resaltador, NULL, pantallaActual, &rectangulo);

	opcion = this->ttfOpcion->getSurface("HOST");
	rectangulo.x = (RESOLUCION_X / 2) - opcion->w / 2;
	rectangulo.y = INICIO_Y_MENU + ( (resaltador->h / 2) - opcion->h / 2);
	rectangulo.w = resaltador->w;
	rectangulo.h = resaltador->h;
	SDL_BlitSurface(opcion, NULL, pantallaActual, &rectangulo);

	opcion = this->ttfOpcion->getSurface("PUERTO");
	rectangulo.x = (RESOLUCION_X / 2) - opcion->w / 2;
	rectangulo.y = INICIO_Y_MENU + resaltador->h + ( (resaltador->h / 2) - opcion->h / 2);
	rectangulo.w = resaltador->w;
	rectangulo.h = resaltador->h;
	SDL_BlitSurface(opcion, NULL, pantallaActual, &rectangulo);

	opcion = this->ttfOpcion->getSurface("ATRAS");
	rectangulo.x = (RESOLUCION_X / 2) - opcion->w / 2;
	rectangulo.y = INICIO_Y_MENU + resaltador->h * 2 + ( (resaltador->h / 2) - opcion->h / 2);
	rectangulo.w = resaltador->w;
	rectangulo.h = resaltador->h;
	SDL_BlitSurface(opcion, NULL, pantallaActual, &rectangulo);	

	SDL_Flip(pantallaActual);
}

void PantallaCliente::mostrarNombreIncorrecto() {

	TTF ttfNombreIncorrecto(TAMANIO_LETRA_ESTANDAR,0,0,255,0,0);
	SDL_Surface *mensaje;

	this->limpiarMenu();
	mensaje = ttfNombreIncorrecto.getSurface("Debe ingresar un nombre para jugar");
	rectangulo.x = RESOLUCION_X / 2 - (mensaje->w / 2);
	rectangulo.y = INICIO_Y_MENU;
	rectangulo.h = mensaje->h;
	rectangulo.w = mensaje->w;
	SDL_BlitSurface(mensaje,NULL,this->pantallaActual, &rectangulo );
	SDL_Flip(this->pantallaActual);

}

void PantallaCliente::fixItFelixSlide() {
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

void PantallaCliente::limpiarMenu() {
	SDL_Rect rect;
	rect.x = 0;
	rect.y = INICIO_Y_MENU;
	rect.w = RESOLUCION_X;
	rect.h = resaltador->h * 4;
	SDL_FillRect(this->pantallaActual, &rect, 0x000000 );
}






//------------------------------------------------------------------*
//	Ingreso de teclas
//------------------------------------------------------------------*

string PantallaCliente::ingresoValorAlfanumerico( int yInicio, string atributo, bool mensajeError) {
	SDL_Event evento;
	bool salir = false;
	TTF ttfValor(TAMANIO_LETRA_ESTANDAR,0,0,255,255,255);
	string cadenaRetorno(""), stringAuxiliar;
	SDL_Surface *valorIngresado, *mensaje;
	SDL_Rect rectanguloMensaje;
	unsigned int cantidadMaxima = 80;
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

		if(SDL_WaitEvent(&evento)) {

			if(evento.type == SDL_KEYDOWN) {
			
				int key = (int)evento.key.keysym.sym;
				ss.clear();

				if( key == SDLK_BACKSPACE && cadenaRetorno.length() > 0 )
					cadenaRetorno = cadenaRetorno.substr(0,cadenaRetorno.length() - 1 );

				else if( key == SDLK_RETURN || key == SDLK_KP_ENTER)
					salir = true;
			
				else if( cadenaRetorno.length() < cantidadMaxima ) {
					if( ( key >= 33 && key <= 127 ) ) {
						ss << (char) key;
						ss >> stringAuxiliar;
						cadenaRetorno += stringAuxiliar;
					} 
					else if( key >= SDLK_KP0 && key <= SDLK_KP9) {
						ss << (char) (key - ( SDLK_KP0 - SDLK_0));
						ss >> stringAuxiliar;
						cadenaRetorno += stringAuxiliar;					
					} 
					else if( key == SDLK_KP_PERIOD ) {
						ss << '.';
						ss >> stringAuxiliar;
						cadenaRetorno += stringAuxiliar;					
					}
					else if( key == SDLK_KP_MINUS ) {
						ss << '-';
						ss >> stringAuxiliar;
						cadenaRetorno += stringAuxiliar;					
					}	
				}

				// Limpio la pantalla de la IP anterior:
				rectangulo.x = (RESOLUCION_X / 2) - (valorIngresado->w / 2);
				rectangulo.y = yInicio;
				rectangulo.w = valorIngresado->w;
				rectangulo.h = valorIngresado->h;				
				SDL_BlitSurface( fondo, &rectangulo, this->pantallaActual, &rectangulo);

				if(cadenaRetorno.length() > 0) {
					valorIngresado = ttfValor.getSurface(cadenaRetorno);
					rectangulo.x = (RESOLUCION_X / 2) - (valorIngresado->w / 2 );
					rectangulo.y = yInicio;
					rectangulo.w = valorIngresado->w;
					rectangulo.h = valorIngresado->h;
					SDL_BlitSurface( valorIngresado, NULL, this->pantallaActual, &rectangulo);
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

int PantallaCliente::obtenerTecla() {
	SDL_Event evento;
	while(true) {
		usleep(DELAY_POLLING_ESTANDAR);

		if(SDL_PollEvent(&evento)){
			if(evento.type == SDL_QUIT) {
				exit(0);
			}

			if(evento.type == SDL_KEYDOWN)
				return (int) evento.key.keysym.sym;
		}
	}

}

void PantallaCliente::ingresoDeConfiguracionTeclas() {
    ConfigFile* c = new ConfigFile("config.conf");
    c->Load();

    this->limpiarMenu();

	rectangulo.x = (RESOLUCION_X / 2) - ( resaltador->w / 2 );
	rectangulo.y = INICIO_Y_MENU;
	rectangulo.h = resaltador->h * 3;
	rectangulo.w = resaltador->w;
	SDL_BlitSurface(fondo, &rectangulo, pantallaActual, &rectangulo);	

	SDL_Surface *mensaje = this->ttfMensaje->getSurface("Ingrese tecla ARRIBA");
	rectangulo.x = (RESOLUCION_X / 2) - (mensaje->w / 2);
	rectangulo.y = INICIO_Y_MENU;
	rectangulo.w = mensaje->w;
	rectangulo.h = mensaje->h;
	SDL_BlitSurface(mensaje, NULL , this->pantallaActual , &rectangulo );
	SDL_Flip(pantallaActual);

	c->SetSetting("[Client]" , "KEYUP",std::to_string(this->obtenerTecla()));

	SDL_BlitSurface(fondo, &rectangulo, pantallaActual, &rectangulo);
	mensaje = this->ttfMensaje->getSurface("Ingrese tecla ABAJO");
	rectangulo.x = (RESOLUCION_X / 2) - (mensaje->w / 2);
	rectangulo.y = INICIO_Y_MENU;
	rectangulo.w = mensaje->w;
	rectangulo.h = mensaje->h;
	SDL_BlitSurface(mensaje, NULL , this->pantallaActual , &rectangulo );	
	SDL_Flip(pantallaActual);

	c->SetSetting("[Client]" , "KEYDOWN",std::to_string(this->obtenerTecla()));

	SDL_BlitSurface(fondo, &rectangulo, pantallaActual, &rectangulo);
	mensaje = this->ttfMensaje->getSurface("Ingrese tecla DERECHA");
	rectangulo.x = (RESOLUCION_X / 2) - (mensaje->w / 2);
	rectangulo.y = INICIO_Y_MENU;
	rectangulo.w = mensaje->w;
	rectangulo.h = mensaje->h;
	SDL_BlitSurface(mensaje, NULL , this->pantallaActual , &rectangulo );	
	SDL_Flip(pantallaActual);

	c->SetSetting("[Client]" , "KEYRIGTH",std::to_string(this->obtenerTecla()));

	SDL_BlitSurface(fondo, &rectangulo, pantallaActual, &rectangulo);
	mensaje = this->ttfMensaje->getSurface("Ingrese tecla IZQUIERDA");
	rectangulo.x = (RESOLUCION_X / 2) - (mensaje->w / 2);
	rectangulo.y = INICIO_Y_MENU;
	rectangulo.w = mensaje->w;
	rectangulo.h = mensaje->h;
	SDL_BlitSurface(mensaje, NULL , this->pantallaActual , &rectangulo );	
	SDL_Flip(pantallaActual);

	c->SetSetting("[Client]" , "KEYLEFT",std::to_string(this->obtenerTecla()));

	SDL_BlitSurface(fondo, &rectangulo, pantallaActual, &rectangulo);
	mensaje = this->ttfMensaje->getSurface("Ingrese tecla REPARAR");
	rectangulo.x = (RESOLUCION_X / 2) - (mensaje->w / 2);
	rectangulo.y = INICIO_Y_MENU;
	rectangulo.w = mensaje->w;
	rectangulo.h = mensaje->h;
	SDL_BlitSurface(mensaje, NULL , this->pantallaActual , &rectangulo );	
	SDL_Flip(pantallaActual);

	c->SetSetting("[Client]" , "KEYREPAIR",std::to_string(this->obtenerTecla()));

	SDL_BlitSurface(fondo, &rectangulo, pantallaActual, &rectangulo);
	delete c;
}

string PantallaCliente::ingresoNumeroEntero(int yInicio, string atributo, bool mensajeError) {
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

int PantallaCliente::menuPrincipalTeclaAbajo(int opcionActual) {
	if(opcionActual < OPCION_SALIR)
		return opcionActual + 1;
	return opcionActual;
}

int PantallaCliente::menuPrincipalTeclaArriba(int opcionActual) {
	if(opcionActual > OPCION_JUGAR)
		return opcionActual - 1;
	return opcionActual;
}

bool PantallaCliente::menuPrincipalTeclaEnter(int opcionActual) {
	if (opcionActual == OPCION_SALIR)
		exit(0);

	if( opcionActual == OPCION_JUGAR) {
		if(!this->nombreValido()) {
			this->mostrarNombreIncorrecto();
			sleep(2);
			return false;
		}
		return true;
	}

	if (opcionActual == OPCION_CONFIGURAR) {
		this->pantallaConfiguracion();
		return false;
	}

	return false;
}


// De menú configuración

int PantallaCliente::menuConfiguracionTeclaArriba(int opcionActual) {
	if (opcionActual > OPCION_CONFIGURACION_TECLAS)
		return opcionActual - 1;
	return opcionActual;
}

int PantallaCliente::menuConfiguracionTeclaAbajo(int opcionActual) {
	if (opcionActual < OPCION_CONFIGURACION_ATRAS)
		return opcionActual + 1;
	return opcionActual;
}

bool PantallaCliente::menuConfiguracionTeclaEnter(int opcionActual) {
	if (opcionActual == OPCION_CONFIGURACION_ATRAS)
		return true;

	if( opcionActual == OPCION_CONFIGURACION_TECLAS) {
		this->ingresoDeConfiguracionTeclas();
		return false;
	}

	if( opcionActual == OPCION_CONFIGURACION_SERVIDOR) {
		this->pantallaConfiguracionServidor();
		return false;
	}

	if( opcionActual == OPCION_CONFIGURACION_NOMBRE) {
		this->pantallaConfiguracionNombre();
		return false;
	}

	return false;
}


// De configuración del servidor

int PantallaCliente::menuConfiguracionServidorTeclaArriba(int opcionActual) {
	if (opcionActual > OPCION_CONFIGURACION_SERVIDOR_IP)
		return opcionActual - 1;
	return opcionActual;
}

int PantallaCliente::menuConfiguracionServidorTeclaAbajo(int opcionActual) {
	if (opcionActual < OPCION_CONFIGURACION_SERVIDOR_ATRAS)
		return opcionActual + 1;
	return opcionActual;
}

bool PantallaCliente::menuConfiguracionServidorTeclaEnter(int opcionActual) {
	if(opcionActual == OPCION_CONFIGURACION_SERVIDOR_IP) {
		this->pantallaConfiguracionHost();
		return false;
	}

	if (opcionActual == OPCION_CONFIGURACION_SERVIDOR_PUERTO) {
		this->pantallaConfiguracionPuerto();
	}

	if(opcionActual == OPCION_CONFIGURACION_SERVIDOR_ATRAS)
		return true;

	return false;
}







//------------------------------------------------------------------*
//	Validaciones de datos
//------------------------------------------------------------------*

bool PantallaCliente::enteroPositivo(string numero) {
	if (numero.empty() || stoi(numero) <= 0 )
		return false;
	return true;
}

bool PantallaCliente::nombreValido() {
	ConfigFile* c = new ConfigFile("config.conf");
	c->Load();
	string nombre = c->GetSetting("Client", "NAME")->AsString;
	delete c;

	if(! nombre.compare(DEFAULT_NAME))
		return false;
	return true;
}



/*

int main() {
	SDL_Surface *pantallaActual;
	if( SDL_Init(SDL_INIT_VIDEO) != 0 ) {
		cout << SDL_GetError() << endl;
		exit(1);
	}

	pantallaActual = SDL_SetVideoMode(PantallaCliente::RESOLUCION_X, PantallaCliente::RESOLUCION_Y, 24, SDL_DOUBLEBUF);
	if(pantallaActual == NULL) {
		cout << SDL_GetError() << endl;
		exit(1);
	}	

	PantallaCliente pantalla(pantallaActual);
	pantalla.pantallaDePresentacion();

	SDL_FreeSurface(pantallaActual);


	SDL_Quit();
	return 0;
}
*/