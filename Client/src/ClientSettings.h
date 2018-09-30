#ifndef CLIENTSETTINGS_H
#define CLIENTSETTINGS_H

#include <iostream>
class ClientSettings {
public:
	string name;
	int port;
	string ipAddress;
	int keyUp;
	int keyDown;
	int keyRight;
	int keyLeft;
	int keyRepair;
	int lifeAmount;
	int inmunityTime;
	int tournamentTime;
	int gameTime;
	
};

#endif