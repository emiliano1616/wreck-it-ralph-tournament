/*
 *   C++ sockets on Unix and Windows
 *   Copyright (C) 2002
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "Socket.h"  // For Socket and SocketException
#include <iostream>           // For cerr and cout
#include <cstdlib>            // For atoi()
#include <string>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <thread>
#include <list>
#include <mutex>
#include <SDL/SDL.h>
#include <vector>
#include "ConfigFile.h"
#include "PantallaCliente.h"
#include "MsgFields.h"
#include "ClientSettings.h"
#include "ObjetoDibujable.h"
#include "Utility.h"
#include "Ventana.h"
#include "Timer.h"

#define FIN 0
#define TIME 1
#define MSJ 2
#define INI 3
#define DIP 4
#define LOG 5
#define CLOSESERVER 6
#define BUFFTAM 1024

#define XOFFSET 230
#define YOFFSET 435
#define CELDAX 70
#define CELDAY 105
#define MAXROWS 12
#define VENTANASPORPISO 4
SDL_Surface * Screen ;
using namespace std;



class Client {
public:
  void manageEventsThread();
  void sendMessageThread();
  void receiveMessageThread();
  void processReceivedMessageThread(string);
  void drawThread();
  TCPSocket sock;
  Client();
  void addMessage(string);
  string removeMessage();

  list<string> messagesList;
  mutex mutexList;
  ClientSettings settings;
  Felix * playerLocal;
  Felix * playerOpponent;
  Ralph * ralph;
  list<ObjetoDibujable*> listaObjetos;

  mutex mutexSendMessage;
  mutex mutexCambioPiso;
  mutex mutexCambioPartida;
  ~Client();
  bool mustExit;

  void mostrarPantallaEspera();

  void subirPiso();

  void loadWindows(string);

  void drawWindows();

  void sendPlayThread();
  void  loadSettings();
  mutex mutexDibujando;
  bool enPartida;
  bool enPlayThread;
  bool recibiConfiguraciones;

private:
  mutex mutexListaObjetos;
  void timerThread();
  SDL_Rect rectFondo;
  SDL_Surface * fondo;
  SDL_Surface * pantallaEspera;
  int getXByColumn(int column);
  int getYByRow(int row);
  int getRowByY(int Y);
  int getColumnByX(int X);
  vector<vector<Ventana > > Ventanas;
  mutex mutexCreacionEdificio;
  mutex mutexEnvioJugar;
  int pisoOffset;
  Timer timer;
  void borrarRectangulos(ObjetoDibujable *);
  void dibujarRectangulosBorrados();
  int nivel;
  void manejarDesconexionTorneo();
  void manejarDesconexionPartida();
  void reiniciarPartida();
  list<SDL_Rect*> rectangulosBorrados;
  bool desconectandoServer;
  bool esFinTorneo;
  list<pair<string,int>> paresResultados ; 
  int indexResultado;
  void mostrarResultadoFinal();

};

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

void Client::mostrarResultadoFinal()
{
  SDL_FillRect(Screen,NULL,0x000000);

  SDL_Rect r;
  r.y = 100;
  int i = 0;
  for (std::list<pair<string,int>>::iterator it=paresResultados.begin() ; it!=paresResultados.end(); it++,i++)
  {
    TTF ttf(25,300,100 + (50 * i),255,0,0);
    SDL_BlitSurface(ttf.getSurface((*it).first + " " + to_string((*it).second)),NULL,Screen,&ttf.rectangulo);

  }
  SDL_Flip(Screen);


  sleep(5);

  this->mustExit = true;
  this->enPartida = false;
  mutexSendMessage.unlock();  
  mutexDibujando.unlock();
  mutexSendMessage.unlock();  
  mutexDibujando.unlock();
}


void Client::manejarDesconexionTorneo()
{
  if(esFinTorneo == true) return;
  this->mustExit = true;
  this->enPartida = false;

  mutexDibujando.lock();
  if(desconectandoServer)
  {
    mutexSendMessage.unlock();  
    mutexDibujando.unlock();
    return;
  }
  desconectandoServer = true;

  TTF mensajeDesconexion(40,0,0,255,0,0);
  SDL_Surface * imagen =  mensajeDesconexion.getSurface("Se ha perdido la conexion con el servidor");
  mensajeDesconexion.rectangulo.h = imagen->h;
  mensajeDesconexion.rectangulo.w = imagen->w;

  mensajeDesconexion.rectangulo.x = 400 - (imagen->w / 2);
  mensajeDesconexion.rectangulo.y = 300 - (imagen->h / 2);

  for(int i = 0; i < 4 ; i ++)
  {
    cout<<"i: " << i<<endl;
    SDL_FillRect(Screen,&mensajeDesconexion.rectangulo,0x000000);
    SDL_Flip(Screen);
    sleep(1);

    SDL_BlitSurface(imagen,NULL,Screen,&mensajeDesconexion.rectangulo);
    SDL_Flip(Screen);
    sleep(1);    
  }
  sock.CloseConnection();
  mutexSendMessage.unlock();  
  mutexDibujando.unlock();

}

void Client::manejarDesconexionPartida()
{
  if(esFinTorneo == true) return;
  cout<<"Voy a manejar la desconexion de la partida"<<endl;
  this->enPartida = false;
  mutexDibujando.lock();
  recibiConfiguraciones = false;
  /*
  if(!enPlayThread)
  {
    thread threadPlay(&Client::sendPlayThread,this);
    threadPlay.detach();  
  }*/
  
  TTF mensajeDesconexion(40,0,0,255,0,0);
  SDL_Surface * imagen =  mensajeDesconexion.getSurface("Se ha perdido la conexion con la partida");
  mensajeDesconexion.rectangulo.h = imagen->h;
  mensajeDesconexion.rectangulo.w = imagen->w;

  mensajeDesconexion.rectangulo.x = 400 - (imagen->w / 2);
  mensajeDesconexion.rectangulo.y = 300 - (imagen->h / 2);

  for(int i = 0; i < 4 ; i ++)
  {
  cout<<"i: " << i<<endl;
    SDL_FillRect(Screen,&mensajeDesconexion.rectangulo,0x000000);
    SDL_Flip(Screen);
    sleep(1);

    SDL_BlitSurface(imagen,NULL,Screen,&mensajeDesconexion.rectangulo);
    SDL_Flip(Screen);
    sleep(1);    
  }

  mutexDibujando.unlock();

}

