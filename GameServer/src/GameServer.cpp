#include "GameServer.h"

GameServer::GameServer()
{
}

GameServer::GameServer(int intGameId)
{
	key_t claveMemoria;
	claveMemoria = intGameId;
	idMemoria = shmget(claveMemoria, sizeof(t_GameData), IPC_CREAT | 0777);
	this->idMemoria = idMemoria;
	flagEndDrawLevel = 0;

	gd = (t_GameData*)shmat(idMemoria, NULL, 0);


	//Creamos el Player J1
	TCPSocket * socketJ1 = new TCPSocket(gd->J1Socket);
	Player *playerJ1 = new Player(gd->J1Id,string(gd->J1Name),gd->J1Lifes,socketJ1);	
	//Creamos el Player J2
	TCPSocket * socketJ2 = new TCPSocket(gd->J2Socket);
	Player *playerJ2 = new Player(gd->J2Id,string(gd->J2Name),gd->J2Lifes,socketJ2);
	//Creamos el Game
	this->objGame = new Game(gd->gameId,*playerJ1,*playerJ2);
	this->objGame->setIsRunning(true);
	this->objGame->setGameTime(gd->gameTime);

	ROWS = 13;
    COLUMNS = 5;   
    PISOSXNIVEL=4;
    pisoActual=0;
    posXJ1=0,posYJ1=0;
    posXJ2=4,posYJ2=0;

	matrix.resize(ROWS);

	for (int i = 0; i < ROWS; i++)
	{
		matrix[i].resize(COLUMNS);
	}
	this->objGame = objGame;
	
	messages = new list<Message>();	
	intReadyToPlay=0;
	this->blnEndRetrieveMessage = false;
	blnEndGame = false;
	isTournomentAlive=true;
	cout<<"SoyGameServer gameId"<<gd->gameId<<" Direcion GD"<<gd<<endl;

	//prctl(PR_SET_PDEATHSIG, SIGTERM);
}

GameServer::~GameServer()
{
	delete messages;
	//delete objGame;
}

string GameServer::MakeLevel(int level)
{
	if(level == 1)
	{
		matrix[0][0] = VENTANA_ROTA1;matrix[0][1] = VENTANA_SANA;matrix[0][2] = VENTANA_ROTA3;matrix[0][3] = VENTANA_CERRADA;matrix[0][4] = VENTANA_SANA;
		matrix[1][0] = VENTANA_SANA;matrix[1][1] = VENTANA_ROTA1;matrix[1][2] = VENTANA_ROTA3;matrix[1][3] = VENTANA_ROTA3;matrix[1][4] = VENTANA_ROTA2;
		matrix[2][0] = VENTANA_ROTA2;matrix[2][1] = VENTANA_SANA;matrix[2][2] = VENTANA_ROTA1;matrix[2][3] = VENTANA_CERRADA;matrix[2][4] = VENTANA_SANA;
		matrix[3][0] = VENTANA_ROTA1;matrix[3][1] = VENTANA_SANA;matrix[3][2] = VENTANA_ROTA3;matrix[3][3] = VENTANA_CERRADA;matrix[3][4] = VENTANA_SANA;
		matrix[4][0] = VENTANA_SANA;matrix[4][1] = VENTANA_ROTA1;matrix[4][2] = VENTANA_ROTA3;matrix[4][3] = VENTANA_ROTA3;matrix[4][4] = VENTANA_ROTA2;
		matrix[5][0] = VENTANA_ROTA2;matrix[5][1] = VENTANA_SANA;matrix[5][2] = VENTANA_ROTA1;matrix[5][3] = VENTANA_CERRADA;matrix[5][4] = VENTANA_SANA;
		matrix[6][0] = VENTANA_SANA;matrix[6][1] = VENTANA_ROTA1;matrix[6][2] = VENTANA_ROTA2;matrix[6][3] = VENTANA_ROTA3;matrix[6][4] = VENTANA_CERRADA;
		matrix[7][0] = VENTANA_SANA;matrix[7][1] = VENTANA_ROTA1;matrix[7][2] = VENTANA_ROTA2;matrix[7][3] = VENTANA_ROTA3;matrix[7][4] = VENTANA_CERRADA;
		matrix[8][0] = VENTANA_SANA;matrix[8][1] = VENTANA_ROTA1;matrix[8][2] = VENTANA_ROTA2;matrix[8][3] = VENTANA_ROTA3;matrix[8][4] = VENTANA_CERRADA;
		matrix[9][0] = VENTANA_SANA;matrix[9][1] = VENTANA_ROTA1;matrix[9][2] = VENTANA_ROTA2;matrix[9][3] = VENTANA_ROTA3;matrix[9][4] = VENTANA_CERRADA;
		matrix[10][0] = VENTANA_SANA;matrix[10][1] = VENTANA_ROTA1;matrix[10][2] = VENTANA_ROTA2;matrix[10][3] = VENTANA_ROTA3;matrix[10][4] = VENTANA_CERRADA;
		matrix[11][0] = VENTANA_SANA;matrix[11][1] = VENTANA_ROTA1;matrix[11][2] = VENTANA_ROTA2;matrix[11][3] = VENTANA_ROTA3;matrix[11][4] = VENTANA_CERRADA;
		matrix[12][0] = VENTANA_SANA;matrix[12][1] = VENTANA_ROTA1;matrix[12][2] = VENTANA_ROTA2;matrix[12][3] = VENTANA_ROTA3;matrix[12][4] = VENTANA_CERRADA;
		// //matrix[13][0] = 1;matrix[13][1] = 1;matrix[13][2] = 1;matrix[13][3] = 1;matrix[13][4] = 1;
		//matrix[14][0] = 1;matrix[14][1] = 1;matrix[14][2] = 1;matrix[14][3] = 1;matrix[14][4] = 1;
	}

	string str = "";
	for(int i=0; i< ROWS; i++)
		for(int j=0;j<COLUMNS;j++)
			str+=to_string(i)+","+to_string(j)+","+to_string(matrix[i][j])+",";
	str = str.substr(0, str.size()-1);
	return str;
}

