
#include "Json.h"
#include <iostream>

using namespace std;
using namespace json;

enum TipoMensaje{
    //Cliente al server
    DESCONEXION = 1,
    TECLAS = 2,
    INSCRIPCION = 3,

    //Server al Cliente
    MOVIMIENTO_OBJETO = 4,
    //jugadorPierdeVida = 4,
    //jugadorDesconectado = 5,
    CONFIGURACIONES = 5 , // ponerse de acuerdo cuales son todas las configuraciones
    EDIFICIO = 6,
    /*jugadorInvulnerable = 8,
    envioFinPiso = 9,
    envioFinNivel = 10,
    movimientoRalph = 11,*/    
    ERROR = 12,
    JUGAR = 13,
    POSICION_INICIAL = 14,
    VENTANA_REPARADA = 15,
    PARTIDA_FINALIZADA = 16,
    TORNEO_FINALIZADO = 17,
    PERDER_VIDA=18,
    FIN_PISO = 19,
    RESULTADO=20,

    EX_KILL_GAMESERVER =98, 
    EX_KILL_TOURNOMENT =99,
    ACK = 666



};

enum Teclas {
    ARRIBA = 1,
    ABAJO = 2,
    DERECHA = 3,
    IZQUIERDA = 4,
    REPARAR = 5
};


enum TipoObjeto {
    VENTANA_SANA = 0,
    VENTANA_ROTA1 = 1,
    VENTANA_ROTA2 = 2,
    VENTANA_ROTA3 = 3,
    VENTANA_CERRADA = 4,
    VENTANA_ABIERTA_IZQ = 5,
    VENTANA_ABIERTA_DER = 6,
    BALCON = 7,
    GAVIOTA = 8,
    LADRILLO = 9,
    JUGADOR1 = 10,
    JUGADOR2 = 11,
    RALPH = 12
    
   
};


class MsgFields {
private:
    int _idTipoMensaje;

    int _columna ; int _fila; //Para gaviota, ladrillos, jugadores o ralph
    int _tipoObjeto;
    int _teclaPresionada;
   
    int _cantidadVidas;
    int _tiempoPartida;
    int _tiempoInmunidad;
    int _numeroJugador; // oponente
    string _nombreJugador; //oponente

    int _puntosTotales;

    //Formato string, fila,columna,tipoObjeto... hay que hacerle split
    string _strEdificio;//Ver cuantos pixeles ocupa esta matriz, hay que parsearlo
    Object obj;
public:
    void idTipoMensaje(int pid) { _idTipoMensaje = pid; obj["idTipoMensaje"] = pid;}
    int idTipoMensaje() {return _idTipoMensaje;}

    void columna(int c) {_columna = c; obj["columna"] = c;}
    int columna() {return _columna;}
    void fila(int f) {_fila = f; obj["fila"] = f;}
    int fila() {return _fila;}
    void nombreJugador(string n) { _nombreJugador = n; obj["nombreJugador"] = n;}
    string nombreJugador() {return _nombreJugador;}
    int tipoObjeto() {return _tipoObjeto;}
    void tipoObjeto(int to) {_tipoObjeto = to; obj["tipoObjeto"] = to;}
    int teclaPresionada() {return _teclaPresionada;}
    void teclaPresionada(int tec) { _teclaPresionada = tec; obj["teclaPresionada"] = tec;}

    int cantidadVidas() {return _cantidadVidas;}
    void cantidadVidas(int cv) { _cantidadVidas = cv; obj["cantidadVidas"] = cv;}
    int tiempoPartida() {return _tiempoPartida;}
    void tiempoPartida(int tp) { _tiempoPartida = tp ; obj["tiempoPartida"] = tp;}
    int tiempoInmunidad () { return _tiempoInmunidad;}
    void tiempoInmunidad (int ti) { _tiempoInmunidad = ti ; obj["tiempoInmunidad"] = ti;}
    int numeroJugador() {return _numeroJugador;}
    void numeroJugador ( int nj) { _numeroJugador = nj ; obj["numeroJugador"] = nj;}

    string strEdificio() {return _strEdificio;}
    void strEdificio (string e) { _strEdificio = e; obj["strEdificio"] = e;}

    int puntosTotales() {return _puntosTotales;}
    void puntosTotales ( int pt) { _puntosTotales = pt ; obj["puntosTotales"] = pt;}


    string Serialize();
    void Deserialize(string);
};