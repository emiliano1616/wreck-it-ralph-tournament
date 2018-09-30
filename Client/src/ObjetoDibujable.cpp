#include "ObjetoDibujable.h"

using namespace std;


void ObjetoDibujable::dibujar(SDL_Surface * pantalla, int pisoOffset)
{
	mutexUltimoRectangulo.lock();
    SDL_BlitSurface(imagen,&rectanguloSprites,pantalla,&rectangulo);

	ultimoRectanguloDibujadoConOffset.x = rectangulo.x;
    ultimoRectanguloDibujadoConOffset.y = rectangulo.y + pisoOffset;
    ultimoRectanguloDibujadoSinOffset.x = rectangulo.x;
    ultimoRectanguloDibujadoSinOffset.y = rectangulo.y;
    anteriorSprite = actualSprite;
    primerDibujo = false;
    mutexUltimoRectangulo.unlock();
}

bool ObjetoDibujable::rectangulosChocando(SDL_Rect& a, SDL_Rect& b)
{
  				//Si mi lado izquierdo es mayor al lado derecho de la ventana
	return !((a.x >= b.x  + b.w)
				|| //O si mi lado derecho es menor al lado izquierdo
			(a.x + a.w <= b.x)
				|| //O Si mi parte de abajo, es mas alta que su parte de arriba
			(a.y + a.h <= b.y)
				|| //O Si mi parte de arriba, es mas baja que su parte de abajo)
			(a.y >= b.y + b.h));
}

bool ObjetoDibujable::seMovio()
{
	return ultimoRectanguloDibujadoSinOffset.x != rectangulo.x || ultimoRectanguloDibujadoSinOffset.y != rectangulo.y || anteriorSprite != actualSprite;
}

void ObjetoDibujable::nextSprite() 
{
	actualSprite = actualSprite == (cantidadSprite - 1) ? 0 : actualSprite + 1;
	//if(actualSprite > 0) cout<<actualSprite<<endl;
	rectanguloSprites.x = actualSprite * width;

}

string ObjetoDibujable::getPathBinario()
{
   char buff[1024];
    ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff)-1);
    if (len != -1) {
      buff[len] = '\0';
      string path = (buff);
      int idx = path.find_last_of("/");
      string path2 = path.substr(0,idx);
      string PATH= path2 +"/";//+fileName;
      return PATH;
    } else {
     /* handle error condition */
    	return NULL;
    }
}

//Esto esta pensado para objetos que se aparecen y se mueven una unica vez. Sus coordenadas son seteadas en el constructor
void ObjetoDibujable::mover()
{
	if(rectangulo.x != xfin ||rectangulo.y != yfin)
	{
		thread t(&ObjetoDibujable::refrescar,this);
		t.detach();
	}
}

//Esto esta pensado para objetos como felix o ralph que se mueven constantemente... el thread no deberia iniciarse mas de una vez
void ObjetoDibujable::mover(int x, int y)
{
	
	xfin = x; yfin = y;
	if(rectangulo.x != xfin ||rectangulo.y != yfin)
	{
		thread t(&ObjetoDibujable::refrescar,this);
		t.detach();

	}
	
}

ObjetoDibujable::~ObjetoDibujable()
{
	cout<<"Destructor"<<endl;
	if(imagen)
		SDL_FreeSurface(imagen);
}

ObjetoDibujable::ObjetoDibujable(int px, int py, int pdx, int pdy)
{
	actualSprite = 0;
 	rectangulo.x = px;
	rectangulo.y = py;
	rectanguloSprites.x = 0;
	rectanguloSprites.y = 0;

	xfin = pdx; 
	yfin = pdy;
	ocupado = false;
	ultimoRectanguloDibujadoConOffset.x = -1;
	ultimoRectanguloDibujadoConOffset.y = -1;
	ultimoRectanguloDibujadoSinOffset.x = -1;
	ultimoRectanguloDibujadoSinOffset.y = -1;

	pathBinario = getPathBinario();
	primerDibujo = true;
	primerDesocupado = true;
}

