#ifndef MESSAGE_H
#define MESSAGE_H
#include "Player.h"

class Message
{
    private:

        Player objPlayer;
        string strMessage;
        string strResponse;
        bool toAll;

    public:
    	Message();
    	Message(Player,string);
    	~Message();
    	void setPlayer(Player);
    	Player getPlayer();
    	void setMessage(string);
		string getMessage();
    	void setResponse(string);
    	string getResponse();
    	void setToAll(bool);
    	bool getToAll();
};

#endif