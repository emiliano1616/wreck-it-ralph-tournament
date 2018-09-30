#include "TournomentServer.h"

//Constructor

TournomentServer::TournomentServer()
{
}

TournomentServer::TournomentServer(SDL_Surface * screen)
{	
	this->screen = screen;
	idClient=1000;
	idGame=5000;
	lstPlayers = new list<Player>();
    lstGames = new list<Game>();
    pidsGamesServer = new list<int>();
    //Semaforo utilizado para indicar la presencia de un jugador nuevo
    //Solo se usa despues de la primera vez que se arman partidas.
    
    semNotPlayer = new Semaphore("SemNotPlayer",true,0);    
    blnMakeGames=true;
	blnEndTournoment=false;
	resumeScreen = NULL;
}


//Funcion principal
void TournomentServer::MainFunction()
{
	cout<<"Main Function"<<endl;
	resumeScreen = new PantallaResumen(this->screen);

    this->ReadConfigFile();

    thread thPollEvents(&TournomentServer::ManageEvents,this);
    thPollEvents.detach();

	//Pantalla pantalla;
	//pantalla.SplashScreen();
	//sleep(1);	
	thread t1(&TournomentServer::ThreadCallAccept, this);
	t1.detach();

	thread tShow(&TournomentServer::ShowResults,this);
  	tShow.detach();


	thread t2(&TournomentServer::MakeGames,this);
  	t2.detach();

    thread thTimer(&TournomentServer::Timer,this);
    thTimer.join();

	//Creamos distintos threads
	// 1 para la parte grafica
	// 2 para la comunicacion
	// 3 para el armado de partida
	// long t=1;
}

void TournomentServer::Timer()
{
	int dd=0,hh=0,mm=0,ss=0;
	Utility::ConvertSecondToTime(tournomentTime,&dd,&hh,&mm,&ss);

	while(!blnEndTournoment && tournomentTime>0)
	{		
		//cout<<"dd:"<<dd<<" hh:"<<hh<<" mm:"<<mm<<" ss:"<<ss<<endl;
		tournomentTime--;
		Utility::ConvertSecondToTime(tournomentTime,&dd,&hh,&mm,&ss);
		//resumeScreen->actualizarTiempo(hh,mm,ss);
		if(resumeScreen != NULL)
			resumeScreen->actualizarTiempo(hh,mm,ss);
    	sleep(1);

    	CheckPlayerAlive();
    }
    //Al salir de este timer finalizamos el torneo
    blnEndTournoment=true;

    while(IsGameRunning())
   	{
   		sleep(2);
   	}

    cout<<"FINALIZANDO SERVIDOR!!!!"<<endl;
    ShowGamesStatus();
    EndTournoment();
}

void TournomentServer::ReadConfigFile()
{
    ConfigFile* c = new ConfigFile("config.conf");
    c->Load();
    port=c->GetSetting("Server", "Port")->AsInt;
    tournomentTime=c->GetSetting("Server", "TournomentTime")->AsInt * 60;
    startTime=c->GetSetting("Server", "StartTime")->AsInt * 60;
    gameTime=c->GetSetting("Server", "GameTime")->AsInt * 60;
    immunityTime=c->GetSetting("Server", "ImmunityTime")->AsInt;
 	gameLife=c->GetSetting("Server", "GameLife")->AsInt;
}

//Hilo CallAccept
void TournomentServer::ThreadCallAccept()
{
	try 
	{
		TCPServerSocket servSock(this->port);   

		while(true)
		{ 
			// Create separate memory for client argument  
			TCPSocket *clntSock = servSock.accept();
			thread t(&TournomentServer::HandleClientConnection, this,clntSock);	
			t.detach();
		}
	}
	catch (SocketException &e)
	{
		cerr << e.what() << endl;
		exit(1);
	}

	// pthread_exit(NULL);
}