void GameServer::PrepareGameToPlayers()
{
	
	//string cadena = "0,0,1,0,1,0,0,2,3,0,3,4,0,4,0,1,0,0,1,1,1,1,2,3,1,3,3,1,4,2,2,0,2,2,1,0,2,2,1,2,3,4,2,4,0,3,0,1,3,1,0,3,2,3,3,3,4,3,4,0,4,0,0,4,1,1,4,2,3,4,3,3,4,4,2,5,0,2,5,1,0,5,2,1,5,3,4,5,4,0,6,0,0,6,1,1,6,2,2,6,3,3,6,4,4,7,0,0,7,1,1,7,2,2,7,3,3,7,4,4,8,0,0,8,1,1,8,2,2,8,3,3,8,4,4,9,0,0,9,1,1,9,2,2,9,3,3,9,4,4,10,0,0,10,1,1,10,2,2,10,3,3,10,4,4,11,0,0,11,1,1,11,2,2,11,3,3,11,4,4,12,0,0,12,1,1,12,2,2,12,3,3,12,4,4";
	string cadena = MakeLevel(1);
	MsgFields objMsgEdificio;
	objMsgEdificio.idTipoMensaje(EDIFICIO);
	objMsgEdificio.strEdificio(cadena);

	string strEdificio = objMsgEdificio.Serialize();

	if(objGame->getAliveJ1())
		objGame->getJ1().getTCPSocket()->send(strEdificio.c_str(),strEdificio.size());
	if(objGame->getAliveJ2())
		objGame->getJ2().getTCPSocket()->send(strEdificio.c_str(),strEdificio.size());


	while(intReadyToPlay<2)
	{
		sleep(2);
	}

 	MsgFields objMsgJUGAR;
	objMsgJUGAR.idTipoMensaje(JUGAR);
	string strJugar = objMsgJUGAR.Serialize();
	if(objGame->getAliveJ1())
		objGame->getJ1().getTCPSocket()->send(strJugar.c_str(),strJugar.size());
	if(objGame->getAliveJ2())
		objGame->getJ2().getTCPSocket()->send(strJugar.c_str(),strJugar.size());
	cout<<"envio jugar a ambos:"<<strJugar<<endl;
}