ObjetoDibujable::ObjetoDibujable(int px, int py)
{
	actualSprite = 0;
 	rectangulo.x = px;
	rectangulo.y = py;
	rectanguloSprites.x = 0;
	rectanguloSprites.y = 0;
	ultimoRectanguloDibujadoConOffset.x = -1;
	ultimoRectanguloDibujadoConOffset.y = -1;
	ultimoRectanguloDibujadoSinOffset.x = -1;
	ultimoRectanguloDibujadoSinOffset.y = -1;
	primerDibujo = true;
	primerDesocupado = true;

	xfin = px; 
	yfin = py;
	ocupado = false;
	pathBinario = getPathBinario();
}

bool ObjetoDibujable::igualSigno(int a, int b)
{
	return (a >= 0 && b >= 0) || (a < 0 && b < 0);
}

void ObjetoDibujable::refrescar()
{
	//ObjetoDibujable obj = *this;
	
	ocupado = true;
	int deltaX = (this->xfin - this->rectangulo.x) > 0 ? 1 : (this->xfin - this->rectangulo.x)  == 0 ? 0 : -1;
	int deltaY = (this->yfin - this->rectangulo.y) > 0 ? 1 : (this->yfin - this->rectangulo.y) == 0 ? 0 : -1;

	bool moviX = true; bool moviY = true;
	int i = 0;
	while(moviX || moviY)
	{
		this->mutexUltimoRectangulo.lock();
		if((i % this->refrescoSprite) == 0 )
			this->nextSprite();
		i++;

		moviX = moviY = false;
		if(this->rectangulo.x != this->xfin)
		{
			this->rectangulo.x += deltaX;
			moviX = true;
		}

		if(this->rectangulo.y != this->yfin)
		{
			this->rectangulo.y += deltaY;
			moviY = true;
		}
		this->mutexUltimoRectangulo.unlock();
		usleep(refresco * 1000);
	}


	//cout<<"fin thread -- X: "<< this->rectangulo.x<< " Y: " << this->rectangulo.y <<endl;
	ocupado = false;
}


Gaviota::Gaviota(int px, int py, int pdx, int pdy) :
ObjetoDibujable(px,py,pdx,pdy)
{
	imagen = IMG_Load(this->getPath().c_str());
	if(!imagen)
	{
		puts(SDL_GetError());
		exit(1);
	}
	refresco = 7;
	cantidadSprite = 7;
	refrescoSprite = 10;
	width = 71;
	heigth = 75;
	rectangulo.w = width;
	rectangulo.h = heigth;

	rectanguloSprites.w = width;
	rectanguloSprites.h = heigth;



	ultimoRectanguloDibujadoConOffset.w = width;
	ultimoRectanguloDibujadoConOffset.h = heigth;
	ultimoRectanguloDibujadoSinOffset.w = width;
	ultimoRectanguloDibujadoSinOffset.h = heigth;


}

void Gaviota::setLevelDificulty(int pLevel)
{
	if((refresco - (2 * pLevel)) > 0)
		refresco -= (2 * pLevel);
	else
		refresco = 1;
}

void Ladrillo::setLevelDificulty(int pLevel)
{
	if((refresco - (2 * pLevel)) > 0)
		refresco -= (2 * pLevel);
	else
		refresco = 1;
}


Ladrillo::Ladrillo(int px, int py, int pdx, int pdy) :
ObjetoDibujable(px,py,pdx,pdy)
{
	imagen = IMG_Load(this->getPath().c_str());
	if(!imagen)
	{
		puts(SDL_GetError());
		exit(1);
	}
	refrescoSprite = 5;
	cantidadSprite = 8;

	refresco = 6;
	width = 56;
	heigth = 78;
	rectangulo.w = width;
	rectangulo.h = heigth;


	rectanguloSprites.w = width;
	rectanguloSprites.h = heigth;

	
	ultimoRectanguloDibujadoConOffset.w = width;
	ultimoRectanguloDibujadoConOffset.h = heigth;
	ultimoRectanguloDibujadoSinOffset.w = width;
	ultimoRectanguloDibujadoSinOffset.h = heigth;

}

