#ifndef GAMESERVER_H
#define GAMESERVER_H

#include "Game.h"
#include "MsgFields.h"
#include <vector>
#include <thread>
#include <mutex>
#include "Socket.h"
#include "Player.h"
#include "Semaphore.h"
#include "Message.h"
#include "Utility.h"
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "SignalHandler.h"
#include <sys/types.h>
#include <sys/wait.h>
#include "Game.h"
#include "Socket.h"
#include <sys/prctl.h>
#define BUFFTAM 1024
#define J1 1
#define J2 2

using namespace std;

class GameServer
{
    private:  
        int ROWS;
        int COLUMNS; 
        int PISOSXNIVEL;
        int posXJ1,posYJ1;
        int posXJ2,posYJ2;
        int pisoActual;
        Game* objGame;
        vector<vector<int>>matrix;
        
        list<Message> *messages;
                
        mutex mutexMessages;   
        mutex mutexProcessMessages;

        bool blnEndGame;
        int intReadyToPlay; 
        int idMemoria;
        t_GameData *gd;

        //Flags para controlar el hilo que recupera msjs
        bool blnEndRetrieveMessage;  
        SignalHandler signalHandler;
        int flagEndDrawLevel;        
        bool isTournomentAlive;

    public:
        //Constructor
        GameServer();
        GameServer(int);
        void MainFunction();
        //MainFunction
        //void MainFunction();
        //Lee el archivo de configuracion
        //void ReadConfigFile();
        string MakeLevel(int);

        void ReceiveMessage(Player);

        void RetrieveMessageFromList();

        void ProcessMessage(Message);

        //Prepara el escenario de juego para que juegen
        void PrepareGameToPlayers();

        void Timer();

        void SendControlMessageToPlayers(int);
        //Actualizamos la memoria compartida datos de la partida (status)
        void UpdateGameData();

        void SendObjects();
        void RefreshPositionPlayer(int,Player);
        void SendPositionToPlayer(int, Player);
        void CheckNextLevel();
        void SendNextLevelToPlayer(Player);
        void SendWindowFixedToPlayer(int, Player);
        bool CheckColision(int,int,int,int);
        void SendLostLife(Player);
        void EndGameServerBySignal();

        void SendObjectsToGame(int,int,int,int);
        //Destructor
        ~GameServer();
        bool IsTournomentAlive();
        void ReleaseResources();
};

#endif