void GameServer::MainFunction()
{   
    // Register signal handler to handle kill signal
    signalHandler.setupSignalHandlers();
   
	//Hilo para recibir mensajes del Jugador 1
	thread thRecvJ1(&GameServer::ReceiveMessage, this,objGame->getJ1());
	thRecvJ1.detach();
	//Hilo para recibir mensajes del Jugador 2
	thread thRecvJ2(&GameServer::ReceiveMessage, this,objGame->getJ2());
	thRecvJ2.detach();	
	//Hilo para recuperar los mensajes de la listas e ir procesandolos
	thread thRetriveMessage(&GameServer::RetrieveMessageFromList,this);
	thRetriveMessage.detach();
	//configuramos los escenarios para los jugadores
	PrepareGameToPlayers();

	
	// hilo para enviar objetos varios(ladrillo gaviotas,etc)
	thread thSendObjects(&GameServer::SendObjects,this);
	thSendObjects.detach();

	//Hilo para chequear el timer para determinar el fin de la partida
	thread thTimer(&GameServer::Timer,this);
	thTimer.join();	

	//Cuando llega a esta instancia finalizo el timer y debo salir la partida
	
	//cout<<"Se esta finalizando la partida... Actualizo GD"<<endl;

	if(this->signalHandler.gotExitSignal() || !isTournomentAlive)
	{
		//SendControlMessageToPlayers(EX_KILL_GAMESERVER);
		objGame->setIsFinished(false);
		ReleaseResources();
	}
	else
	{
		SendControlMessageToPlayers(PARTIDA_FINALIZADA);
		objGame->setIsFinished(true);
		UpdateGameData();
	}
}

void GameServer::Timer()
{
	int dd=0,hh=0,mm=0,ss=0;
	
	int gameTime = this->objGame->getGameTime();

	Utility::ConvertSecondToTime(gameTime,&dd,&hh,&mm,&ss);	
	while(!blnEndGame && gameTime > 0)
	{		
		//cout<<"dd:"<<dd<<" hh:"<<hh<<" mm:"<<mm<<" ss:"<<ss<<endl;
		gameTime--;
		Utility::ConvertSecondToTime(gameTime,&dd,&hh,&mm,&ss);
    	sleep(1);
    	this->objGame->setGameTime(gameTime); 


    	if(!IsTournomentAlive())
		{
			isTournomentAlive = false; 
			gameTime=0;
		}

    	if(this->signalHandler.gotExitSignal())
			gameTime=0;			 			


		if(!objGame->getAliveJ1() && !objGame->getAliveJ2())
		{			
			mutexProcessMessages.unlock();
			mutexProcessMessages.unlock();
			blnEndRetrieveMessage=true;
			gameTime=0;			
		}
		
		UpdateGameData();
    
    }
    //Al salir de este timer finalizamos el torneo
    if(this->signalHandler.gotExitSignal())
	    blnEndGame=false;
	else
		blnEndGame=true;    
}

void GameServer::ReceiveMessage(Player objPlayer)
{
	try
	{
		bool blnEndReceive = false;
		cout<< "Entro en Recibe Message "<<objPlayer.getName()<<endl;
		char buff[BUFFTAM];
		while(!blnEndReceive)
		{			
			int result = objPlayer.getTCPSocket()->recv(buff,BUFFTAM);
			if( result > 0)
			{
				string temp(buff);
				list<string> jsons = Utility::SplitJSON(temp.substr(0,result));

				mutexMessages.lock();
				for (std::list<string>::iterator itj= jsons.begin(); itj != jsons.end(); itj++)
				{
					Message m1(objPlayer,string(*itj));
					//cout<<"Meto en Messages:"<<string(*itj)<<endl;
					messages->push_back(m1);
					mutexProcessMessages.unlock();
				}
				
				mutexMessages.unlock();
			}else
				if(result==0)
				{
					cout<<"El cliente "<<objPlayer.getId()<<" ha cerrado la conexion"<<endl;					
					if(objGame->getJ1().getId() == objPlayer.getId())
					{
						objGame->setAliveJ1(false);	
						this->posXJ1 = this->posYJ1 = -1;
					}	
					if(objGame->getJ2().getId() == objPlayer.getId())
					{
						objGame->setAliveJ2(false);
						this->posXJ2 = this->posYJ2 = -1;	
					}
					blnEndReceive = true;

					cout<<"J1Alive:"<<objGame->getAliveJ1()<<" J2Alive:"<<objGame->getAliveJ2()<<endl;					
				}
				else
				{
					cout<<"error socket return -1"<<endl;
					blnEndReceive = true;
				}
		}
	}
	catch (SocketException &e)
	{
		//objPlayer.setKeepConnection(false);
		if(objGame->getJ1().getId() == objPlayer.getId())
			objGame->setAliveJ1(false);

		if(objGame->getJ2().getId() == objPlayer.getId())
			objGame->setAliveJ2(false);
		cerr << e.what() << endl;
		//exit(1);				
	}
}