void Client::borrarRectangulos(ObjetoDibujable * obj)
{

  if(obj)
  {
    if(obj->seMovio())
    {
      SDL_BlitSurface(this->fondo,&obj->ultimoRectanguloDibujadoConOffset,Screen,&obj->ultimoRectanguloDibujadoSinOffset);
      rectangulosBorrados.push_back(&obj->ultimoRectanguloDibujadoSinOffset);
    }

  }
}

void Client::mostrarPantallaEspera()
{
  SDL_FillRect(Screen,NULL,0x000000);
  SDL_BlitSurface(pantallaEspera,NULL,Screen,NULL);
  SDL_Flip(Screen);
}

void Client::sendPlayThread()
{
  enPlayThread = true;
  MsgFields temp;
  temp.idTipoMensaje(JUGAR);
  //mutexEnvioJugar.lock();
  //mutexEnvioJugar.lock();
  //mutexEnvioJugar.lock();
  //mutexEnvioJugar.unlock();
  addMessage(temp.Serialize());
  enPlayThread = false;
}


void Client::loadWindows(string cad)
{

  mutexCreacionEdificio.lock();
	vector<string> vec = split(cad,',');
	unsigned int i = 0;
	Ventanas.resize(15);
	for(int j = 0; j < 15; j++)
		Ventanas[j].resize(5);

  cout<<"loadWindows"<<endl;
	while(i < vec.size())
	{
		//cout<<"FILA: "<< vec[i]<<" COLUMNA: "<<vec[i+1]<<" ESTADO: "<<vec[i+2]<<endl;
		//string temp = vec[i];
		Ventana temp(stoi(vec[i+2]),getXByColumn( stoi(vec[i +1 ]) ), getYByRow( stoi(vec[i])));

		Ventanas[stoi(vec[i])][stoi(vec[i+1])] = temp;
		//Ventanas[vec[i]][vec[i+1]] = (int)vec[i + 2];
		i += 3;
	}
  mutexCreacionEdificio.unlock();

}