Ralph::Ralph(int px, int py) :
ObjetoDibujable(px,py)
{
	imagen = IMG_Load(this->getPath().c_str());
	if(!imagen)
	{
		puts(SDL_GetError());
		exit(1);
	}

	refresco = 10;
	refrescoSprite = 8;

	width = 80;
	heigth = 80;

	rectangulo.w = width;
	rectangulo.h = heigth;

	rectanguloSprites.w = width;
	rectanguloSprites.h = heigth;

	rectanguloSprites.x = width * Parado();

	
	ultimoRectanguloDibujadoConOffset.w = width;
	ultimoRectanguloDibujadoConOffset.h = heigth;
	ultimoRectanguloDibujadoSinOffset.w = width;
	ultimoRectanguloDibujadoSinOffset.h = heigth;
}

void Ralph::reiniciar(int px, int py) 
{
	actualSprite = 0;
 	rectangulo.x = px;
	rectangulo.y = py;
	rectanguloSprites.x = 0;
	rectanguloSprites.y = 0;

	xfin = px; 
	yfin = py;
	ocupado = false;
	ultimoRectanguloDibujadoConOffset.x = -1;
	ultimoRectanguloDibujadoConOffset.y = -1;
	ultimoRectanguloDibujadoSinOffset.x = -1;
	ultimoRectanguloDibujadoSinOffset.y = -1;

	rectanguloSprites.x = width * Parado();
}

void Ralph::refrescar()
{

	ocupado = true;
	mutexMovimiento.lock();
	while(true)
	{
		while(!listaMovimientos.empty())
		{
			pair<int,int> par = listaMovimientos.front();
			listaMovimientos.pop_front();
			this->xfin = par.first;
			this->yfin = par.second;

			int deltaX = (this->xfin - this->rectangulo.x) > 0 ? 1 : (this->xfin - this->rectangulo.x)  == 0 ? 0 : -1;
			int deltaY = (this->yfin - this->rectangulo.y) > 0 ? 1 : (this->yfin - this->rectangulo.y) == 0 ? 0 : -1;

			bool moviX = true; bool moviY = true;
			int i = 0;
			while(moviX || moviY)
			{
				if((i % this->refrescoSprite) == 0 )
					this->nextSprite();
				i++;

				moviX = moviY = false;
				if(this->rectangulo.x != this->xfin)
				{
					this->rectangulo.x += deltaX;
					moviX = true;
				}

				if(this->rectangulo.y != this->yfin)
				{
					this->rectangulo.y += deltaY;
					moviY = true;
				}

				usleep(refresco * 1000);
			}
			this->nextSprite();



		//	cout<<"fin thread -- X: "<< this->rectangulo.x<< " Y: " << this->rectangulo.y <<endl;

		}
		mutexMovimiento.lock();

	}
}

void Ralph::mover(int x, int y)
{
	listaMovimientos.push_back(make_pair(x,y));
	mutexMovimiento.unlock();

	if(!this->ocupado)
	{
		thread t(&Ralph::refrescar,this);
		t.detach();
	}
}

void Ralph::nextSprite()
{
	if(xfin > rectangulo.x)
	{
	 	moverDerecha();
	 	return;
	}

	if(xfin < rectangulo.x)
	{
	 	moverIzquierda();
	 	return;
	}

	//No deberia llegar nunca hasta aca... pero por las dudas lo pongo
	rectanguloSprites.x = width * (float)Parado();
	actualSprite = Parado();
}

void Ralph::moverDerecha()
{
	offsetReversa = 0;
	if(actualSprite < CorridaBegin() || actualSprite > CorridaEnd())
		actualSprite = CorridaBegin();

	rectanguloSprites.x = width * actualSprite;
	actualSprite++;
}

void Ralph::moverIzquierda()
{
	offsetReversa = 5;
	if(actualSprite < CorridaBegin() || actualSprite > CorridaEnd())
		actualSprite = CorridaBegin();

	rectanguloSprites.x = width * actualSprite;
	actualSprite++;
}