void GameServer::RetrieveMessageFromList()
{	

	mutexProcessMessages.lock();

	while(!blnEndRetrieveMessage)
	{		
		mutexProcessMessages.lock();
		
		///Seccion critica
		mutexMessages.lock();
		std::list<Message>::iterator it = messages->begin();
		if(it != messages->end())
		{

			Message msg = *it;
			Message msgToProcess(msg.getPlayer(),msg.getMessage());
	 		//cout<<"Recupero de la lista size()"<<messages->size()<<":"<<msg.getMessage()<<endl;
	 		messages->erase(it);	
	 		mutexMessages.unlock();
			ProcessMessage(msgToProcess);
		}

		if(!objGame->getAliveJ1() && !objGame->getAliveJ2())
		{
			cout<<"se ha cerrado la partida porque ambos han cerrado la conexicon"<<endl;
			blnEndRetrieveMessage = true;
		}

		if(objGame->getIsFinished())
		{
			cout<<"Ha finalizado la partida en game server"<<endl;
			blnEndRetrieveMessage = true;
		}
	}
}

void GameServer::ProcessMessage(Message msg)
{
    //Procesamiento del msj en cuestion
	cout<<"ProcesoMSG:"<<msg.getMessage()<<endl;
	MsgFields objMsg;
	objMsg.Deserialize(msg.getMessage());


	switch(objMsg.idTipoMensaje())
	{
		case FIN_PISO:
		{
			flagEndDrawLevel--;	

			if(flagEndDrawLevel == 0 && pisoActual >=3 )	
				blnEndGame=true;
			break;
		}
		case JUGAR:
			intReadyToPlay++;
			break;
		case TECLAS:			
				RefreshPositionPlayer(objMsg.teclaPresionada(), msg.getPlayer());					
			break;

		case PERDER_VIDA:
				cout<<"perder vida"<<endl;
				if(msg.getPlayer().getId() == objGame->getJ1().getId())
				{
					objGame->getJ1().setLife(objGame->getJ1().getLife()-1);
				}
				else
				{
					objGame->getJ2().setLife(objGame->getJ2().getLife()-1);
				}

				cout<<"Vidas J1 "<<	objGame->getJ1().getLife()<<endl;
				cout<<"Vidas J2 "<<	objGame->getJ2().getLife()<<endl;
				
				SendLostLife(msg.getPlayer());

				if(objGame->getJ1().getLife() <= 0)
				{ 
					this->posXJ1 = this->posYJ1 = -1;
					cout<<"pos-1 j1"<<endl;
				}

				if(objGame->getJ2().getLife() <= 0)
				{
					this->posXJ2 = this->posYJ2 = -1;
					cout<<"pos-1 j2"<<endl;
				}

			break;
		default:
			cout<<"Mensaje desconocido"<<objMsg.idTipoMensaje()<<endl;
			break;
	}
}

