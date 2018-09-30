#include "Message.h"

Message::Message()
{

}
Message::Message(Player p,string s)
{
	this->objPlayer = p;
	this->strMessage = s;
}
Message::~Message()
{

}
void Message::setPlayer(Player p)
{
	this->objPlayer = p;
}

Player Message::getPlayer()
{
	return this->objPlayer;
}
void Message::setMessage(string s)
{
	this->strMessage = s;
}
string Message::getMessage()
{
	return this->strMessage;
}

void Message::setResponse(string s)
{
	this->strResponse = s;
}
string Message::getResponse()
{
	return this->strResponse;
}
void Message::setToAll(bool value)
{
	this->toAll = value;
}
bool Message::getToAll()
{
	return this->toAll;
}