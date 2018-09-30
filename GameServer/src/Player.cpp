#include "Player.h"

Player::Player()
{

}

Player::Player(int id)
{
	this->id = id;
}

Player::Player(int id,string str,int life)
{
    this->id = id;
    this->name = str;
    this->life = life;
}
Player::Player(int id ,string str ,int life,TCPSocket* sock)
{
	this->id = id;
    this->name = str;
    this->life = life;
    this->socket = sock;   
    readyToPlay = false; 
}

void Player::setId(int id)
{
	this->id = id;
}

int Player::getId()
{
	return this->id;
}

void Player::setName(string name)
{
	this->name = name;
}

string Player::getName()
{
	return this->name;
}

void Player::setLife(int cant)
{
	this->life = cant;
}

int Player::getLife()
{
	return this->life;
}

void Player::setSocket(TCPSocket *socket)
{
	this->socket = socket;
}

int Player::getSocketDescriptor()
{
	return this->socket->getSocketDescriptor();
}

TCPSocket* Player::getTCPSocket()
{
	return this->socket;
}

bool Player::getKeepConnection()
{
	return this->keepConnection;
}
void Player::setKeepConnection(bool value)
{
	this->keepConnection = value;
}


bool Player::getReadyToPlay()
{
	return this->readyToPlay;
}

void Player::setReadyToPlay(bool value)
{
	this->readyToPlay = value;
}

void Player::lostLife()
{
	this->life--;
}
Player::~Player()
{
}