int Client::getXByColumn(int column) 
{
	return XOFFSET + (CELDAX * column);
}


int Client::getYByRow(int row) 
{
	//cout<<YOFFSET<<" + (("<<MAXROWS<<" - "<<row<<") * "<<CELDAY<<") - "<<pisoOffset<< " = " <<YOFFSET + ((MAXROWS - row) * CELDAY) - pisoOffset<<endl;
	return YOFFSET + ((MAXROWS - row) * CELDAY) - pisoOffset;
}

int Client::getRowByY(int Y) 
{
	return MAXROWS - (( Y + pisoOffset - YOFFSET) / CELDAY);
}

int Client::getColumnByX(int X) 
{
	return (X - XOFFSET) / CELDAX;
}



Client::Client()
{
  mustExit = false;
  enPartida = false;
  enPlayThread = false;
  esFinTorneo = false;
  recibiConfiguraciones = false;
  this->fondo = IMG_Load((Utility::GetPathBIN() + "images/edificio.png").c_str());
  if(! this->fondo)
  {
    puts(SDL_GetError());
    exit(1);
  }

  this->pantallaEspera = IMG_Load(((Utility::GetPathBIN() + "images/PantallaEspera.png").c_str()));
  if(! this->fondo)
  {
    puts(SDL_GetError());
    exit(1);
  }

  desconectandoServer = false;
}

Client::~Client()
{
  if(playerLocal)
    delete playerLocal;
  if(playerOpponent)
    delete playerOpponent;
  if(ralph)
    delete ralph;
}

void Client::loadSettings()
{
    ConfigFile* c = new ConfigFile("config.conf");
    c->Load();
    this->settings.port =stoi(c->GetSetting("Client", "PORT")->AsString.c_str());
    this->settings.ipAddress =c->GetSetting("Client", "IP")->AsString;
    this->settings.name =c->GetSetting("Client", "NAME")->AsString;

    this->settings.keyUp =stoi(c->GetSetting("Client", "KEYUP")->AsString.c_str());
    this->settings.keyDown =stoi(c->GetSetting("Client", "KEYDOWN")->AsString.c_str());
    this->settings.keyRight =stoi(c->GetSetting("Client", "KEYRIGHT")->AsString.c_str());
    this->settings.keyLeft =stoi(c->GetSetting("Client", "KEYLEFT")->AsString.c_str());
    this->settings.keyRepair =stoi(c->GetSetting("Client", "KEYREPAIR")->AsString.c_str());
    delete c;

}

void Client::addMessage(string pMessage)
{
  mutexList.lock();
  messagesList.push_back(pMessage);
  mutexList.unlock();
  mutexSendMessage.unlock();
}

string Client::removeMessage()
{

  string temp = messagesList.front();
  mutexList.lock();
  messagesList.pop_front();
  mutexList.unlock();
  return temp;
}

void Client::subirPiso()
{
  this->mutexCambioPiso.lock();
  int offsetDestino = this->rectFondo.y - (VENTANASPORPISO * CELDAY);
  nivel ++;
  while(offsetDestino < this->rectFondo.y && enPartida && !mustExit)
  {
    this->rectFondo.y-=5;
    SDL_BlitSurface(this->fondo,&this->rectFondo,Screen,NULL);
    SDL_Flip(Screen);
  }
  pisoOffset = offsetDestino;

  for(int i = 0 ; i < 13 ; i++)
    for(int j = 0 ; j < 5 ; j++)
    {
      Ventanas[i][j].rectangulo.y = getYByRow(i);
    }

  if(playerLocal->numeroJugador == 1)
  {
    playerLocal->mover(getXByColumn(0),getYByRow(nivel*4));
    playerOpponent->mover(getXByColumn(4),getYByRow(nivel*4));
  }
  else
  {
    playerOpponent->mover(getXByColumn(0),getYByRow(nivel*4));
    playerLocal->mover(getXByColumn(4),getYByRow(nivel*4)); 
  }

  drawWindows();
  dibujarRectangulosBorrados();

  this->mutexCambioPiso.unlock();

}