void TournomentServer::HandleClientConnection(TCPSocket *clientSock)
{
	int recvMsgSize=0;
	string strMsj=""; 
	bool blnEndClient=false;
	const int RCVBUFSIZE = 1024;
	char echoBuffer[RCVBUFSIZE];

	string name;
	bool flag=false;
	string antMSJ="";
	
	recvMsgSize = clientSock->recv(echoBuffer, RCVBUFSIZE); 	
	if(recvMsgSize > 0)
	{      	           	
		strMsj = string(echoBuffer);
		strMsj = strMsj.substr(0,recvMsgSize);
	} 

	if(blnEndTournoment)
	{
		//respondo que finalizo el toneo, no se puede inscribi
		MsgFields objMsg;
		objMsg.idTipoMensaje(TORNEO_FINALIZADO);
		string str = objMsg.Serialize();

		cout<<str<<endl;
		clientSock->send(str.c_str(),str.size());
	}
	else
	{
		//Proceso un nuevo jugador 
		 MsgFields objMsg;
		 objMsg.Deserialize(strMsj);
		if(objMsg.idTipoMensaje() == INSCRIPCION)
		{
			cout<<"Inscribiendo a un jugador:"<<objMsg.nombreJugador()<<" "<<idClient<<endl;
			mutexListPlayer.lock();
			Player *p1 = new Player(idClient,objMsg.nombreJugador(),this->gameLife);
			p1->setSocket(clientSock);
			lstPlayers->push_back(*p1);
			idClient++;
			mutexListPlayer.unlock();

			if(startTime == 0)
				this->semNotPlayer->UnLock();			
		}
	}
}



//Hilo Armado de juegos
// Toma jugadores sin jugar que van a estar en lista lstPlayer
// y armar una lista de Games (lstGames)
void TournomentServer::MakeGames()
{	
	int intentos = 0;
	//QUITAR ESTA LINEA!! ES SOLO PARA TEST 
	startTime = 2;	

	while(!blnEndTournoment)
	{				
		//Esperamos un tiempo prudencial antes de empezar a armar partidas la primera vez
		if(startTime > 0)
		{
			sleep(startTime);
			startTime = 0;
		}	
		else
		{
			cout<<"Me bloqueo hata que caigan nuevos jugadores"<<endl;
			this->semNotPlayer->Lock();
			cout<<"Se desbloque semnotplayer"<<endl;
			intentos=0;
		}
		if(blnEndTournoment)return;

	 	long int aleatorio=0;
		srand (time(NULL)); //Utilizo la hr del sistema como semilla
		std::list<Player>::iterator it1 = lstPlayers->begin();
		std::list<Player>::iterator it2 = lstPlayers->begin();
		
		bool end=false;
		bool blnPar = false;
		if( (lstPlayers->size()%2)==0)
			blnPar=true;

		// if(lstPlayers->size() <= 1)
		// 	return;
		cout<<"Lista de players Size:"<< lstPlayers->size()<<endl;
		bool blnNewGame=false;
		while(!end && lstPlayers->size()> 1)
		{
			//Obtengo Un Player1 Al azar
			aleatorio = rand()%lstPlayers->size();
			it1 = lstPlayers->begin();
			std::advance(it1, aleatorio);
			Player objPlayer1 =*it1;

			//Obtengo Un Player2 Al azar
			aleatorio = rand()%lstPlayers->size();
			it2 = lstPlayers->begin();
			std::advance(it2, aleatorio);
			Player objPlayer2 =*it2;

			if(objPlayer1.getId()!=objPlayer2.getId() && !ArePlayerInGame(&objPlayer1,&objPlayer2))
			{
				cout<<"player1:"<<objPlayer1.getName()<<endl;
				cout<<"player2:"<<objPlayer2.getName()<<endl;

				Game *objGame = new Game(idGame,objPlayer1,objPlayer2);
				objGame->setGameTime(gameTime);
				objGame->setScorePlayer1(0);
				objGame->setScorePlayer2(0);
				lstGames->push_back(*objGame);
				idGame++;

				//Elimino de la lista de players a los jugadores seleccionados
				 lstPlayers->erase(it1);
				 lstPlayers->erase(it2);
				 blnNewGame=true;
				 intentos=0;
			}
			else
			{
				cout<<"iNTENTOS++ "<<intentos<<endl;
				intentos++;
			}

			if(intentos > 50)
				end = true;

			if((blnPar && lstPlayers->size()==0) || !blnPar && lstPlayers->size()==1) 
				end = true;
		}

		// std::unique_lock<std::mutex> lk(m);    
	 	// cv.notify_one();	
	
		//ShowGames();
		if(blnNewGame)
		{		
			for (std::list<Game>::iterator itG = lstGames->begin(); itG != lstGames->end(); itG++)
			{
				Game &objGame = *itG;
				//cout<<"Gameid"<<objGame.getId()<<"GameStatus running "<< objGame.getIsRunning()<<" finished "<<objGame.getIsFinished()<<endl;

				//cout<<"game "<<objGame.getId()<<" runnig "<<objGame.getIsRunning() <<" end "<<objGame.HasEnd()<<endl;
				if(objGame.getIsRunning() == false && objGame.getIsFinished() == false)
				{
					//No se esta ejecutando el juego por lo tanto lo lanzo y lo marco como running
					//ThrowGames(objGame);
					objGame.setIsRunning(true);

					//cout<<"Lanzando game "<<objGame.getId()<<" P1 "<<objGame.getJ1().getId()<<" P2 "<<objGame.getJ2().getId()<<" Running: "<<objGame.getIsRunning()<<" End: "<<objGame.getIsFinished()<<endl;
					thread threadThrowGames(&TournomentServer::ThrowGames,this,&objGame);
  					threadThrowGames.detach();
				}	
			}
			blnNewGame=false;
		}	
	}
}