void GameServer::RefreshPositionPlayer(int intTeclaPresionada, Player objPlayerOrigin)
{
	bool blnSendPosition=false;
	bool blnSendFix=false;
	int x,y;
	int x2,y2;
	int points;
	if(objPlayerOrigin.getId() == objGame->getJ1().getId())
	{	
		x = this->posXJ1;
		y = this->posYJ1;
		x2 = this->posXJ2;
		y2 = this->posYJ2;
	}
	else
	{	
		x = this->posXJ2;
		y = this->posYJ2;
		x2 = this->posXJ1;
		y2 = this->posYJ1;
	}

	cout<<"x1:"<<x<<" y1:"<<y<<" x2:"<<x2<<" y2:"<<y2<<endl;
	switch(intTeclaPresionada)
	{
		case ARRIBA:				
			if( ((y+1) < ((pisoActual*PISOSXNIVEL)+PISOSXNIVEL)) && !CheckColision(x,y+1,x2,y2) )
			{	y++;
				blnSendPosition=true;
			}
			break;
		case ABAJO:
			if((y-1) >= (pisoActual*PISOSXNIVEL) && !CheckColision(x,y-1,x2,y2) )
			{	y--; 
				blnSendPosition=true;
			}
			break;
		case DERECHA:
			if(((x+1) < COLUMNS ) && !CheckColision(x+1,y,x2,y2) )
			{	x++;
				blnSendPosition=true;
			}
			break;
		case IZQUIERDA:
			if((x-1)>=0 && !CheckColision(x-1,y,x2,y2) )
			{	x--;
				blnSendPosition=true;
			}
			break;
		case REPARAR:
				switch(matrix[y][x])
				{
					//cout<<"estado mat("<<x<<","<<y<<")="<<matrix[x][y]<<endl;
					case VENTANA_ROTA1:
						points=10;
						blnSendFix=true;
						matrix[y][x]=VENTANA_SANA;
						break;
					case VENTANA_ROTA2:
						points=10;
						blnSendFix=true;
						matrix[y][x]=VENTANA_SANA;
						break;
					case VENTANA_ROTA3:
						points=15;	
						blnSendFix=true;
						matrix[y][x]=VENTANA_ROTA2;
						break;
					//cout<<"estado Final mat("<<x<<","<<y<<")="<<matrix[x][y]<<endl;
					//cout<<"sendfix:"<<blnSendFix<<endl;
				}
				
				//sleep(2);							

			break;
		default:
			cout<<"tecla desconocida"<<intTeclaPresionada<<endl;
			break;
	}
	if(objPlayerOrigin.getId() == objGame->getJ1().getId())
	{	
		this->posXJ1=x;
		this->posYJ1=y;
		if(blnSendFix)
		{
			objGame->setScorePlayer1(objGame->getScorePlayer1()+points);
			if(objGame->getAliveJ1())
				SendWindowFixedToPlayer(JUGADOR1,objGame->getJ1());		
			if(objGame->getAliveJ2())
				SendWindowFixedToPlayer(JUGADOR1,objGame->getJ2());
			CheckNextLevel();


		}else if(blnSendPosition)
		{
			if(objGame->getAliveJ1())
				SendPositionToPlayer(JUGADOR1,objGame->getJ1());
			
			if(objGame->getAliveJ2())
				SendPositionToPlayer(JUGADOR1,objGame->getJ2());
		}
	}
	else
	{
		this->posXJ2=x;
		this->posYJ2=y;	
		if(blnSendFix)
		{
			objGame->setScorePlayer2(objGame->getScorePlayer2()+points);
			
			if(objGame->getAliveJ1())
				SendWindowFixedToPlayer(JUGADOR2,objGame->getJ1());		
			if(objGame->getAliveJ2())
				SendWindowFixedToPlayer(JUGADOR2,objGame->getJ2());
			CheckNextLevel();

		
		}else if(blnSendPosition)
		{
			if(objGame->getAliveJ1())
				SendPositionToPlayer(JUGADOR2,objGame->getJ1());		
			if(objGame->getAliveJ2())
				SendPositionToPlayer(JUGADOR2,objGame->getJ2());
		}
	}
}

bool GameServer::CheckColision(int x1,int y1,int x2,int y2)
{
	if(x1 == x2 && y1==y2)
		return true;
	else
		return false;
}

void GameServer::SendPositionToPlayer(int JX, Player objPlayer)
{
	MsgFields objMsg;
	objMsg.idTipoMensaje(MOVIMIENTO_OBJETO);
	objMsg.tipoObjeto(JX);

	//if(objPlayer.getId() == objGame->getJ1().getId())
	if(JX == JUGADOR1)
	{
		objMsg.columna(this->posXJ1);
		objMsg.fila(this->posYJ1);
	}
	else
	{
		objMsg.columna(this->posXJ2);
		objMsg.fila(this->posYJ2);
	}

	string str = objMsg.Serialize();

	
	cout<<"Envio PositionToPlayer: "<<objPlayer.getId()<<" "<<str<<endl;	
	objPlayer.getTCPSocket()->send(str.c_str(),str.size());
}

