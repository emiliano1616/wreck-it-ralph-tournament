#include "Timer.h"


Timer::Timer()
{
	ttf = new TTF(50,400,600,255,255,255);
	isTiming = false;
}

Timer::~Timer()
{
	delete ttf;
}

void Timer::draw(SDL_Surface * pantalla)
{
	SDL_Surface * temp = ttf->getSurface(std::to_string(secondsLeft));

	ttf->rectangulo.x = 400 - (temp->w / 2);
	ttf->rectangulo.y = 637 - (temp->h / 2);
	ttf->rectangulo.h = temp->h;
	ttf->rectangulo.w = temp->w;
	SDL_FillRect(pantalla,&ttf->rectangulo,0x000000);
	SDL_BlitSurface(temp,NULL,pantalla,&ttf->rectangulo);
	lastSecondDrawn = secondsLeft;

}

void Timer::drawGameOver(SDL_Surface * pantalla)
{
	SDL_Surface * temp = ttf->getSurface("GAME OVER");
	ttf->rectangulo.x = 400 - (temp->w / 2);
	ttf->rectangulo.y = 637 - (temp->h / 2);
	ttf->rectangulo.h = temp->h;
	ttf->rectangulo.w = temp->w;
	SDL_FillRect(pantalla,&ttf->rectangulo,0x000000);
	SDL_BlitSurface(temp,NULL,pantalla,&ttf->rectangulo);

	

}

bool Timer::needDraw()
{
	return lastSecondDrawn != secondsLeft;
}

void Timer::startTiming()
{
	isTiming = true;
	while(isTiming)
	{
		if(secondsLeft > 0)
			secondsLeft--;
		sleep(1);
		
	}
}