bool TournomentServer::ArePlayerInGame(Player *p1,Player *p2)
{
	bool bln=false;
	//mutexListGame.lock();
	for (std::list<Game>::iterator it = lstGames->begin(); it != lstGames->end(); it++)
	{
		Game objGame=*it;
		int idJ1=p1->getId();
		int idJ2=p2->getId();

		if((objGame.getJ1().getId() == idJ1 && objGame.getJ2().getId() == idJ2)
			|| (objGame.getJ1().getId() == idJ2 && objGame.getJ2().getId() == idJ1))
		 	bln = true;		
	}
	//mutexListGame.unlock();
	//cout<<"devuelve ArePlayerInGame"<<bln<<endl;
	return bln;	
}


void TournomentServer::ShowGames()
{
	
}


void TournomentServer::ThrowGames(Game *objGame)
{
	t_GameData *gd = (t_GameData*)malloc(sizeof(t_GameData));

	key_t claveMemoria;
	int IdMemoria;
	//claveMemoria = ftok("D-RalpGameData", objGame->getId());
	//claveMemoria = ftok("/", objGame->getId());
	//cout<<"ClaveMemoria FTOK:"<<claveMemoria<<endl;
	claveMemoria = objGame->getId();
	IdMemoria = shmget(claveMemoria, sizeof(t_GameData), IPC_CREAT | 0777);

	//cout<<"Game id "<<objGame->getId()<<" memoria id "<<IdMemoria<<" clavemem "<<claveMemoria<<endl;
	gd = (t_GameData*)shmat(IdMemoria, NULL, 0);

	//Completamos la estruct GameData con el object
	gd->gameId = objGame->getId();
    gd->gameTime = objGame->getGameTime();
    gd->J1Id = objGame->getJ1().getId();
    gd->J2Id = objGame->getJ2().getId();
    strcpy(gd->J1Name,objGame->getJ1().getName().c_str());
    strcpy(gd->J2Name,objGame->getJ2().getName().c_str());
    gd->J1Lifes = objGame->getJ1().getLife();
    gd->J2Lifes = objGame->getJ2().getLife();
  	gd->J1Socket = objGame->getJ1().getSocketDescriptor();
	gd->J2Socket = objGame->getJ2().getSocketDescriptor();
	gd->J1Alive = true;
	gd->J2Alive = true;
    gd->J1ScorePoints = 0;
    gd->J2ScorePoints = 0;
    gd->endGame = false;

	thread threadUpdateGD(&TournomentServer::UpdateGameData,this,gd,objGame);
	threadUpdateGD.detach();



    cout<<"-------------------- STRUCT GD ANTES --------------------"<<endl;
    cout<<"Direccion de GD"<<gd<<endl;
    cout<<"GameID:"<<gd->gameId<<" J1ID:"<<gd->J1Id<<" J2ID:"<<gd->J2Id<<endl;
    cout<<"NameJ1:"<<gd->J1Name<<" NameJ2:"<<gd->J2Name<<endl;
    cout<<"Alive J1:"<<gd->J1Alive<<" Alive J2:"<<gd->J2Alive<<endl;
    cout<<"Lifes J1:"<<gd->J1Lifes<<" Lifes J2:"<<gd->J2Lifes<<endl;
    cout<<"J1ScorePoints:"<<gd->J1ScorePoints<<" J2ScorePoints:"<<gd->J2ScorePoints<<" endGame:"<<gd->endGame<<endl;
  	cout<<"---------------------------------------------------------"<<endl;    

    //Envio configuracion previa a los jugadores
    //cout<<"Antes de enviar configuraciones a "<< objGame->getJ1().getName() <<" y "<<objGame->getJ2().getName()<<endl;
    SendConfigToPlayers(objGame);

	//cout<<"ThrowGames id mem"<<IdMemoria<<endl;
	string str="";
	pid_t pid;
	pid=fork();
 	string strBinPath = Utility::GetPathBIN()+"GameServer";
 	
 	switch(pid)
	{
		case -1: //Error
				perror("fork"); 
				exit(EXIT_FAILURE);
			break;
		case 0 : //Proceso Hijo					
				str=to_string(objGame->getId());
				execl(strBinPath.c_str(),"GameServer",str.c_str(),NULL);
				//perror("fallo en execlp");		
			break;
		default: 
				pidsGamesServer->push_back(pid);

				
				//Proceso Padre
				waitpid(pid,NULL,0);				


				pidsGamesServer->remove(pid);			

				cout<<"---------------------- STRUCT GD DESPUES --------------------"<<endl;
				cout<<"Direccion de GD"<<gd<<endl;
    			cout<<"GameID:"<<gd->gameId<<" J1ID:"<<gd->J1Id<<" J2ID:"<<gd->J2Id<<endl;
    			cout<<"NameJ1:"<<gd->J1Name<<" NameJ2:"<<gd->J2Name<<endl;
    			cout<<"Alive J1:"<<gd->J1Alive<<" Alive J2:"<<gd->J2Alive<<endl;
    			cout<<"Lifes J1:"<<gd->J1Lifes<<" Lifes J2:"<<gd->J2Lifes<<endl;
    			cout<<"J1ScorePoints:"<<gd->J1ScorePoints<<" J2ScorePoints:"<<gd->J2ScorePoints<<" endGame:"<<gd->endGame<<endl;;
  				cout<<"-------------------------------------------------------------"<<endl;    


				//Luego de que termino la Partida se actualiza el objGame
				objGame->setScorePlayer1(gd->J1ScorePoints);
				objGame->setScorePlayer2(gd->J2ScorePoints);
				objGame->getJ1().setLife(gd->J1Lifes);
				objGame->getJ2().setLife(gd->J2Lifes);						
				objGame->setAliveJ1(gd->J1Alive);
				objGame->setAliveJ2(gd->J2Alive);
				objGame->setIsRunning(false);
				objGame->setIsFinished(gd->endGame);

				//objGame->setAliveJ1(CheckAlive(objGame->getJ1()));
				//objGame->setAliveJ2(CheckAlive(objGame->getJ2()));

				if(objGame->getIsFinished() == false)
				{
					//Si el game volvio con endGame = false
					//se debe volver a jugar la partida ya que no se concluyo
					//envio a los jugadores que arranquen de nuevo
					SendControlMessageToPlayers(objGame,EX_KILL_GAMESERVER);
					sleep(15);
					//Para eso quitamos el gameid de la lista de games
					//mutexListGame.lock();
					for (std::list<Game>::iterator it = lstGames->begin(); it != lstGames->end(); it++)
					{
						Game obj=*it;

						if(obj.getId()==objGame->getId())
						{
							cout<<"Borrando gameid:"<<objGame->getId()<<" listGame size:"<<lstGames->size()<<endl;
							lstGames->erase(it);
							cout<<" listGame size:"<<lstGames->size()<<endl;
							break;
						}	
					}
					//mutexListGame.unlock();
				}
					
				cout<<"desatacho mem"<<endl;
				//Desatachamos la memoria y la marcamos para ser destruida
				shmdt(gd);
				shmctl(IdMemoria, IPC_RMID, 0);

				//Si le quedan vidas a los jugadores vuelven a la lista para seguir jugando
				if(objGame->getAliveJ1())
				{
					mutexListPlayer.lock();
					this->lstPlayers->push_back(objGame->getJ1());
					mutexListPlayer.unlock();
					cout<<"vuelve a la lista el jugador J1"<<objGame->getJ1().getName()<<endl;
				}
				if(objGame->getAliveJ2())
				{
					mutexListPlayer.lock();
					this->lstPlayers->push_back(objGame->getJ2());
					mutexListPlayer.unlock();
					cout<<"vuelve a la lista el jugador J2"<<objGame->getJ2().getName()<<endl;
				}
				this->semNotPlayer->UnLock();
			break;
	}
}