void GameServer::CheckNextLevel()
{
	//Voy a checkear que todas las ventanas esten arregladas
	bool blnNextLevel = true;
	for(int i = pisoActual*PISOSXNIVEL; i < ((pisoActual + 1)*PISOSXNIVEL) && blnNextLevel == true; i ++)
		for(int j = 0; j < COLUMNS && blnNextLevel == true; j++)
			if(matrix[i][j] != VENTANA_SANA && matrix[i][j] != VENTANA_CERRADA)
				blnNextLevel = false;


	if(blnNextLevel && pisoActual < 3)
	{
		pisoActual++;
		flagEndDrawLevel = 2;
		this->posXJ1 = 0;
		this->posYJ1 = pisoActual*PISOSXNIVEL;
		this->posXJ2 = 4;
		this->posYJ2 = pisoActual*PISOSXNIVEL;
		if(objGame->getAliveJ1())
			SendNextLevelToPlayer(objGame->getJ1());
		if(objGame->getAliveJ2())
			SendNextLevelToPlayer(objGame->getJ2());
	}

}

void GameServer::SendNextLevelToPlayer(Player objPlayer)
{
	MsgFields objMsg;
	objMsg.idTipoMensaje(FIN_PISO);
	string str = objMsg.Serialize();

	cout<<"Envio NextLevelToPlayer: "<<objPlayer.getId()<<" "<<str<<endl;
	objPlayer.getTCPSocket()->send(str.c_str(),str.size());
}


void GameServer::SendWindowFixedToPlayer(int JX, Player objPlayer)
{
	MsgFields objMsg;
	objMsg.idTipoMensaje(VENTANA_REPARADA);
	objMsg.tipoObjeto(JX);

	//if(objPlayer.getId() == objGame->getJ1().getId())
	if(JX == JUGADOR1)
	{
		objMsg.columna(this->posXJ1);
		objMsg.fila(this->posYJ1);
		objMsg.puntosTotales(objGame->getScorePlayer1());
	}
	else
	{
		objMsg.columna(this->posXJ2);
		objMsg.fila(this->posYJ2);
		objMsg.puntosTotales(objGame->getScorePlayer2());
	}

	string str = objMsg.Serialize();

	
	cout<<"envio Repara "<<objPlayer.getId()<<str<<endl;
	objPlayer.getTCPSocket()->send(str.c_str(),str.size());
}

void GameServer::SendLostLife(Player objPlayer)
{
	MsgFields objMsg;
	objMsg.idTipoMensaje(PERDER_VIDA);


	//cout<<"envio lostlife "<<objPlayer.getId()<<str<<endl;

	if(objPlayer.getId() == objGame->getJ1().getId())
	{			
		//this->objGame->getJ1().setLife(this->objGame->getJ1().getLife()-1);
		objMsg.tipoObjeto(JUGADOR1);
		string str = objMsg.Serialize();
		if(objGame->getAliveJ2())
			objGame->getJ2().getTCPSocket()->send(str.c_str(),str.size());
	}
	else
	{

		//this->objGame->getJ2().setLife(this->objGame->getJ2().getLife()-1);
		objMsg.tipoObjeto(JUGADOR2);
		string str = objMsg.Serialize();
		if(objGame->getAliveJ1())
			objGame->getJ1().getTCPSocket()->send(str.c_str(),str.size());
	}



}
void GameServer::UpdateGameData()
{
	//t_GameData *gd;
	//gd = (t_GameData*)shmat(idMemoria, NULL, 0);

	// //Actualizamos el estado del Game y sus jugadores
	//gd->J1Lifes = objGame->getJ1().getLife()-1;
	//gd->J2Lifes = objGame->getJ2().getLife()-2;
	gd->J1ScorePoints = objGame->getScorePlayer1();
	gd->J2ScorePoints = objGame->getScorePlayer2();
	gd->J1Alive = objGame->getAliveJ1();
	gd->J2Alive = objGame->getAliveJ2();
	gd->endGame = objGame->getIsFinished();

	//shmdt(gd);
}


