#include "Game.h"

Game::Game()
{
	this->isFinished = false;
	this->isRunning = false;
}

Game::Game(int id,Player j1, Player j2)
{
	this->id = id;
	
	this->J1 = new Player(j1.getId(),j1.getName(),j1.getLife(),j1.getTCPSocket());
	this->J2 = new Player(j2.getId(),j2.getName(),j2.getLife(),j2.getTCPSocket());
	
	//this->J1 = j1;
	//this->J2 = j2;
	this->isFinished = false;
	this->isRunning = false;
	this->aliveJ1 = true;
	this->aliveJ2 = true;
}

bool Game::getIsFinished()
{
	return this->isFinished;
}
void Game::setIsFinished(bool value)
{
	this->isFinished = value;
}

bool Game::getIsRunning()
{
	return this->isRunning;
}
void Game::setIsRunning(bool value)
{
	this->isRunning = value;
}

Player& Game::WhoWon()
{
	//if(isFinished)
	//{
		if(scorePointsJ1 > scorePointsJ2)
			return *J1;
		if(scorePointsJ1< scorePointsJ2)
			return *J2;
	//	if(scorePointsJ1< scorePointsJ2)
	//		return NULL; //Juego empatado
	//}
	//else
	//{
	//	return NULL;
	//}
}

int Game::getId()
{
	return id;
}
int Game::getScorePlayer1()
{
	return scorePointsJ1;
}

void Game::setScorePlayer1(int score)
{
	this->scorePointsJ1 = score;
}

int Game::getScorePlayer2()
{
	return scorePointsJ2;
}

void Game::setScorePlayer2(int score)
{
	this->scorePointsJ2 = score;
}

int Game::getGameTime()
{
	return this->gameTime;
}

void Game::setGameTime(int value)
{
	this->gameTime = value;
}

Player& Game::getJ1()
{
	return *J1;
}

Player& Game::getJ2()
{
	return *J2;
}



void Game::setAliveJ1(bool value)
{
	this->aliveJ1 = value;
	//this->J1->setKeepConnection(value);
}
void Game::setAliveJ2(bool value)
{
	this->aliveJ2 = value;
	//this->J2->setKeepConnection(value);
}

bool Game::getAliveJ1()
{
	return this->aliveJ1;
}

bool Game::getAliveJ2()
{
	return this->aliveJ2;
}


//Destructor
Game::~Game()
{

}