void Felix::refrescar()
{

	ocupado = true;
	mutexMovimiento.lock();
	while(true)
	{
		while(!listaMovimientos.empty())
		{
			pair<int,int> par = listaMovimientos.front();
			listaMovimientos.pop_front();
			this->xfin = par.first;
			this->yfin = par.second;

			int deltaX = (this->xfin - this->rectangulo.x) > 0 ? 1 : (this->xfin - this->rectangulo.x)  == 0 ? 0 : -1;
			int deltaY = (this->yfin - this->rectangulo.y) > 0 ? 1 : (this->yfin - this->rectangulo.y) == 0 ? 0 : -1;

			bool moviX = true; bool moviY = true;
			int i = 0;
			while(moviX || moviY)
			{
				if((i % this->refrescoSprite) == 0 )
					this->nextSprite();
				i++;

				moviX = moviY = false;
				if(this->rectangulo.x != this->xfin)
				{
					this->rectangulo.x += deltaX;
					moviX = true;
				}

				if(this->rectangulo.y != this->yfin)
				{
					this->rectangulo.y += deltaY;
					moviY = true;
				}

				usleep(refresco * 1000);
			}
			this->nextSprite();

		//	cout<<"fin thread -- X: "<< this->rectangulo.x<< " Y: " << this->rectangulo.y <<endl;

		}

		mutexMovimiento.lock();

	}
}


void Felix::mover(int x, int y)
{
	listaMovimientos.push_back(make_pair(x,y));
	mutexMovimiento.unlock();

	if(!this->ocupado)
	{
		thread t(&Felix::refrescar,this);
		t.detach();
	}
}

void Felix::nextSprite()
{
	if(soyInvulnerable)
	{
		spriteInvulnerable();
		return;
	}

	if(perdiendoVida)
	{
		spritePerderVida();
		return;
	}

	if(xfin > rectangulo.x)
	{
	 	moverDerecha();
	 	return;
	}

	if(xfin < rectangulo.x)
	{
	 	moverIzquierda();
	 	return;
	}

	if(yfin > rectangulo.y)
	{
		bajar();
		return;
	}

	if(yfin < rectangulo.y)
	{
		saltar();
		return;
	}

	//No deberia llegar nunca hasta aca... pero por las dudas lo pongo
	rectanguloSprites.x = width * (float)Parado();
	actualSprite = Parado();
}

void Felix::spriteInvulnerable()
{
	//offsetReversa = 0;
	actualSprite++;
	if((actualSprite - offsetPlayer) >= 14)
		actualSprite = offsetPlayer;

	rectanguloSprites.x = width * actualSprite;

}

void Felix::spritePerderVida()
{
	offsetReversa = 0;
	actualSprite = Perder();
	rectanguloSprites.x = width * (float)Perder();

}

void Felix::moverDerecha()
{
	offsetReversa = 0;
	if(actualSprite == Corrida1())
	{
		actualSprite = Corrida2();
		rectanguloSprites.x = width * (float)Corrida2();
	}
	else
	{
		actualSprite = Corrida1();
		rectanguloSprites.x = width * (float)Corrida1();

	}
	//cout<<"Muevo Derecha... actualsprite: " <<actualSprite<<" width: "<< width<<endl;

}

void Felix::moverIzquierda()
{
	offsetReversa = REVERSEPLAYERSPRITES;
	if(actualSprite == Corrida1())
	{
		actualSprite = Corrida2();
		rectanguloSprites.x = width * (float)Corrida2();
	}
	else
	{
		actualSprite = Corrida1();
		rectanguloSprites.x = width * (float)Corrida1();
	}

}

void Felix::saltar()
{
	//El offsetreversa tiene que quedar igual que en la ultima posicion
	actualSprite = Salto1();
	rectanguloSprites.x = width * (float)Salto1();

}

void Felix::bajar()
{
	//El offsetreversa tiene que quedar igual que en la ultima posicion
	actualSprite = Caida();
	rectanguloSprites.x = width * (float)Caida();

}