void TournomentServer::SendConfigToPlayers(Game *objGame)
{	
	MsgFields objMsgToJ1;
	objMsgToJ1.idTipoMensaje(CONFIGURACIONES);
	objMsgToJ1.cantidadVidas(this->gameLife);
	objMsgToJ1.tiempoPartida(this->gameTime);
	objMsgToJ1.tiempoInmunidad(this->immunityTime);
	objMsgToJ1.numeroJugador(2);
	objMsgToJ1.nombreJugador(objGame->getJ2().getName());

	MsgFields objMsgToJ2;
	objMsgToJ2.idTipoMensaje(CONFIGURACIONES);
	objMsgToJ2.cantidadVidas(this->gameLife);
	objMsgToJ2.tiempoPartida(this->gameTime);
	objMsgToJ2.tiempoInmunidad(this->immunityTime);
	objMsgToJ2.numeroJugador(1);
	objMsgToJ2.nombreJugador(objGame->getJ1().getName());
	
	string str1 = objMsgToJ1.Serialize();
	string str2 = objMsgToJ2.Serialize();
	if(objGame->getAliveJ1())
		objGame->getJ1().getTCPSocket()->send(str1.c_str(),str1.size());
	if(objGame->getAliveJ2())
		objGame->getJ2().getTCPSocket()->send(str2.c_str(),str2.size());

	//cout<<"envio "<<str1<<endl<<"envio "<<str2<<endl;
}