void Client::manageEventsThread()
{
  SDL_Event event;
  MsgFields campos;
  campos.idTipoMensaje(TECLAS);

  int Fila =4;
  int Columna =2;

  while( this->mustExit == false)
  {

    //While the user hasn't quit
    while( this->enPartida )
    {
      usleep(40 * 1000);
        //If there's an event to handle
        if( SDL_PollEvent( &event ) )
        {
            //If a key was pressed
            if( event.type == SDL_KEYDOWN && playerLocal->vidasRestantes != 0)
            {
                int key = (int)event.key.keysym.sym;
                if(key == this->settings.keyUp)
                {
                  campos.teclaPresionada(ARRIBA);
                  this->addMessage(campos.Serialize());
                }
                else if(key == this->settings.keyDown)
                {
                  campos.teclaPresionada(ABAJO);
                  this->addMessage(campos.Serialize());
                }
                else if(key == this->settings.keyLeft)
                {
                  campos.teclaPresionada(IZQUIERDA);
                  this->addMessage(campos.Serialize());
                  Columna--;
                }
                else if(key == this->settings.keyRight)
                {
                  campos.teclaPresionada(DERECHA);
                  this->addMessage(campos.Serialize());
                  Columna++;
                }
                else if(key == this->settings.keyRepair)
                {
                  campos.teclaPresionada(REPARAR);
                  this->addMessage(campos.Serialize());
                  //Ventanas[playerLocal->fila][playerLocal->columna].repararVentana();
                  //playerLocal->sumarPuntos(campos.puntosTotales());
                }
                else if(key == 27) //escape
                {
                  this->mustExit = true;
                  enPartida = false;
                  this->mutexSendMessage.unlock();
                }
                else if(key == 286) //F5
                {
                  subirPiso();
                }
                //ralph->mover(getXByColumn(Columna),getYByRow(Fila));
            }

            //If the user has Xed out the window
            else if( event.type == SDL_QUIT )
            {
                //Quit the program
                this->mustExit = true;
                enPartida = false;
                this->mutexSendMessage.unlock();
            }
        }
    }
    sleep(1);
  }
}