Felix::Felix(int x, int y, int pNumeroJugador, string pNombre) :
ObjetoDibujable(x,y)
{
	
	imagen = IMG_Load(getPath().c_str());
	if(!imagen)
	{
		puts(SDL_GetError());
		exit(1);
	}
	perdiendoVida = false;
	soyInvulnerable = false;
	imagenVida = IMG_Load(pNumeroJugador == 1 ? "images/MarioLife.png" : "images/LuigiLife.png");
	if(!imagenVida)
	{
		puts(SDL_GetError());
		exit(1);
	}

	thInvulnerable = NULL;

	rectanguloVida.x = pNumeroJugador == 1 ? 0 : 800 - imagenVida->w;
	rectanguloVida.y = 625;
	rectanguloVida.w = imagenVida->w;
	rectanguloVida.h = imagenVida->h;


	offsetPlayer = pNumeroJugador == 1 ? 0 : SECONDPLAYERSPRITES;
	offsetReversa = pNumeroJugador == 1 ? 0 : REVERSEPLAYERSPRITES;
	numeroJugador = pNumeroJugador;

	ventanasReparadas = 0;
	refresco = 2;
	width = 59;
	heigth = 72;
	rectangulo.w = width;
	rectangulo.h = heigth;

	refrescoSprite = 15;

	fila = 0;
	columna = pNumeroJugador == 1 ? 0 : 4;

	rectanguloSprites.x = width * Parado();
	rectanguloSprites.w = width;
	rectanguloSprites.h = heigth;
	 
	ultimoRectanguloDibujadoConOffset.w = width;
	ultimoRectanguloDibujadoConOffset.h = heigth;
	ultimoRectanguloDibujadoSinOffset.w = width;
	ultimoRectanguloDibujadoSinOffset.h = heigth;


	ultimosPuntosDibujados = -1;
	puntos = 0;

	puntosDibujables = new TTF(25,0,600,255,0,0);
	nombre = pNombre;

	t = NULL;
}

Felix::~Felix()
{
	if(puntosDibujables )
		delete puntosDibujables;
	if(imagenVida)
		SDL_FreeSurface(imagenVida);
	if(t)
		delete t;
}

void Felix::reiniciar(int x, int y, int pNumeroJugador, string pNombre) 
{

	actualSprite = 0;
 	rectangulo.x = x;
	rectangulo.y = y;
	rectanguloSprites.x = 0;
	rectanguloSprites.y = 0;

	xfin = x; 
	yfin = y;
	ocupado = false;
	ultimoRectanguloDibujadoConOffset.x = -1;
	ultimoRectanguloDibujadoConOffset.y = -1;
	ultimoRectanguloDibujadoSinOffset.x = -1;
	ultimoRectanguloDibujadoSinOffset.y = -1;
	ventanasReparadas = 0;
	
	soyInvulnerable = false;
	perdiendoVida = false;
	
	SDL_FreeSurface(imagenVida);
	imagenVida = IMG_Load(pNumeroJugador == 1 ? "images/MarioLife.png" : "images/LuigiLife.png");
	if(!imagenVida)
	{
		puts(SDL_GetError());
		exit(1);
	}

	rectanguloVida.x = pNumeroJugador == 1 ? 0 : 800 - imagenVida->w;
	rectanguloVida.y = 625;
	rectanguloVida.w = imagenVida->w;
	rectanguloVida.h = imagenVida->h;


	offsetPlayer = pNumeroJugador == 1 ? 0 : SECONDPLAYERSPRITES;
	offsetReversa = pNumeroJugador == 1 ? 0 : REVERSEPLAYERSPRITES;
	numeroJugador = pNumeroJugador;

	refresco = 2;
	width = 59;
	heigth = 72;
	rectangulo.w = width;
	rectangulo.h = heigth;

	refrescoSprite = 15;

	fila = 0;
	columna = pNumeroJugador == 1 ? 0 : 4;

	rectanguloSprites.x = width * Parado();
	rectanguloSprites.w = width;
	rectanguloSprites.h = heigth;
	 
	ultimoRectanguloDibujadoConOffset.w = width;
	ultimoRectanguloDibujadoConOffset.h = heigth;
	ultimoRectanguloDibujadoSinOffset.w = width;
	ultimoRectanguloDibujadoSinOffset.h = heigth;


	ultimosPuntosDibujados = -1;
	puntos = 0;

	//puntosDibujables = new TTF(25,0,600,255,0,0);
	nombre = pNombre;

}