void TournomentServer::SendControlMessageToPlayers(Game *objGame,int MSG)
{
	MsgFields objMsg;
	objMsg.idTipoMensaje(MSG);
	string strMsj = objMsg.Serialize();

	if(objGame->getAliveJ1())
		objGame->getJ1().getTCPSocket()->send(strMsj.c_str(),strMsj.size());
	
	if(objGame->getAliveJ2())
		objGame->getJ2().getTCPSocket()->send(strMsj.c_str(),strMsj.size());
}

void TournomentServer::ManageEvents()
{
	bool endManageEvents=false;
	SDL_Event event;

	while(!endManageEvents)
	{
		SDL_PollEvent(&event);
		if(event.type == SDL_QUIT)
		{
			//SDL_Quit();
			endManageEvents=true;			
			blnEndTournoment = true;
			//Eviamos un SIGINT a las partidas en ejecucion
			// for (std::list<int>::iterator it = pidsGamesServer->begin(); it != pidsGamesServer->end(); it++)
			// {
			// 	int pid = *it;
			// 	cout<<"enviando SIGINT a "<<pid<<endl;
			// 	kill(pid,SIGINT);
			// }
			EndTournomentBySignal();
		}

		usleep(40*1000);
	}
}

void TournomentServer::ShowGamesStatus()
{
	cout<<"Listando Games finales"<<endl;
	for (std::list<Game>::iterator it = lstGames->begin(); it != lstGames->end(); it++)
	{
		Game objGame=*it;
		
		cout<<"GameID:"<<objGame.getId()<<endl;
		cout<<"Player1:"<<objGame.getJ1().getId()<<" "<<objGame.getJ1().getName()<<endl;
		cout<<"Player2:"<<objGame.getJ2().getId()<<" "<<objGame.getJ2().getName()<<endl;
		cout<<"Score1:"<<objGame.getScorePlayer1()<<" Score2:"<<objGame.getScorePlayer2()<<endl;
		cout<<"Finish:"<<objGame.getIsFinished()<<" Running:"<<objGame.getIsRunning()<<endl;
		cout<<"-------------------------------------------------------------------------"<<endl;
	}
}

void TournomentServer::EndTournomentBySignal()
{
	MsgFields objMsg;
	objMsg.idTipoMensaje(EX_KILL_TOURNOMENT);
	string strKillTournoment = objMsg.Serialize();

	for (std::list<Game>::iterator it = lstGames->begin(); it != lstGames->end(); it++)
	{
		Game objGame=*it;
		cout<<"Enviando TOURNOMENTKILL a "<<objGame.getJ1().getId()<<endl;
		cout<<"Enviando TOURNOMENTKILL a "<<objGame.getJ2().getId()<<endl;
		if(objGame.getAliveJ1())
			objGame.getJ1().getTCPSocket()->send(strKillTournoment.c_str(),strKillTournoment.size());
		if(objGame.getAliveJ2())
			objGame.getJ2().getTCPSocket()->send(strKillTournoment.c_str(),strKillTournoment.size());
	}
}

//Destructor
TournomentServer::~TournomentServer()
{
	delete lstPlayers;
	delete semNotPlayer;
	delete lstGames;

	//delete screen;

}

int TournomentServer::GetPlayerCountInGame()
{
	int cont=0;
	for (std::list<Game>::iterator it = lstGames->begin(); it != lstGames->end(); it++)
	{
		Game objGame=*it;
		if(!objGame.getIsFinished() && objGame.getIsRunning())
		{
			if(objGame.getAliveJ1())
				cont++;
			if(objGame.getAliveJ2())
				cont++;
		}
	}
	return cont;
}

