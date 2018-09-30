#include "MsgFields.h"

string MsgFields::Serialize()
{
	return json::Serialize(obj);
}

void MsgFields::Deserialize(string pjson)
{
	Object temp = json::Deserialize(pjson);
	this->idTipoMensaje(temp["idTipoMensaje"]);

	switch(this->idTipoMensaje())
	{
		case PERDER_VIDA:
			this->tipoObjeto(temp["tipoObjeto"]);
			break;
		case INSCRIPCION:
			this->nombreJugador(temp["nombreJugador"]);
			break;
		case VENTANA_REPARADA:
			this->fila(temp["fila"]);
			this->columna(temp["columna"]);
			this->puntosTotales(temp["puntosTotales"]);
			this->tipoObjeto(temp["tipoObjeto"]);
			break;

		case TECLAS:
			this->teclaPresionada(temp["teclaPresionada"]);
			break;
		case MOVIMIENTO_OBJETO:
			this->tipoObjeto(temp["tipoObjeto"]);
			this->columna(temp["columna"]);
			this->fila(temp["fila"]);
			break;
		case CONFIGURACIONES:
			this->cantidadVidas(temp["cantidadVidas"]);
			this->tiempoPartida(temp["tiempoPartida"]);
			this->tiempoInmunidad(temp["tiempoInmunidad"]);
			this->numeroJugador(temp["numeroJugador"]);
			this->nombreJugador(temp["nombreJugador"]);
			break;
		case EDIFICIO:
			this->strEdificio(temp["strEdificio"]);
			break;
		case TORNEO_FINALIZADO:
			this->numeroJugador(temp["numeroJugador"]);
			break;
		case RESULTADO:
			this->nombreJugador(temp["nombreJugador"]);
			this->puntosTotales(temp["puntosTotales"]);
			break;
	}
}

/*
int main ()
{

	MsgFields lala;
	lala.idTipoMensaje(5);
	lala.numeroJugador(1);
	lala.tiempoInmunidad(5);
	lala.tiempoPartida(4);
	lala.cantidadVidas(3);
	string miJson = lala.Serialize();
	cout<<miJson<<endl;

	MsgFields lolo;
	lolo.Deserialize(miJson);
	
	cout<<lolo.Serialize()<<endl;	

	return 0;
}
*/