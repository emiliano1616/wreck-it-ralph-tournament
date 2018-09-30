#include <unistd.h> 
#include <signal.h> 
#include <iostream>  
#include <cstdlib>
#include "GameServer.h"


using namespace std;


int main(int argc, char *argv[]) 
{
	int IdMemoria = stoi(argv[1]);	
	
	//Creamos el GameServer y le pasamos el id de memoria de datos
	GameServer * objGameServer = new GameServer(IdMemoria);

	objGameServer->MainFunction();
	
	delete objGameServer;
}

