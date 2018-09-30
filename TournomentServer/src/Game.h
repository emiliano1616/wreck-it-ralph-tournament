#ifndef GAME_H
#define GAME_H

#include <list>
#include <iostream>
#include <string>
#include "Player.h"


using namespace std;


class Game
{
    private:
        int id;
        Player *J1;
        Player *J2;       
        
		//Tiempo de juego en segundos
		int gameTime;
		//puntos del jugador 1
		int scorePointsJ1;
		//puntos del jugador 2         
		int scorePointsJ2;
        //string qe contiene el versus J1vsJ2       
        //indica si se ha terminado el juego
        bool isFinished;
        //Indica si se esta ejecutando el juego
        bool isRunning;

        bool aliveJ1;
        bool aliveJ2;   
        Player *pp;
 

    public:
        Game();
        Game(int,Player, Player);

        int getId();
        //pregunta si ha finalizado el juego
        bool HasEnd();
        //indica quien gano o null sino ha terminado
        Player& WhoWon();
        //Getter & setters score p1
        int getScorePlayer1();
        void setScorePlayer1(int);
		//Getter & setters score p1
		int getScorePlayer2();
        void setScorePlayer2(int);
       	//Termino el juego;
        bool getIsFinished();
        void setIsFinished(bool);

        bool getIsRunning();
        void setIsRunning(bool);

        Player& getJ1();
        Player& getJ2();

        int getGameTime();
        void setGameTime(int);


        void setAliveJ1(bool value);
        void setAliveJ2(bool value);

        bool getAliveJ1();
        bool getAliveJ2();
       


        //Destructor
        ~Game();
};



typedef struct 
{
    int gameId;
    int gameTime;
    int J1Id;
    int J2Id;
    char J1Name[200];
    char J2Name[200];
    int J1Lifes;
    int J2Lifes;
    int J1Socket;
    int J2Socket;
    int J1ScorePoints;
    int J2ScorePoints;
    bool endGame;
    bool J1Alive;
    bool J2Alive;
}t_GameData;
#endif