void Client::processReceivedMessageThread(string buff)
{
  cout<<"Mensaje recibido: " << buff<<endl;
  MsgFields campos;
  campos.Deserialize(buff);

  switch(campos.idTipoMensaje())
  {
    case CONFIGURACIONES:
    {
      //mutexCambioPartida.lock();

      rectFondo.h = 600;
      rectFondo.w = 800;
      rectFondo.y = fondo->h - 600; 
      rectFondo.x = 0;
      nivel = 0;
      pisoOffset = rectFondo.y;

      settings.inmunityTime = campos.tiempoInmunidad();
      settings.lifeAmount = campos.cantidadVidas();
      settings.gameTime = campos.tiempoPartida();
      this->timer.secondsLeft = campos.tiempoPartida() ;
      int n = campos.numeroJugador();
      if(!playerLocal)
        playerLocal = new Felix(getXByColumn(n == 1 ? 4 : 0),getYByRow(0),n == 1? 2 :1, settings.name);
      else
        playerLocal->reiniciar(getXByColumn(n == 1 ? 4 : 0),getYByRow(0),n == 1? 2 :1, settings.name);

      if(!playerOpponent)
        playerOpponent = new Felix(getXByColumn(n == 1 ? 0 : 4),getYByRow(0),n == 1? 1:2, campos.nombreJugador());
      else
        playerOpponent->reiniciar(getXByColumn(n == 1 ? 0 : 4),getYByRow(0),n == 1? 1:2, campos.nombreJugador());

      if(!ralph)
        ralph = new Ralph(getXByColumn(2),getYByRow(4));
      else
        ralph->reiniciar(getXByColumn(2),getYByRow(4));

      playerLocal->vidasRestantes = playerOpponent->vidasRestantes = settings.lifeAmount;

      recibiConfiguraciones =true;
      //mutexEnvioJugar.unlock();

      //mutexCambioPartida.unlock();

      break;      
    }

    case JUGAR:
    {

      enPartida = true;
      thread t(&Client::drawThread,this);
      t.detach();

      if(timer.isTiming)
      {
        timer.secondsLeft = settings.gameTime;
      }
      else
      {
        thread t2(&Timer::startTiming,&this->timer);
        t2.detach();  
      }
      break;
    }

    case VENTANA_REPARADA:
    {
      Ventanas[campos.fila()][campos.columna()].repararVentana();
      
      if(campos.tipoObjeto() == JUGADOR1)
      {
        if(playerLocal->numeroJugador == 1)
        {
          playerLocal->sumarVentanaReparada(settings.inmunityTime);
          playerLocal->sumarPuntos(campos.puntosTotales());
        }
        else
        {
          playerOpponent->sumarVentanaReparada(settings.inmunityTime);
          playerOpponent->sumarPuntos(campos.puntosTotales());
        }
      }
      else
      {
        if(playerLocal->numeroJugador == 2)
        {
          playerLocal->sumarVentanaReparada(settings.inmunityTime);
          playerLocal->sumarPuntos(campos.puntosTotales());
        }
        else
        {
          playerOpponent->sumarVentanaReparada(settings.inmunityTime);
          playerOpponent->sumarPuntos(campos.puntosTotales());
        }
      }
      
      break;
    }

    case TORNEO_FINALIZADO:
    {
        esFinTorneo = true;
        indexResultado = campos.numeroJugador();
        break;
    }

    case RESULTADO:
    {
      pair<string,int> par = make_pair(campos.nombreJugador(),campos.puntosTotales());  
      paresResultados.push_back(par);
      indexResultado--;
      if(indexResultado == 0)
      {
        mostrarResultadoFinal();
      }
      break;
    }


    case MOVIMIENTO_OBJETO:
    {
    	switch(campos.tipoObjeto())
    	{
    		case GAVIOTA:
    		{
      		int columnaInicio = campos.columna() == 0 ? 0 : 700;
      		int columaFin = columnaInicio == 0 ? 700 : 0;
      		Gaviota * dibujo;
          dibujo = new Gaviota(columnaInicio ,this->getYByRow(campos.fila()), columaFin, this->getYByRow(campos.fila()));
      		dibujo->setLevelDificulty(nivel);
          dibujo->mover();
          mutexListaObjetos.lock();
      		this->listaObjetos.push_back(dibujo);
          mutexListaObjetos.unlock();

      		break;
    		}
        case LADRILLO:
        {

	    		Ladrillo * dibujo;
          dibujo = new Ladrillo(this->getXByColumn(campos.columna()),getYByRow(campos.fila()),this->getXByColumn(campos.columna()), 500);
          dibujo->setLevelDificulty(nivel);
	    		dibujo->mover();
      		mutexListaObjetos.lock();
          this->listaObjetos.push_back(dibujo);
          mutexListaObjetos.unlock();
	    		break;
        }     
      	case JUGADOR1:
    		{
      		if(this->playerLocal->numeroJugador == 1)
          {
            this->playerLocal->fila = campos.fila();
            this->playerLocal->columna = campos.columna();
      			this->playerLocal->mover(getXByColumn( this->playerLocal->columna),getYByRow( this->playerLocal->fila));
          }
          else
          {
            this->playerOpponent->fila = campos.fila();
            this->playerOpponent->columna = campos.columna();
            this->playerOpponent->mover(getXByColumn( this->playerOpponent->columna),getYByRow( this->playerOpponent->fila));
          }
          break;
    		}
      	case JUGADOR2:
      	{
      		if(this->playerLocal->numeroJugador == 2)
          {
            this->playerLocal->fila = campos.fila();
            this->playerLocal->columna = campos.columna();
            this->playerLocal->mover(getXByColumn( this->playerLocal->columna),getYByRow( this->playerLocal->fila));
          }
          else
          {
            this->playerOpponent->fila = campos.fila();
            this->playerOpponent->columna = campos.columna();
            this->playerOpponent->mover(getXByColumn( this->playerOpponent->columna),getYByRow( this->playerOpponent->fila));
          }
          break;
      	}

        case RALPH:
        {
          ralph->mover(getXByColumn(campos.columna()),getYByRow(campos.fila()));
        }

    	}
    	break;
    }

    case PARTIDA_FINALIZADA:
    {
      this->reiniciarPartida();

      break;
    }

    case PERDER_VIDA:
    {
      
      if(campos.tipoObjeto() == JUGADOR1)
      {
        if(playerLocal->numeroJugador == 1)
        {
          //playerLocal->perderVida(getXByColumn(0),getYByRow((nivel - 1)*4));
        }
        else 
        {
          playerOpponent->perderVida();
        }
      }

      if(campos.tipoObjeto() == JUGADOR2)
      {
        if(playerLocal->numeroJugador == 2)
        {
          //playerLocal->perderVida(getXByColumn(4),getYByRow((nivel - 1)*4));
        }
        else 
        {
          playerOpponent->perderVida();
        }
      }
      
      break;
    }

    case EX_KILL_GAMESERVER:
    {
      manejarDesconexionPartida();
      reiniciarPartida();
      break;
    }

    case EX_KILL_TOURNOMENT:
    {
      manejarDesconexionTorneo();
      break;
    }

    case FIN_PISO:
    {
      subirPiso();
      MsgFields subePiso;
      subePiso.idTipoMensaje(FIN_PISO);
      addMessage(subePiso.Serialize());
      break;
    }

    case EDIFICIO:
    {
      //mutexCambioPartida.lock();
      rectFondo.h = 600;
      rectFondo.w = 800;
      rectFondo.y = fondo->h - 600; 
      rectFondo.x = 0;
      nivel = 0;
      pisoOffset = rectFondo.y;

      loadWindows(campos.strEdificio());
      //while(recibiConfiguraciones == false)
      //{
        //usleep(50*100);
      //}
      cout<<"VOY A ENVIAR JUGAR "<<endl;
      MsgFields play;
      play.idTipoMensaje(JUGAR);
      addMessage(play.Serialize());
      cout<<"METIDO EN LA COLA"<<endl;
      //mutexEnvioJugar.unlock();
      //mutexCambioPartida.unlock();
      break;
    }
    case ERROR :
    {
      cout<<"Tipo mensaje, error"<<endl;
      break;
    }
    case ACK:
    {
      break;
    }
    default : cout<<"messageType unknow... messageType: " <<campos.idTipoMensaje()<<endl; break;


  }
}

