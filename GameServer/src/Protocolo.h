#ifndef PROTOCOLO_H
#define PROTOCOLO_H


#include "Json.h"

using namespace std;
using namespace json;

class TipoMensaje{
public:
	static int getTipoMensaje(string pjson); 
};


/******************************************************
* Mensajes del cliente al server
*******************************************************/
enum tipoMensajeCliente{
	desconexion = 1,
	envioAccionTecla = 2,
	envioNombreJugador = 3
};

/******************************************************
* Desconexion
*******************************************************/

class Desconexion{
public:
	const static int idTipoMensaje = desconexion;
	string serialize();
};


/******************************************************
* Envio Accion Tecla
*******************************************************/
enum accionTecla {
	arriba = 1,
	abajo = 2,
	derecha = 3,
	izquierda = 4,
	reparar = 5
};

class AccionTecla {
public:
	const static int idTipoMensaje = envioAccionTecla;
	int teclaPresionada;
	string serialize();
	static AccionTecla deserialize(string);
	AccionTecla(int tecla) { teclaPresionada = tecla; }
	AccionTecla() { }
};


/**********************************************************
* EnvioNombreJugador
***********************************************************/
class EnvioNombreJugador {
public: 
	const static int idTipoMensaje = envioNombreJugador;
	string nombreJugador;
	string serialize();
	static EnvioNombreJugador deserialize(string);
};


/**********************************************************
* Mensajes del server al cliente
***********************************************************/

enum tipoMensajeServer {
	posicionJugadores = 1,
	aparicionObjetoDibujable = 2,
	envioNumeroJugador = 3,
	jugadorPierdeVida = 4,
	//jugadorDesconectado = 5,
	envioConfiguraciones = 6 , // ponerse de acuerdo cuales son todas las configuraciones
	envioMapaCompleto = 7,
	jugadorInvulnerable = 8,
	envioFinPiso = 9,
	envioFinNivel = 10,
	movimientoRalph = 11,
	error = 12


};

/*****************************************************
* PosicionJugadores
******************************************************/

class PosicionJugadores {
public:
	const static int idTipoMensaje = posicionJugadores;
	int jugador1X;
	int jugador1Y;
	int jugador2X;
	int jugador2Y;
	//PosicionJugadores() { }
	//posicionJugadores(int j1x,int j1y,int j2x,int j2y) { jugador1X = j1x; jugador1Y = j1y, jugador2X = j2x; jugador2Y = j2y;}
	string serialize();
	static PosicionJugadores deserialize(string);
};


/******************************************************
* Envio Aparicion Objeto Dibujable
*******************************************************/

enum tipoAparicionObjetoDibujable {
	gaviota = 1,
	ladrillo = 2,
};


class AparicionObjetoDibujable {
public:
	const static int idTipoMensaje = aparicionObjetoDibujable;
	int tipoObjeto;
	int xInicio;
	int yInicio; 
	int xFin;
	int yFin;
	string serialize();
	static AparicionObjetoDibujable deserialize(string);
	AparicionObjetoDibujable(int , int , int ,int ,int );
};


/******************************************************
* Envio Jugador Pierde Vida
*******************************************************/

class JugadorPierdeVida {
public:
	const static int idTipoMensaje = jugadorPierdeVida;
	int numeroJugador;
	string serialize();
	static JugadorPierdeVida deserialize(string);
	JugadorPierdeVida(int numeroJugador) { this->numeroJugador = numeroJugador;}
};



/******************************************************
* Envio Jugador Invulnerable
*******************************************************/

class JugadorInvulnerable {
public:
	const static int idTipoMensaje = jugadorInvulnerable;
	int numeroJugador;
	string serialize();
	static JugadorInvulnerable deserialize(string);
	JugadorInvulnerable(int numeroJugador) { this->numeroJugador = numeroJugador;}
};


/******************************************************
* Envio Fin Piso
*******************************************************/

class FinPiso {
public:
	const static int idTipoMensaje = envioFinPiso;
	string serialize();
	static FinPiso deserialize(string);
	FinPiso() {}
};



/******************************************************
* Envio Fin Nivel
*******************************************************/


class FinNivel {
public:
	const static int idTipoMensaje = envioFinNivel;
	string serialize();
	static FinNivel deserialize(string);
	FinNivel() {}
};


/******************************************************
* Envio Movimiento Ralph
*******************************************************/

class MovimientoRalph {
public:
	const static int idTipoMensaje = movimientoRalph;
	int x, y;
	string serialize();
	static MovimientoRalph deserialize(string);
	MovimientoRalph(int x, int y) { this->x = x; this->y = y; };
};


/******************************************************
* Envio Error
*******************************************************/
enum codigoError {
	tiempoTorneoAgotado = 1
};

class Error {
public:
	const static int idTipoMensaje = error;
	int codigo;
	string msj;
	string serialize();
	static Error deserialize(string);
	Error(int codigo, string msj);	
	Error() ;
};
#endif
