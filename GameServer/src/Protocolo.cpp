#include "Protocolo.h"
#include <iostream>

int TipoMensaje::getTipoMensaje(string pjson)
{ 
	Object obj;
	obj = json::Deserialize(pjson);
	return obj["idTipoMensaje"];
}

string Desconexion::serialize()
{
	Object obj;
	obj["idTipoMensaje"] = this->idTipoMensaje;
	return json::Serialize(obj);
}


string AccionTecla::serialize()
{
	Object obj;
	obj["idTipoMensaje"] = idTipoMensaje;
	obj["teclaPresionada"] = teclaPresionada;
	return json::Serialize(obj);
}

AccionTecla AccionTecla::deserialize(string pjson)
{
	Object obj = json::Deserialize(pjson);
	AccionTecla tecla(obj["teclaPresionada"]);
	return tecla;
}



string EnvioNombreJugador::serialize()
{
	Object obj;
	obj["nombreJugador"] = nombreJugador;
	obj["idTipoMensaje"] = idTipoMensaje;
	return json::Serialize(obj);
}

EnvioNombreJugador EnvioNombreJugador::deserialize(string pjson)
{
	Object obj = json::Deserialize(pjson);
	EnvioNombreJugador oRetorno;
	oRetorno.nombreJugador = (string)obj["nombreJugador"];
	return oRetorno;
}

string PosicionJugadores::serialize()
{
	Object obj;
	obj["idTipoMensaje"] = idTipoMensaje;
	obj["jugador1Y"] = jugador1Y;
	obj["jugador1X"] = jugador1X;
	obj["jugador2Y"] = jugador2Y;
	obj["jugador2X"] = jugador2X;
	return json::Serialize(obj);

}

PosicionJugadores PosicionJugadores::deserialize(string pjson)
{
	Object obj = json::Deserialize(pjson);
	PosicionJugadores oRetorno;
	oRetorno.jugador2X = obj["jugador2X"];
	oRetorno.jugador1X = obj["jugador1X"];
	oRetorno.jugador2Y = obj["jugador2Y"];
	oRetorno.jugador1Y = obj["jugador1Y"];
	return oRetorno;
}


AparicionObjetoDibujable::AparicionObjetoDibujable(int tipo, int pxInicio, int pyInicio, int pxFin, int pyFin){
	this->tipoObjeto = tipo;
	this->xInicio = pxInicio;
	this->yInicio = pyInicio;
	this->xFin = pxFin;
	this->yFin = pyFin;
}

string AparicionObjetoDibujable::serialize()
{
	Object obj;
	obj["idTipoMensaje"] = this->idTipoMensaje;
	obj["tipoObjeto"] = this->tipoObjeto;
	obj["xInicio"] = this->xInicio;
	obj["yInicio"] = this->yInicio;
	obj["xFin"] = this->xFin;
	obj["yFin"] = this->yFin;
	return json::Serialize(obj);
}

AparicionObjetoDibujable AparicionObjetoDibujable::deserialize(string pjson)
{
	Object obj = json::Deserialize(pjson);
	AparicionObjetoDibujable oReturn(obj["tipoObjeto"], obj["xInicio"], obj["yInicio"], obj["xFin"], obj["yFin"] );
	return oReturn;
}


string JugadorPierdeVida::serialize(){
	Object obj;
	obj["idTipoMensaje"] = this->idTipoMensaje;
	obj["numeroJugador"] = this->numeroJugador;
	return json::Serialize(obj);
}

JugadorPierdeVida JugadorPierdeVida::deserialize(string pjson) {
	Object obj = json::Deserialize(pjson);
	JugadorPierdeVida oReturn(obj["numeroJugador"]);
	return oReturn;
}


string JugadorInvulnerable::serialize(){
	Object obj;
	obj["idTipoMensaje"] = this->idTipoMensaje;
	obj["numeroJugador"] = this->numeroJugador;
	return json::Serialize(obj);
}

JugadorInvulnerable JugadorInvulnerable::deserialize(string pjson) {
	Object obj = json::Deserialize(pjson);
	JugadorInvulnerable oReturn(obj["numeroJugador"]);
	return oReturn;
}


string FinPiso::serialize() {
	Object obj;
	obj["idTipoMensaje"] = this->idTipoMensaje;
	return json::Serialize(obj);
}

FinPiso FinPiso::deserialize(string pjson) {
	FinPiso oReturn;
	return oReturn;
}


string FinNivel::serialize() {
	Object obj;
	obj["idTipoMensaje"] = this->idTipoMensaje;
	return json::Serialize(obj);
}

FinNivel FinNivel::deserialize(string pjson) {
	FinNivel oReturn;
	return oReturn;
}



string MovimientoRalph::serialize() {
	Object obj;
	obj["idTipoMensaje"] = this->idTipoMensaje;
	obj["x"] = this->x;
	obj["y"] = this->y;
	return json::Serialize(obj);
}

MovimientoRalph MovimientoRalph::deserialize(string pjson) {
	cout<<pjson<<endl;
	Object obj = json::Deserialize(pjson);
	cout<<2<<endl;
	MovimientoRalph oReturn(obj["x"], obj["y"]);
	return oReturn;
}


string Error::serialize() {
	 Object obj;
	 obj["idTipoMensaje"] = this->idTipoMensaje;
	 obj["codigo"] = this->codigo;
	 obj["msj"] = this->msj;
	 return json::Serialize(obj);
}

Error Error::deserialize(string pjson) {
	Object obj = json::Deserialize(pjson);
	Error oReturn(obj["codigo"],obj["msj"]);
	return oReturn;
}

Error::Error(int codigo, string mjs)
{
	this->codigo = codigo;
	this->msj = mjs;
}
Error::Error()
{}