void GameServer::SendControlMessageToPlayers(int MSG)
{
	MsgFields objMsg;
	objMsg.idTipoMensaje(MSG);
	string strFIN = objMsg.Serialize();

	if(objGame->getAliveJ1())
		objGame->getJ1().getTCPSocket()->send(strFIN.c_str(),strFIN.size());
	
	if(objGame->getAliveJ2())
		objGame->getJ2().getTCPSocket()->send(strFIN.c_str(),strFIN.size());
}

void GameServer::SendObjectsToGame(int tipoMsj,int tipoObj,int x,int y)
{
	MsgFields objMsg;
	objMsg.idTipoMensaje(tipoMsj);
	objMsg.tipoObjeto(tipoObj);
	objMsg.columna(x);
	objMsg.fila(y);
	string str = objMsg.Serialize();
	cout<<"SendingObjets:"<<str<<endl;

	if(objGame->getAliveJ1())
		this->objGame->getJ1().getTCPSocket()->send(str.c_str(),str.size());
	
	if(objGame->getAliveJ2())
		this->objGame->getJ2().getTCPSocket()->send(str.c_str(),str.size());
}

//Hilo que mantiene la logica en envio de ladrillos movimiento de bowser
void GameServer::SendObjects()
{

	long int aleatorio=0;
	int bowserCol = 2;
	int bowserFil = (pisoActual*PISOSXNIVEL) + PISOSXNIVEL;
	int refresco = 3;
	while(!blnEndGame)
	{		
		while(flagEndDrawLevel > 0)
			sleep(1);

		srand (time(NULL)); //Utilizo la hr del sistema como semilla
		aleatorio = rand()%(5);

		bowserFil = (pisoActual*PISOSXNIVEL) + PISOSXNIVEL;
		switch(aleatorio)
		{
			case 0:
				SendObjectsToGame(MOVIMIENTO_OBJETO,GAVIOTA,0,(pisoActual*PISOSXNIVEL)+1);
				SendObjectsToGame(MOVIMIENTO_OBJETO,GAVIOTA,COLUMNS-1,(pisoActual*PISOSXNIVEL)+2);
				break;
			case 1:
				SendObjectsToGame(MOVIMIENTO_OBJETO,LADRILLO,bowserCol,bowserFil);
				break;
			case 2:
				SendObjectsToGame(MOVIMIENTO_OBJETO,LADRILLO,bowserCol,bowserFil);
				break;
			case 3:
				if(bowserCol<(COLUMNS - 1))
				{	
					bowserCol++;
					SendObjectsToGame(MOVIMIENTO_OBJETO,RALPH,bowserCol,bowserFil);
				}
				break;
			case 4:
				if(bowserCol > 0)
				{
					bowserCol--;
					SendObjectsToGame(MOVIMIENTO_OBJETO,RALPH,bowserCol,bowserFil);
				}
				break;
		}

		sleep((refresco - (pisoActual)) > 0 ? refresco - (pisoActual) : 1);
	}
}


void GameServer::EndGameServerBySignal()
{
	cout<<"Finalizando by signal Partida ID:"<<this->objGame->getId()<<" espere..."<<endl;
	this->objGame->setIsFinished(false);
	mutexProcessMessages.unlock();
	mutexProcessMessages.unlock();
	blnEndRetrieveMessage=true;
	blnEndGame=true;

}

bool GameServer::IsTournomentAlive()
{
	pid_t ppid = getppid();
	int status = kill(ppid,0);

	if(status <0)
		return false;
	else
		return true;
}

void GameServer::ReleaseResources()
{
	cout<<"empiezo a liberar recursos"<<endl;
	shmdt(gd);
	shmctl(idMemoria, IPC_RMID, 0);	

	objGame->getJ1().getTCPSocket()->CloseConnection();
	objGame->getJ2().getTCPSocket()->CloseConnection();
	objGame->getJ1().getTCPSocket()->CloseConnection();
	objGame->getJ2().getTCPSocket()->CloseConnection();
	
	Semaphore *semNotPlayer;
	semNotPlayer = new Semaphore("SemNotPlayer",false,0);
	delete semNotPlayer;

	cout<<"termine de  liberar recursos"<<endl;


}