void Felix::sumarVentanaReparada(int tiempo)
{
	ventanasReparadas++;

	if((ventanasReparadas % 8) == 0 )
	{	
		soyInvulnerable = true;
		if(thInvulnerable != NULL)
			delete thInvulnerable;
		thInvulnerable = new thread(&Felix::invulnerableThread,this,tiempo);
		thInvulnerable->detach();
	}
}

void Felix::invulnerableThread(int tiempo)
{
	int flag = 0;
	while(flag < tiempo * 10)
	{
		flag++;
		usleep(100 * 1000);
		nextSprite();
	}
	soyInvulnerable = false;
}

void Felix::perderVida()
{
	//mutexPerderVida.lock();
	perdiendoVida = true;
	vidasRestantes = vidasRestantes == 0 ? 0 : vidasRestantes - 1;
	cout<<"Pierdo vida. Vidas Restantes: " << vidasRestantes<<endl;
	nextSprite();
	
	if(t != NULL)
		delete t;
	t = new thread(&Felix::perderVidaThread,this);
	t->detach();
	
	//listaMovimientos.push_back(make_pair(x,y));
	//mutexPerderVida.unlock();

}

void Felix::perderVidaThread()
{
	sleep(3);
	perdiendoVida = false;
}

void Felix::dibujarVidas(SDL_Surface * pantalla)
{
	mutexPuntos.lock();
	SDL_Surface * surfPuntos = puntosDibujables->getSurface(nombre + ": " + to_string(puntos));
	puntosDibujables->rectangulo.x = numeroJugador == 1 ? 0 : 800 - surfPuntos->w;
	//SDL_FillRect(pantalla,&puntosDibujables->rectangulo,0x000000);
	SDL_BlitSurface(surfPuntos,NULL,pantalla,&puntosDibujables->rectangulo);
	ultimosPuntosDibujados = puntos;
	mutexPuntos.unlock();

	vector<SDL_Rect> vec;
	vec.resize(vidasRestantes,rectanguloVida);
	for(int i = vidasRestantes - 1; i >= 0;i-- )
	{
		if(numeroJugador == 1)
			vec[i].x += (i * vec[i].w);
		else
			vec[i].x -= (i * vec[i].w);
		//SDL_FillRect(pantalla,&vec[i],0x000000);
		SDL_BlitSurface(imagenVida,NULL,pantalla,&vec[i]);
	}
}

bool Felix::nuevosPuntos()
{
	return ultimosPuntosDibujados != puntos || ultimasVidasDibujadas != vidasRestantes;
}

void Felix::sumarPuntos(int pPuntos)
{
	mutexPuntos.lock();
	puntos = pPuntos;
	mutexPuntos.unlock();
}



