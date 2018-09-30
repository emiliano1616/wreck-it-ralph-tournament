#ifndef PLAYER_H
#define PLAYER_H

#include <list>
#include <iostream>
#include <string>
#include "Socket.h"

using namespace std;


class Player
{
    private:
        int id;
        string name;
        int life;       
        TCPSocket *socket;
        bool keepConnection;
        bool readyToPlay;
    public:
        Player();
        Player(int);
        Player(int,string,int);
        Player(int,string,int,TCPSocket*);
        void setId(int);
        int getId();
        void setName(string);
        string getName();
        void setLife(int);
        int getLife();
        void setSocket(TCPSocket *);
        TCPSocket* getTCPSocket();
        int getSocketDescriptor();

        bool getKeepConnection();
        void setKeepConnection(bool);
        
        bool getReadyToPlay();
        void setReadyToPlay(bool);
        void lostLife();

        //Destructor
        ~Player();
};

#endif