void Client::reiniciarPartida()
{
  //mutexCambioPartida.lock();

  this->enPartida = false;

  timer.secondsLeft = 0;

  this->mostrarPantallaEspera();

  recibiConfiguraciones = false;
  /*
  if(!enPlayThread)
  {
    thread threadPlay(&Client::sendPlayThread,this);

    //mutexCambioPartida.unlock();
    threadPlay.join();  
  }
  */
  

  //mutexDibujando.lock();

  //En teoria... ya puedo afirmar que el draw thread termino... ponele...
  //for(list<ObjetoDibujable*>::iterator it = listaObjetos.begin(); it != listaObjetos.end();it++)
  //  delete *it;
  //listaObjetos.clear();

  //mutexDibujando.unlock();


}
void Client::receiveMessageThread()
{
  char buff[BUFFTAM];
  while(!this->mustExit)
  {
    int result = this->sock.recv(buff,BUFFTAM);
    if( result > 0)
    {
      string temp(buff);

      list<string> messages = Utility::SplitJSON(temp.substr(0,result ));
      for(list<string>::iterator itj = messages.begin(); itj != messages.end() ; itj++)
      {
        thread t(&Client::processReceivedMessageThread,this,string(*itj));
        t.detach();
      }

    }
    else if(result < 0)
    {
      cout<<"problemas al recibir...."<<endl;
      manejarDesconexionTorneo();
      return;
    }
    else 
    {
      cout<<"se perdio la conexion con el servidor...."<<endl;
      manejarDesconexionTorneo();
      return;
    }

  }
}

void Client::sendMessageThread()
{

  this->mutexSendMessage.lock();

  while(! this->mustExit)
  {
    this->mutexSendMessage.lock();
    if(! mustExit)
    {
      string temp = this->removeMessage();
      cout<<temp<<endl;
      this->sock.send(temp.c_str(),temp.size());
    }else
    {
      string temp = this->removeMessage();
      cout<<"NO ENVIE: "<<temp<<endl;

    }
  }
    this->mutexSendMessage.unlock();
}


void Client::drawWindows()
{
  mutexCreacionEdificio.lock();
    for(int i = 0; i <= 12 ; i++)
      for(int j = 0; j <= 4 ; j++)
        //if(Ventanas[i][j].cambioEstado())
        {
          rectangulosBorrados.push_back(&Ventanas[i][j].rectangulo);
          //Ventanas[i][j].dibujar(Screen);
        }

  mutexCreacionEdificio.unlock();
}
  