/*



Felix obj(200,200,1);
Felix obj2(300,300,2);




int flag = 1;

void alarm_handler(int a)
{
	flag = 0;
}

	bool quit = false;

void eventos()
{

	SDL_Event event;

	int lastX = 200;
	int lastY = 200;
	int lastX2 = 300;
	int lastY2 = 300;
	//While the user hasn't quit
    while( quit == false )
    {
        //If there's an event to handle
        if( SDL_PollEvent( &event ) )
        {
            //If a key was pressed
            if( event.type == SDL_KEYDOWN )
            {
                //Set the proper message surface
                switch( event.key.keysym.sym )
                {
                    case SDLK_UP:cout<<"up"<<endl; if(! obj.ocupado) {obj.mover(lastX,lastY - 100); lastY -=100 ;obj2.mover(lastX2,lastY2- 100); lastY2 -=100 ;} break;
                    case SDLK_DOWN:cout<<"down"<<endl; if(!obj.ocupado) { obj.mover(lastX,lastY + 100); lastY +=100;obj2.mover(lastX2,lastY2 + 100); lastY2 +=100 ;} break;
                    case SDLK_LEFT:cout<<"left"<<endl; if(!obj.ocupado) {obj.mover(lastX -100,lastY); lastX -=100;obj2.mover(lastX2 - 100,lastY2); lastX2 -=100 ; }break;
                    case SDLK_RIGHT:cout<<"right"<<endl; if(!obj.ocupado) {obj.mover(lastX + 100,lastY); lastX +=100;obj2.mover(lastX2 + 100,lastY2); lastX2 +=100 ;} break;
                    default: break;
                }
            }

            //If the user has Xed out the window
            else if( event.type == SDL_QUIT )
            {
                //Quit the program
                quit = true;
            }
        }
    }


}

int main ()
{
	
	//signal(SIGALRM,alarm_handler);
	//alarm(5);

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		puts(SDL_GetError());
		exit(1);
	}

	atexit(SDL_Quit);

	SDL_Surface * Pantalla = SDL_SetVideoMode(900, 600, 32, SDL_DOUBLEBUF);
	if(!Pantalla)
	{
		puts(SDL_GetError());
		exit(1);
	}
	thread t(&eventos);
	t.detach();



	Gaviota obj1(500,100,0,100);

	Gaviota obj3(500,200,0,200);
	//Gaviota obj4(500,300,0,300);
	Gaviota obj7(0,100,500,100);
	Ladrillo obj2(0,200,500,200);
	//Gaviota obj8(0,300,500,300);

	Ralph obj5(500,500,0,500);
	Ralph obj6(0,500,500,500);

	Ralph obj9(0,0,500,0);
	Ralph obj10(500,0,0,0);
	Ralph obj11(500,400,0,400);
	Ralph obj12(0,400,500,400);

	obj1.mover();
	obj2.mover();
	obj3.mover();
	//obj4.mover();
	obj5.mover();
	obj6.mover();
	obj7.mover();
	//obj8.mover();
	obj9.mover();
	obj10.mover();
	obj11.mover();
	obj12.mover();

	while(quit == false)
	{

		SDL_FillRect(Pantalla,NULL,0x000000);

		SDL_BlitSurface(obj.imagen,&obj.rectanguloSprites,Pantalla,&obj.rectangulo);
		SDL_BlitSurface(obj2.imagen,&obj2.rectanguloSprites,Pantalla,&obj2.rectangulo);

		
	    SDL_BlitSurface(obj1.imagen,&obj1.rectanguloSprites,Pantalla,&obj1.rectangulo);
	    SDL_BlitSurface(obj2.imagen,&obj2.rectanguloSprites,Pantalla,&obj2.rectangulo);
	    SDL_BlitSurface(obj3.imagen,&obj3.rectanguloSprites,Pantalla,&obj3.rectangulo);
	    //SDL_BlitSurface(obj4.imagen,&obj4.rectanguloSprites,Pantalla,&obj4.rectangulo);

	    SDL_BlitSurface(obj9.imagen,&obj9.rectanguloSprites,Pantalla,&obj9.rectangulo);
	    SDL_BlitSurface(obj10.imagen,&obj10.rectanguloSprites,Pantalla,&obj10.rectangulo);
	    SDL_BlitSurface(obj11.imagen,&obj11.rectanguloSprites,Pantalla,&obj11.rectangulo);
	    SDL_BlitSurface(obj12.imagen,&obj12.rectanguloSprites,Pantalla,&obj12.rectangulo);
	    

	    SDL_BlitSurface(obj5.imagen,&obj5.rectanguloSprites,Pantalla,&obj5.rectangulo);
	    SDL_BlitSurface(obj6.imagen,&obj6.rectanguloSprites,Pantalla,&obj6.rectangulo);
	    SDL_BlitSurface(obj7.imagen,&obj7.rectanguloSprites,Pantalla,&obj7.rectangulo);
	    //SDL_BlitSurface(obj8.imagen,&obj8.rectanguloSprites,Pantalla,&obj8.rectangulo);
	    
	    SDL_Flip(Pantalla);
	    usleep(15 * 1000);

	}

	if(Pantalla)
		SDL_FreeSurface(Pantalla);

	return 0;
}
*/