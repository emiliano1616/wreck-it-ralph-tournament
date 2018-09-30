#ifndef TOURNOMENTSERVER_H
#define TOURNOMENTSERVER_H


#include <list>
#include "Socket.h"
#include <iostream>
#include <thread>
#include <string>
#include <signal.h>
#include <unistd.h>
#include "ConfigFile.h"
#include "Player.h"
#include "Game.h"
//#include "List.cpp"
#include <mutex>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include "PantallaServidor.h"
#include "Utility.h"
#include <condition_variable>
#include <atomic>
#include <chrono>
#include "Semaphore.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <chrono>
#include <ctime>
#include "MsgFields.h"
#include <SDL/SDL.h>
#include "PantallaResumen.h"

using namespace std;


class TournomentServer
{
    private:
        SDL_Surface *screen;

        //DateTime horaInicioTOrneo;
        int idClient;  
        int idGame;      
        int port;
        int tournomentTime;
        int startTime;
        int gameTime; 
        int immunityTime;
        int gameLife;

        bool blnEndTournoment;
        bool blnMakeGames;
        bool blnEndMake;
        mutex mutexListPlayer; 
        mutex mutexListGame;     
        //Lista de jugadores
        list<Player> *lstPlayers;
        //Lista de juegos 
        list<Game> *lstGames;
        list<int> *pidsGamesServer;
        
        std::mutex m;
        std::condition_variable cv;
        Semaphore *semNotPlayer;

        PantallaResumen *resumeScreen;

    public:
    //Constructor
    TournomentServer();
    TournomentServer(SDL_Surface *);
    //MainFunction
    void MainFunction();
    //Lee el archivo de configuracion
    void ReadConfigFile();
    //Funcion del hilo CallAccept    
    void ThreadCallAccept();    
    //Funcion del hilo de cada cliente conectado
    void HandleClientConnection(TCPSocket *);

    void Timer();

    int GetRandomNumber(int);
    bool ArePlayerInGame(Player *, Player *);    

    void ShowGames();
    //Armador de partidas
    void MakeGames();
    //Lanza las partidas en un proceso distinto
    void ThrowGames(Game*);

    void SendConfigToPlayers(Game *);

    void ShowResults();

    void ManageEvents();

    void ShowGamesStatus();

    void SendControlMessageToPlayers(Game * ,int );

    void EndTournomentBySignal();

    int GetPlayerCountInGame();
    //Destructor
    ~TournomentServer();

    bool IsGameRunning();
    void EndTournoment();
    void CheckPlayerAlive();
    void UpdateGameData(t_GameData *,Game * );
};

#endif