void Client::drawThread()
{

  MsgFields msgPerderVida;
  msgPerderVida.idTipoMensaje(PERDER_VIDA);
  msgPerderVida.tipoObjeto(playerLocal->numeroJugador == 1 ? JUGADOR1 : JUGADOR2);

  mutexDibujando.lock();
  cout<<"Inicio DrawThread"<<endl;
  list<ObjetoDibujable*>::iterator tempIT;
  SDL_FillRect(Screen,NULL,0x000000); 
  SDL_BlitSurface(this->fondo,&this->rectFondo,Screen,NULL);

  SDL_Rect temp;
  temp.x = 0; temp.y = 600; temp.h = 75; temp.w = 800;
  //SDL_FillRect(Screen,&temp,SDL_MapRGB(Screen->format, 119, 136, 156));
  bool hayCambio =true;

  this->drawWindows();  
  playerLocal->dibujar(Screen,pisoOffset);
  playerOpponent->dibujar(Screen,pisoOffset);
  ralph->dibujar(Screen,pisoOffset);


  while(this->enPartida && !this->mustExit)
  {

    this->mutexCambioPiso.lock();
    //SDL_FillRect(Screen,NULL,0x000000);


    //Dibujo Al jugador local
    borrarRectangulos(playerLocal);
    borrarRectangulos(playerOpponent);
    borrarRectangulos(ralph);

    mutexListaObjetos.lock();
    
    for (list<ObjetoDibujable*>::iterator it = this->listaObjetos.begin(); it != this->listaObjetos.end(); it++)
    {
      if(!(*it)) continue;
      if((*it)->ocupado)//Si esta ocupado, es porque esta dibujando
      {
        if((*it)->primerDibujo)
          (*it)->dibujar(Screen,pisoOffset);

        borrarRectangulos(*it); 
        if(! playerLocal->soyInvulnerable && ! playerLocal->perdiendoVida && ObjetoDibujable::rectangulosChocando((*it)->rectangulo,playerLocal->rectangulo))
        {
          playerLocal->perderVida();
          addMessage(msgPerderVida.Serialize());

        }

        //(*it)->dibujar(Screen,pisoOffset);

      }
      else
      {
        if((*it)->primerDesocupado)
        {
          (*it)->primerDesocupado = false;
          borrarRectangulos(*it); 
        }
        //delete *it;
        //it = this->listaObjetos.erase(it);
      }
    }
    mutexListaObjetos.unlock();

    if(playerLocal->nuevosPuntos() || playerOpponent->nuevosPuntos())
    {
      SDL_FillRect(Screen,&temp,0x000000);
      playerLocal->dibujarVidas(Screen);
      playerOpponent->dibujarVidas(Screen);
      if(playerLocal->vidasRestantes != 0)
      {
        this->timer.draw(Screen);
      }
      else
      {
        timer.drawGameOver(Screen);
      }
      hayCambio = true;
    }

    if(this->timer.needDraw() && playerLocal->vidasRestantes != 0)
    {
      this->timer.draw(Screen);
      hayCambio = true;
    }
    dibujarRectangulosBorrados();
    
    //cout<< "tamanio lista rectangulso borrados: "<< rectangulosBorrados.size()<<endl;
    if(rectangulosBorrados.size() > 0 || hayCambio)
      SDL_Flip(Screen);
    rectangulosBorrados.clear();

    this->mutexCambioPiso.unlock();

    usleep(40 * 1000);
    hayCambio = false;
  }

  mutexDibujando.unlock();
  cout<<"Termino DrawThread"<<endl;


}