void TournomentServer::ShowResults()
{
	while(!blnEndTournoment)
	{
		int cant = lstPlayers->size() + GetPlayerCountInGame();
		//int cant =0;
		if(resumeScreen!=NULL)
		{
			resumeScreen->actualizarConectados(cant);
			resumeScreen->actualizarPartidas(*lstGames);
		}
		sleep(2);
	}
}

bool TournomentServer::IsGameRunning()
{
	for (std::list<Game>::iterator it = lstGames->begin(); it != lstGames->end(); it++)
	{
		Game objGame=*it;
		if(objGame.getIsRunning())
		{
			return true;
		}
	}
	return false;
}

void TournomentServer::EndTournoment()
{
	cout<<"EndTournoment"<<endl;
	map<string,int> puntos;

	MsgFields objMsg;
	objMsg.idTipoMensaje(TORNEO_FINALIZADO);
	objMsg.numeroJugador(lstPlayers->size());
	string strMsj = objMsg.Serialize();

	for (std::list<Player>::iterator it = lstPlayers->begin(); it != lstPlayers->end(); it++)
	{
		Player objPlayer = *it;
		objPlayer.getTCPSocket()->send(strMsj.c_str(),strMsj.size());

		puntos[to_string(objPlayer.getId()) + " - " + objPlayer.getName()] = 0;
	}

	sleep(2);


	for (std::list<Player>::iterator itP = lstPlayers->begin(); itP != lstPlayers->end(); itP++)
	{
		Player objPlayer = *itP;
		for (std::list<Game>::iterator itG = lstGames->begin(); itG != lstGames->end(); itG++)
		{
			Game objGame=*itG;

			if(objGame.getIsFinished())
			{
				
				if(objPlayer.getId() == objGame.getJ1().getId())
					puntos[to_string(objGame.getJ1().getId()) + " - " + objGame.getJ1().getName()] += objGame.getScorePlayer1();
			
				if(objPlayer.getId() == objGame.getJ2().getId())
					puntos[to_string(objGame.getJ2().getId()) + " - " + objGame.getJ2().getName()] += objGame.getScorePlayer2();
			}
		}
	}
	cout<<"PlayerSIze"<<lstPlayers->size()<<endl;
	for (std::map<string,int>::iterator it=puntos.begin(); it!=puntos.end(); it++)
   		std::cout << it->first << " => " << it->second << '\n';

	for (std::list<Player>::iterator it = lstPlayers->begin(); it != lstPlayers->end(); it++)
	{
		Player objPlayer = *it;

		for (std::map<string,int>::iterator itMap=puntos.begin(); itMap!=puntos.end(); itMap++)
    	{//std::cout << it->first << " => " << it->second << '\n';
			MsgFields objMsg;
			objMsg.idTipoMensaje(RESULTADO);
			objMsg.nombreJugador(itMap->first);
			objMsg.puntosTotales(itMap->second);
			string strMsj = objMsg.Serialize();
			cout<<"envioResultado:"<<strMsj<<endl;
			try
			{
				objPlayer.getTCPSocket()->send(strMsj.c_str(),strMsj.size());
			}
			catch(SocketException &e)
			{

			}
		}
	}
}

void TournomentServer::CheckPlayerAlive()
{
	mutexListPlayer.lock();
	for (std::list<Player>::iterator itP = lstPlayers->begin(); itP != lstPlayers->end(); itP++)
	{
		Player objPlayer= *itP;

		try
		{
			MsgFields objMsg;
			objMsg.idTipoMensaje(ACK);
			string str = objMsg.Serialize();
			objPlayer.getTCPSocket()->send(str.c_str(),str.size());
		}
		catch(SocketException &e)
		{
			cout<<"se-------------------"<<endl;
			cout<<"SocketException"<<e.what()<<endl;
			lstPlayers->erase(itP);
			cout<<"luego de eliminar "<<lstPlayers->size()<<endl;
			break;
		}
	}
	mutexListPlayer.unlock();
}


void TournomentServer::UpdateGameData(t_GameData *gd,Game * objGame)
{

	while(objGame->getIsRunning())
	{

		objGame->setScorePlayer1(gd->J1ScorePoints);
		objGame->setScorePlayer2(gd->J2ScorePoints);
		objGame->setAliveJ1(gd->J1Alive);
		objGame->setAliveJ2(gd->J2Alive);

		sleep(2);
	}
}