#ifndef TIMER_H
#define TIMER_H
#include <SDL/SDL.h>
#include <string>
#include "TTF.h"


class Timer{
private:
	int lastSecondDrawn;
	SDL_Rect rectangulo;
	TTF * ttf;
public:
	bool isTiming;
	int secondsLeft;
	void startTiming();
	bool needDraw();
	void drawGameOver(SDL_Surface *);
	void draw(SDL_Surface *);
	Timer();
	~Timer();
};

#endif