void Client::dibujarRectangulosBorrados()
{
  for (list<SDL_Rect*>::iterator it = this->rectangulosBorrados.begin(); it != this->rectangulosBorrados.end(); it++)
    {
      if(!*it) continue;
      if(rectangulosBorrados.size() <= 65)
      {
        int fila = getRowByY((*it)->y);
        int col = getColumnByX((*it)->x);
        if(fila < 13 && fila >= 0 && col >= 0 && col < 5) 
          Ventanas[fila][col].dibujar(Screen);
      }

      for(int i = 0; i < 13; i++)
      {
        for(int j = 0 ; j < 5; j++)
        {
          if(ObjetoDibujable::rectangulosChocando(Ventanas[i][j].rectangulo,**it))
          {
            Ventanas[i][j].dibujar(Screen);
          }
        }
      }
      

      if(playerLocal->vidasRestantes != 0 && ObjetoDibujable::rectangulosChocando(playerLocal->rectangulo,**it))
      {
        playerLocal->dibujar(Screen,pisoOffset);
      }

      if(ObjetoDibujable::rectangulosChocando(playerOpponent->rectangulo,**it))
      {
        //cout<<"choco con oponente"<<endl;
        playerOpponent->dibujar(Screen,pisoOffset);
      }

      if(ObjetoDibujable::rectangulosChocando(ralph->rectangulo,**it))
      {
        //cout<<"choco con ralph"<<endl;
        ralph->dibujar(Screen,pisoOffset);
      }


      for (list<ObjetoDibujable*>::iterator it2 = this->listaObjetos.begin(); it2 != this->listaObjetos.end(); it2++)
      {
        if(!(*it2)) continue;
        if((*it2)->ocupado)//Si esta ocupado, es porque esta dibujando
        {
          if(ObjetoDibujable::rectangulosChocando((*it2)->rectangulo,**it))
            (*it2)->dibujar(Screen,pisoOffset);
        }
      }

    }
}


void initScreen()
{
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
  {
    puts(SDL_GetError());
    exit(1);
  }

  Screen = SDL_SetVideoMode(800, 675, 24, SDL_DOUBLEBUF);
  if(!Screen)
  {
    puts(SDL_GetError());
    exit(1);
  }

  atexit(SDL_Quit);
}

  Client client;

void sigintHandler(int sig)
{
  client.mustExit = true;
  client.enPartida = false;
  client.mutexSendMessage.unlock();
  cout<<"me tiraron sigint viteh"<<endl;
}

int main (int argc , char ** argv)
{
  
  initScreen();
  signal(SIGINT,sigintHandler);

  PantallaCliente pantalla(Screen);
  pantalla.pantallaDePresentacion();

  client.loadSettings();

  client.mostrarPantallaEspera();

  cout<<client.settings.ipAddress<<endl;

  try
  {
    client.sock.connect(client.settings.ipAddress,client.settings.port);
  }
    catch (SocketException &e)
  {
    cout<<"No se encuentra el servidor. Se cerrara el programa."<<endl;
    exit(1);
  }


  /*
  if(!client.enPlayThread)
  {
    thread threadPlay(&Client::sendPlayThread,&client);
    threadPlay.detach();  
  }
  */

  thread t1(&Client::sendMessageThread,&client);

  usleep(250 * 1000);

  MsgFields campos;
  campos.idTipoMensaje(INSCRIPCION);
  campos.nombreJugador(client.settings.name);
  cout<<"setting name:"<<client.settings.name<<endl;
  client.addMessage(campos.Serialize());


  thread t2(&Client::receiveMessageThread,&client);
  t2.detach();

  thread t3(&Client::manageEventsThread,&client);


  //string cadena = "0,0,1,0,1,0,0,2,3,0,3,4,0,4,0,1,0,0,1,1,1,1,2,3,1,3,3,1,4,2,2,0,2,2,1,0,2,2,1,2,3,4,2,4,0,3,0,1,3,1,0,3,2,3,3,3,4,3,4,0,4,0,0,4,1,1,4,2,3,4,3,3,4,4,2,5,0,2,5,1,0,5,2,1,5,3,4,5,4,0,6,0,0,6,1,1,6,2,2,6,3,3,6,4,4,7,0,0,7,1,1,7,2,2,7,3,3,7,4,4,8,0,0,8,1,1,8,2,2,8,3,3,8,4,4,9,0,0,9,1,1,9,2,2,9,3,3,9,4,4,10,0,0,10,1,1,10,2,2,10,3,3,10,4,4,11,0,0,11,1,1,11,2,2,11,3,3,11,4,4,12,0,0,12,1,1,12,2,2,12,3,3,12,4,4";
  //client.loadWindows(cadena);
 
  t1.join();
  t3.join();

  sleep(1);
  client.mutexDibujando.lock();
  client.mutexDibujando.unlock();

  SDL_FreeSurface(Screen);
  

  return 0;
  
}
